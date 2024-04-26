#include <rtconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <finsh.h>
#include <string.h>
#include "aic_core.h"
#include <env.h>
#include <absystem.h>
#include <dfs_posix.h>
// #define OTA_DOWNLOADER_DEBUG

#ifdef AIC_SPINOR_DRV
#include <fal.h>

/* the address offset of download partition */
#ifndef RT_USING_FAL
#error "Please enable and confirgure FAL part."
#endif /* RT_USING_FAL */

const struct fal_partition *dl_part = RT_NULL;
#endif

#ifdef AIC_SPINAND_DRV
struct rt_mtd_nand_device *nand_mtd;
rt_device_t nand_dev;
#endif

#define DBG_ENABLE
#define DBG_SECTION_NAME "http_ota"
#ifdef OTA_DOWNLOADER_DEBUG
#define DBG_LEVEL DBG_LOG
#else
#define DBG_LEVEL DBG_INFO
#endif
#define DBG_COLOR
#include <rtdbg.h>


#define ALIGN_xB_UP(x, y) (((x) + (y - 1)) & ~(y - 1))

#define MAX_CPIO_FILE_NAME 18


#define MAX_IMAGE_FNAME 32

/*
 * OTA upgrade has two caches
 * OTA unpacking requires one cache because the cpio header info may be split into two transfer,
 * determine by USER_OTA_HEAD_LEN
 * Burning requires a cache because burning requires address and length align,
 * determine by USER_OTA_BURN_BUFF_LEN
 */
#define USER_OTA_BURN_BUFF_LEN (2048 * 2)
#define USER_OTA_BURN_LEN      2048
#define USER_OTA_HEAD_LEN      (2048 * 2)
#define USER_OTA_BUFF_LEN      (4096 / 2)

enum flag_cpio {
    FLAG_CPIO_HEAD1,
    FLAG_CPIO_HEAD2,
    FLAG_CPIO_HEAD3,
    FLAG_CPIO_HEAD4,
    FLAG_CPIO_HEAD,
    FLAG_CPIO_FILE,
};

struct filehdr {
    unsigned int format;
    unsigned int size;              //当前升级文件分区的大小    os  或  rodata
    unsigned int size_align;        //文件内存对齐 所需的大小
    unsigned int begin_offset;      //
    unsigned int cpio_header_len;
    unsigned int namesize;          //
    unsigned int filename_size;     //文件名大小
    unsigned int filename_align;    //文件名内存对齐 所需的字节 
    unsigned int burn_len;
    unsigned int chksum;
    unsigned int sum;
    char filename[MAX_IMAGE_FNAME];
};

struct bufhdr {
    int buflen; //有效数据大小
    int head;   //有效数据的起始地址
    int size;   //缓存大小
    char *buf;
};

enum cpio_fields {
    C_MAGIC,
    C_INO,
    C_MODE,
    C_UID,
    C_GID,
    C_NLINK,
    C_MTIME,
    C_FILESIZE,
    C_MAJ,
    C_MIN,
    C_RMAJ,
    C_RMIN,
    C_NAMESIZE,
    C_CHKSUM,
    C_NFIELDS
};

#ifdef OTA_DOWNLOADER_DEBUG
static unsigned int test_sum = 0;
static unsigned int test_leng = 0;
#endif

static int file_offset = 0; /*接收到的数据偏移量*/

static unsigned char flag_cpio = FLAG_CPIO_HEAD1; /*升级文件名索引*/
static unsigned char cpio_or_file = FLAG_CPIO_HEAD;
/*parse header info or file content*/

static struct bufhdr bhdr = { 0 };  /*烧录缓冲区*/
static struct bufhdr shdr = { 0 };  /*头部缓冲器*/
static struct filehdr fhdr = { 0 }; /*升级文件信息*/

unsigned int cpio_file_checksum(unsigned char *buffer, unsigned int length)
{
    unsigned int sum = 0;
    int i = 0;

    for (i = 0; i < length; i++)
        sum += buffer[i];

#ifdef OTA_DOWNLOADER_DEBUG
    test_sum += sum;
    test_leng += length;
    // printf("%s sum = 0x%x length = %d\n", __func__, test_sum, test_leng);
#endif
    return sum;
}

static void ota_buf_init(struct bufhdr *hdr, char *buf, int size)
{
    hdr->buf = buf;
    hdr->size = size;
    hdr->buflen = 0;
    hdr->head = 0;
}

static int ota_buf_push(struct bufhdr *hdr, char *data, int len)
{
    if ((hdr->buflen + len) > hdr->size) {
        LOG_E("Queue overflow,please increase buffer size\n");
        return -RT_ERROR;
    }

    rt_memcpy(hdr->buf + hdr->buflen, data, len);
    hdr->buflen += len;

#ifdef OTA_DOWNLOADER_DEBUG
    int i;
    printf("%s:\n", __func__);
    for (i = 0; i < 20; i++)
        printf("0x%x ", data[i]);
    printf("\n");

    printf("%s hdr->buflen = %d\n", __func__, hdr->buflen);
#endif

    return RT_EOK;
}
volatile uint8_t user_ota_per = 0;
static void print_progress(size_t cur_size, size_t total_size)
{
    static unsigned char progress_sign[100 + 1];
    uint8_t i, per = cur_size * 100 / total_size;
    static unsigned char per_size = 0;

    if (per > 100) {
        per = 100;
    }

    if (per_size == per)
        return;

    for (i = 0; i < 100; i++) {
        if (i < per) {
            progress_sign[i] = '=';
        } else if (per == i) {
            progress_sign[i] = '>';
        } else {
            progress_sign[i] = ' ';
        }
    }

    progress_sign[sizeof(progress_sign) - 1] = '\0';

    // LOG_I("Download: [%s] %03d%%\033[1A", progress_sign, per);
    printf("Download: [%s] %03d%%\033[1A\r\n", progress_sign, per);

    user_ota_per = per;


    per_size = per;
}

static int aic_ota_find_part(char *partname)
{
    switch (aic_get_boot_device()) {
#ifdef AIC_SPINOR_DRV
        case BD_SPINOR:
            /* Get download partition information and erase download partition data */
            if ((dl_part = fal_partition_find(partname)) == RT_NULL) {
                LOG_E("Firmware download failed! Partition (%s) find error!",
                      partname);
                return -RT_ERROR;
            }
            break;
#endif
#ifdef AIC_SPINAND_DRV
        case BD_SPINAND:
            nand_dev = rt_device_find(partname);
            if (nand_dev == RT_NULL) {
                LOG_E("Firmware download failed! Partition (%s) find error!",
                      partname);
                return -RT_ERROR;
            }

            nand_mtd = (struct rt_mtd_nand_device *)nand_dev;
            break;
#endif
        default:
            return -RT_ERROR;
            break;
    }

    LOG_I("Partition (%s) find success!", partname);
    return 0;
}

#ifdef AIC_SPINOR_DRV
static int aic_ota_nor_erase_part(void)
{
    LOG_I("Start erase flash (%s) partition!", dl_part->name);
    if (fal_partition_erase(dl_part, 0, dl_part->len) < 0) {
        LOG_E("Firmware download failed! Partition (%s) erase error! len = %d",
              dl_part->name, dl_part->len);
        return -RT_ERROR;
    }
    LOG_I("Erase flash (%s) partition success! len = %d", dl_part->name,
          dl_part->len);
    return 0;
}
#endif

#ifdef AIC_SPINAND_DRV
static int aic_ota_nand_erase_part(void)
{
    unsigned long blk_offset = 0;

    LOG_I("Start erase nand flash partition!");

    while (nand_mtd->block_total > blk_offset) {
        if (rt_mtd_nand_check_block(nand_mtd, blk_offset) != RT_EOK) {
            LOG_W("Erase block is bad, skip it.\n");
            blk_offset++;
            continue;
        }

        rt_mtd_nand_erase_block(nand_mtd, blk_offset);
        blk_offset++;
    }

    LOG_I("Erase nand flash partition success! len = %d",
          nand_mtd->block_total);

    return 0;
}

static int aic_ota_nand_write(uint32_t addr, const uint8_t *buf, size_t size)
{
    unsigned long blk = 0, offset = 0, page = 0;
    static unsigned long bad_block_off = 0;
    unsigned long blk_size = nand_mtd->pages_per_block * nand_mtd->page_size;
    rt_err_t ret = 0;

    if (size > 2048) {
        LOG_E("USER_OTA_BURN_LEN need set 2048! size = %d", size);
        return -RT_ERROR;
    }

    ret = rt_device_open(nand_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret) {
        LOG_E("Open MTD device failed.!\n");
        return ret;
    }

    offset = addr + bad_block_off;

    /* Search for the first good block after the given offset */
    if (offset % blk_size == 0) {
        blk = offset / blk_size;
        while (rt_mtd_nand_check_block(nand_mtd, blk) != RT_EOK) {
            LOG_W("find a bad block, off adjust to the next block\n");
            bad_block_off += nand_mtd->pages_per_block;
            offset = addr + bad_block_off;
            blk = offset / blk_size;
        }
    }

    page = offset / nand_mtd->page_size;
    ret = rt_mtd_nand_write(nand_mtd, page, buf, size, RT_NULL, 0);
    if (ret) {
        LOG_E("Failed to write data to NAND.\n");
        ret = -RT_ERROR;
        goto aic_ota_nand_write_exit;
    }

aic_ota_nand_write_exit:
    rt_device_close(nand_dev);

    return 0;
}
#endif

static int aic_ota_erase_part(void)
{
    int ret = 0;

    switch (aic_get_boot_device()) {
#ifdef AIC_SPINOR_DRV
        case BD_SPINOR:
            ret = aic_ota_nor_erase_part();
            break;
#endif
#ifdef AIC_SPINAND_DRV
        case BD_SPINAND:
            ret = aic_ota_nand_erase_part();
            break;
#endif
        default:
            break;
    }

    return ret;
}

static int aic_ota_part_write(uint32_t addr, const uint8_t *buf, size_t size)
{
    int ret = 0;

    switch (aic_get_boot_device()) {
#ifdef AIC_SPINOR_DRV
        case BD_SPINOR:
            ret = fal_partition_write(dl_part, addr, buf, size);
            if (ret < 0) {
                LOG_E(
                    "Firmware download failed! Partition (%s) write data error!",
                    dl_part->name);
                return -RT_ERROR;
            }
            break;
#endif
#ifdef AIC_SPINAND_DRV
        case BD_SPINAND:
            ret = aic_ota_nand_write(addr, buf, size);
            if (ret < 0) {
                LOG_E(
                    "Firmware download failed! nand partition write data error!");
                return -RT_ERROR;
            }
            break;
#endif
        default:
            return -RT_ERROR;
            break;
    }

    return ret;
}

/*
 * 如果缓冲区中的有效数据超过 USER_OTA_BURN_LEN，
 * 开始烧录数据
 * 
 */
static int download_buf_pop(struct bufhdr *bhdr, struct filehdr *fhdr)
{
    int ret = RT_EOK;
    int burn_len = 0;
    int burn_last = 0;

download_buf_pop_last:
    /*Last upgrade data*/
    if (fhdr->size - fhdr->begin_offset < bhdr->buflen) {
        /*upgrade in two stags*/
        if (fhdr->size - fhdr->begin_offset > USER_OTA_BURN_LEN) {
            burn_len = USER_OTA_BURN_LEN;
            burn_last = 1;
        } else if (fhdr->size - fhdr->begin_offset == USER_OTA_BURN_LEN) {
            burn_len = USER_OTA_BURN_LEN;
#ifdef OTA_DOWNLOADER_DEBUG
            LOG_I("Burn the last data size = %d!", USER_OTA_BURN_LEN);  //烧录最后一个数据大小
#endif
        } else {
            burn_len = fhdr->size - fhdr->begin_offset;
#ifdef OTA_DOWNLOADER_DEBUG
            LOG_I("Burn the last data size = %d!", burn_len);
#endif
        }
    } else {
        /*当缓冲区 len 不足时，下次接收数据后再处理*/
        if (bhdr->buflen < USER_OTA_BURN_LEN)
            return 0;
        else
            burn_len = USER_OTA_BURN_LEN;
    }

    /* 将数据写入对应的分区地址 */
    ret = aic_ota_part_write(fhdr->begin_offset, (const uint8_t *)bhdr->buf,
                             burn_len);
    if (ret)
        return -RT_ERROR;

    fhdr->sum += cpio_file_checksum((unsigned char *)bhdr->buf, burn_len);

    fhdr->begin_offset += burn_len;
    // print_progress(fhdr->begin_offset, fhdr->size);

    if (burn_len == USER_OTA_BURN_LEN) {
        rt_memcpy(bhdr->buf, bhdr->buf + USER_OTA_BURN_LEN,
                  bhdr->buflen - USER_OTA_BURN_LEN);
        bhdr->buflen -= USER_OTA_BURN_LEN;
        if (fhdr->size - fhdr->begin_offset <= 0) {
            bhdr->head += fhdr->size_align;
            bhdr->buflen -= fhdr->size_align;
            /*将 BHDR 所有数据传输到 SHDR*/
            ret = ota_buf_push(&shdr, bhdr->buf + bhdr->head, bhdr->buflen);
            if (ret < 0) {
                return ret;
            } else {
                /*BHDR 所有数据都已传递给 SHDR*/
                bhdr->buflen = 0;
                bhdr->head = 0;
            }
        }
    } else {
        bhdr->head += (burn_len + fhdr->size_align);
        bhdr->buflen -= (burn_len + fhdr->size_align);

        /*将 BHDR 所有数据传输到 SHDR*/
        ret = ota_buf_push(&shdr, bhdr->buf + bhdr->head, bhdr->buflen);
        if (ret < 0) {
            return ret;
        } else {
            /*BHDR 所有数据都已传递给 SHDR*/
            bhdr->buflen = 0;
            bhdr->head = 0;
        }
    }

    /* 开始升级剩余数据 */
    if (burn_last == 1) {
        burn_last = 0;
        goto download_buf_pop_last;
    }

    return 0;
}

/*
 * find_cpio_data - 在未压缩的 cpio 中搜索文件
 * @fhdr:     struct filehdr containing the address, length and
 *              filename (with the directory path cut off) of the found file.
 * @data:       指向 cpio 存档或内部标头的指针
 * @len:        基于数据指针的 cpio 的剩余长度
 * @return:     0 is success,others is failed
 */
int find_cpio_data(struct filehdr *fhdr, void *data, size_t len)
{
    const char *p;
    unsigned int *chp, v;
    unsigned char c, x;
    int i, j;
    unsigned int ch[C_NFIELDS];
    int file_end_offset = 0;
    int cpio_header_len = 0;

    fhdr->cpio_header_len = 8 * C_NFIELDS - 2;

    p = data;

    if (!*p) {
        /* 所有 cpio 标头都需要 4 字节对齐 */
        LOG_I("*p = 0x%x\n", *p);
        p += 4;
        len -= 4;
    }

    j = 6; /* The magic field 只有 6 个字符 */
    chp = ch;
    for (i = C_NFIELDS; i; i--) {
        v = 0;
        while (j--) {
            v <<= 4;
            c = *p++;

            x = c - '0';
            if (x < 10) {
                v += x;
                continue;
            }

            x = (c | 0x20) - 'a';
            if (x < 6) {
                v += x + 10;
                continue;
            }

            LOG_E("error 1 Invalid hexadecimal\n");
            goto quit; /* Invalid hexadecimal */
        }
        *chp++ = v;
        j = 8; /* 所有其他字段均为 8 个字符 */
    }

    if ((ch[C_MAGIC] - 0x070701) > 1) {
        LOG_E("error 2 Invalid magic\n");
        goto quit; /* Invalid magic */
    }

#ifdef OTA_DOWNLOADER_DEBUG
    for (i = 0; i < 14; i++)
        printf("ch[%d] = 0x%x\n", i, ch[i]);
#endif

    if ((ch[C_MODE] & 0170000) == 0100000) {
        if (ch[C_NAMESIZE] >= MAX_CPIO_FILE_NAME) {
            LOG_E("File %s exceeding MAX_CPIO_FILE_NAME [%d]\n", p,
                  MAX_CPIO_FILE_NAME);
        }
        strncpy(fhdr->filename, p, ch[C_NAMESIZE]);

        fhdr->format = ch[C_MAGIC];
        fhdr->size = ch[C_FILESIZE]; /* 升级文件大小 */
        fhdr->begin_offset = 0;
        fhdr->filename_size = ch[C_NAMESIZE];
        fhdr->chksum = ch[C_CHKSUM];
        fhdr->sum = 0;

        /* 文件名对齐偏移量 */
        file_end_offset = fhdr->cpio_header_len + fhdr->filename_size;
        fhdr->filename_align =
            ALIGN_xB_UP(file_end_offset, 4) - file_end_offset;
#ifdef OTA_DOWNLOADER_DEBUG
        printf("fhdr->filename_align = %d %d %d\n", fhdr->filename_align,
               file_end_offset, ALIGN_xB_UP(file_end_offset, 4));
#endif

        /* 解析cpio标头信息，文件名和对齐数据，如果解析不完整，则进行下一步解析*/
        cpio_header_len =
            fhdr->cpio_header_len + fhdr->filename_size + fhdr->filename_align;
        if (len < cpio_header_len) {
            LOG_E("Incomplete filename and align data!\n");
            return -1;
        }

        /*file align offset*/
        file_end_offset = fhdr->size;
        fhdr->size_align = ALIGN_xB_UP(file_end_offset, 4) - file_end_offset;

#ifdef OTA_DOWNLOADER_DEBUG
        printf("fhdr->size_align = %d %d %d\n", fhdr->size_align,
               file_end_offset, ALIGN_xB_UP(file_end_offset, 4));
        test_sum = 0;
        test_leng = 0;
#endif

        LOG_I("find file %s cpio data success\n", fhdr->filename);
        return 0; /* Found it! */
    } else {
        strncpy(fhdr->filename, p, ch[C_NAMESIZE]);
        fhdr->filename_size = ch[C_NAMESIZE];
        LOG_I("find file %s cpio data success\n", fhdr->filename);
        return 0; /* Found it! */
    }

quit:
    LOG_E("find file in cpio data failed\n");
    return -1;
}

/*
 * 删除 cpio 标头信息、文件名和对齐数据
 * 然后将 shdr 剩余数据传输到 bhdr
 */
static int head_buf_pop(struct bufhdr *bhdr, struct bufhdr *shdr,
                        struct filehdr *fhdr)
{
    int ret = RT_EOK;
    int len =
        fhdr->cpio_header_len + fhdr->filename_size + fhdr->filename_align;

    /*删除 cpio 标头信息*/
    shdr->head += len;
    shdr->buflen -= len;

    if (shdr->buflen > 0) {
#ifdef OTA_DOWNLOADER_DEBUG
        int i;
        printf("%s shdr->buflen = %d, len = %d\n", __func__, shdr->buflen, len);
        printf("%s:\n", __func__);
        for (i = 0; i < 20; i++)
            printf("0x%x ", (shdr->buf + shdr->head)[i]);
#endif
        ret = ota_buf_push(bhdr, shdr->buf + shdr->head, shdr->buflen);
        if (ret)
            return ret;

        shdr->buflen = 0;
        shdr->head = 0;
    } else {
        shdr->head = 0;
    }

    return ret;
}


/* 句柄功能，可以存储数据等 每次读2048字节，调用此函数 */
static int ota_shard_download_handle(char *buffer, int length)
{
    char buffer_tmp[2048] = {0};
    memcpy(buffer_tmp, buffer, length);
    int ret = RT_EOK;
    int len = 8 * C_NFIELDS - 2;
    char *partname = NULL;

    file_offset += length;
    // LOG_I("header length = %d", length);
    /*
     * 开始默认解析 cpio 标头信息、文件名和对齐数据
     * 然后，解析文件内容
     */
    if (cpio_or_file != FLAG_CPIO_FILE) {
        ret = ota_buf_push(&shdr, buffer_tmp, length);      //ota获取文件内容(包含头部信息)
        if (ret)
            goto __download_exit;

    ota_shard_download_handle_last:

        if (shdr.buflen < len) {
            LOG_I("Incomplete header info! shdr.buflen = %d", shdr.buflen);
            goto __download_exit;
        }

        /*
         * 查找完整的标题信息，如果没有，请直接返回
         */
        ret = find_cpio_data(&fhdr, shdr.buf, shdr.buflen);
        if (ret < 0) {
            LOG_E("Not find file info\n");
            goto __download_exit;
        }

        /*拖车！！！是最后一个文件，解压结束*/
        ret = rt_strncmp(fhdr.filename, "TRAILER!!!", fhdr.filename_size);
        if (ret == 0) {
            goto __download_exit;
        }

        ret = head_buf_pop(&bhdr, &shdr, &fhdr);    //去除头部信息，将剩余有效数据放入 bhdr 中
        if (ret < 0) {
            LOG_E("head_buf_pop error!\n");
            goto __download_exit;
        }

        partname = aic_upgrade_get_partname(flag_cpio);

        ret = aic_ota_find_part(partname);
        if (ret)
            goto __download_exit;

        ret = aic_ota_erase_part();
        if (ret)
            goto __download_exit;

        LOG_I("Start upgrade %s!", fhdr.filename);

        ret = download_buf_pop(&bhdr, &fhdr);       //将有效数据 bhdr ，写入flash
        if (ret < 0) {
            LOG_E("download_buf_pop error! len = %d\n", len);
            goto __download_exit;
        }

        cpio_or_file = FLAG_CPIO_FILE;
    } else { /*解析文件内容*/
        ret = ota_buf_push(&bhdr, buffer_tmp, length);  //ota获取文件内容
        if (ret)
            goto __download_exit;

        ret = download_buf_pop(&bhdr, &fhdr);   //将有效数据 bhdr ，写入flash
        if (ret < 0) {
            LOG_E("download_buf_pop error! len = %d\n", len);
            goto __download_exit;
        }

        if (fhdr.size <= fhdr.begin_offset) {
#ifdef OTA_DOWNLOADER_DEBUG
            LOG_I("fhdr.size = %d fhdr.begin_offset = %d\n", fhdr.size,
                  fhdr.begin_offset);
#endif
            if (fhdr.sum == fhdr.chksum) {
                LOG_I("Sum check success!");
                LOG_I("download %s success!\n", fhdr.filename);
            } else {
                LOG_E(
                    "Sum check failed, fhdr->sum = 0x%x,fhdr->chksum = 0x%x\n",
                    fhdr.sum, fhdr.chksum);
                goto __download_exit;
            }

            cpio_or_file = FLAG_CPIO_HEAD;
            flag_cpio++;

            goto ota_shard_download_handle_last;
        }
    }

__download_exit:
    return ret;
}











#include <sys/time.h>
static float time_diff(struct timespec *start, struct timespec *end)
{
    float diff;
#define NS_PER_SEC      1000000000

    if (end->tv_nsec < start->tv_nsec) {
        diff = (float)(NS_PER_SEC + end->tv_nsec - start->tv_nsec)/NS_PER_SEC;
        diff += end->tv_sec - 1 - start->tv_sec;
    } else {
        diff = (float)(end->tv_nsec - start->tv_nsec)/NS_PER_SEC;
        diff += end->tv_sec - start->tv_sec;
    }

    return diff;
}

static struct timespec start = {0}, end = {0};





/* UART Register Offsets. */

static struct rt_semaphore rx_sem = {0};
static rt_device_t serial = NULL;


#define UPFILE_TYPE_CPIO 0  //文件类型

#define UUP_PACKET_SIZE		2048
#define UUP_MAX_FRAME_LEN	(UUP_PACKET_SIZE + 16)
#define OTA_RX_FRAME_NUM	8
static unsigned char uart_rx_buf[OTA_RX_FRAME_NUM][UUP_MAX_FRAME_LEN];
static unsigned char *uart_rx_ptr;
static int uart_rx_head = 0;
static int uart_rx_tail = 0;


typedef enum {
	UART_FRAME_START,
	UART_FRAME_FILEINFO,
	UART_FRAME_FILEXFER,
	UART_FRAME_FINISH,
} eUartFrameType;

typedef enum {
	OTA_STATE_IDLE,
	OTA_STATE_START,
	OTA_STATE_GET_FILEINFO,
	OTA_STATE_FILE_TFR,
	OTA_STATE_END,
} eOtaUpdateState;

#define UART_ACK_OK			1
#define UART_ACK_FAIL		0

#define OTA_MAX_FILE_SIZE	0x2000000

static int ota_status = OTA_STATE_IDLE;
static int ota_file_type = 0;
static int ota_file_size = 0;
static int ota_rev_size = 0;
static int ota_rev_packet = 0;
static int ota_rev_len = 0;

/*
 *	应答包：
 *	0x55 0x80 0x02(数据域长度) 
 *	应答帧类型(0x00开始升级 0x01文件信息 0x02文件传输 0x03传输结束) 返回结果（0x00失败 0x01成功)
 * 	校验字节
 */ 		
static void ota_send_ack(int type, int ret)
{
	unsigned char buf[7] = {0x55, 0x80, 0xc5, 0x02, 0x00, 0x00, 0x00};
	int i;

	buf[4] = type;
	buf[5] = ret;
	for (i = 1; i < 6; i++)
		buf[6] ^= buf[i];

    rt_device_write(serial, 0, buf, 7);
}

#define OTA_BUFFER_LEN 8192
static uint8_t *ota_buffer;

static void _ota_update(uint8_t *framebuf, size_t len)
{
	// framebuf 指向数据域
	int frametype = framebuf[0];	//帧类型(0x00开始升级 0x01文件信息 0x02文件传输 0x03传输结束)
	uint8_t *buf = framebuf + 1;	//数据域
	unsigned int framelen;
	unsigned int packetnum;			//数据包序数

	switch (frametype) 
	{
		//接收到开始信号
		case UART_FRAME_START:
			ota_send_ack(frametype, UART_ACK_OK);
			ota_status = OTA_STATE_START;
			break;

		//接收文件信息
		case UART_FRAME_FILEINFO:
			if (ota_status != OTA_STATE_START && ota_status != OTA_STATE_GET_FILEINFO) {
				ota_send_ack(frametype, UART_ACK_FAIL);
				break;
			}
			ota_rev_len = 0;		//接收字节长度
			ota_rev_packet = 0;		//接收包数量
            ota_rev_size = 0;

			ota_file_type = buf[0];		//文件类型
            printf("ota_file_type %d.\n", ota_file_type);
			if (ota_file_type != UPFILE_TYPE_CPIO) {
				printf("Rev wrong file type %d.\n", ota_file_type);
				ota_send_ack(frametype, UART_ACK_FAIL);
				break;
			}

			ota_file_size = (buf[1] << 24) | (buf[2] << 16) | (buf[3] << 8) | buf[4];	//文件数据包个数  ota_packet_num

			// ota_file_size = UUP_PACKET_SIZE * ota_packet_num;			//文件总大小
            // printf("ota_packet_num %d.\n", ota_packet_num);
            printf("ota_file_size %d.\n", ota_file_size);
			if (ota_file_size > OTA_MAX_FILE_SIZE) {
				printf("Rev wrong file size.\n");
				ota_send_ack(frametype, UART_ACK_FAIL);
				break;
			}

			ota_send_ack(frametype, UART_ACK_OK);
			ota_status = OTA_STATE_GET_FILEINFO;
			break;

		//接收文件数据
		case UART_FRAME_FILEXFER:
			if (ota_status != OTA_STATE_GET_FILEINFO && ota_status != OTA_STATE_FILE_TFR) {
				ota_send_ack(frametype, UART_ACK_FAIL);
				break;
			}

			packetnum = buf[0];		//数据包序数
			// printf("ota_rev_packet %d.\n", ota_rev_packet);
			if ((ota_rev_packet & 0xff) != packetnum) {
				printf("Wrong packet number.\n");
				ota_send_ack(frametype, UART_ACK_FAIL);
				break;
			}


			framelen = len - 2;		//帧长度	数据帧：帧类型+包序数+数据包
			/* only last frame size is less than UUP_PACKET_SIZE */
			// if (framelen > UUP_PACKET_SIZE ||
			// 	(framelen < UUP_PACKET_SIZE && ota_rev_packet != ota_packet_num - 1)) {
			// 	printf("Wrong packet len.\n");
			// 	ota_send_ack(frametype, UART_ACK_FAIL);
			// 	break;
			// }

			if (ota_rev_len + framelen > OTA_BUFFER_LEN) {
				printf("ota_buffer is overflow.\n");
				ota_send_ack(frametype, UART_ACK_FAIL);
				break;
			}

			// rt_kprintf("ota_rev_len %d\n", ota_rev_len);
			memcpy(ota_buffer + ota_rev_len, buf + 1, framelen);	//将buf中有效文件数据拷贝到 ota_buffer缓存区
			ota_rev_len += framelen;
            ota_rev_size += framelen;
            ota_rev_packet++;

            // rt_kprintf("framelen %d\n", framelen);
            // rt_kprintf("ota_rev_size %d\n", ota_rev_size);

            print_progress(ota_rev_size, ota_file_size);
            if(ota_rev_len >= 2048)	
			{
				//写入flash
				ota_shard_download_handle((char *)ota_buffer, 2048);

				ota_rev_len -= 2048;
				memcpy(ota_buffer, ota_buffer + 2048, ota_rev_len);
			}

			ota_send_ack(frametype, UART_ACK_OK);
            // rt_kprintf("UART_ACK_OK\n");
			ota_status = OTA_STATE_FILE_TFR;
			break;

		case UART_FRAME_FINISH:
            rt_kprintf("ota_rev_size %d\n", ota_rev_size);
            rt_kprintf("ota_file_size %d\n", ota_file_size);
			if (ota_status != OTA_STATE_FILE_TFR && ota_status != UART_FRAME_FINISH) {
				ota_send_ack(frametype, UART_ACK_FAIL);
				break;
			}
			//0异常  1正常
			if (!buf[0]) {
				printf("update end with error!\n");
				ota_send_ack(frametype, UART_ACK_FAIL);
				ota_status = OTA_STATE_END;
				break;
			}
            
            if(ota_rev_size != ota_file_size)
            {
                rt_kprintf("ota_rev_size %d\n", ota_rev_size);
                rt_kprintf("ota_file_size %d\n", ota_file_size);
                rt_kprintf("Wrong packet num\n");
                ota_send_ack(frametype, UART_ACK_FAIL);
				ota_status = OTA_STATE_END;
                break;
            }
			if(ota_rev_len != 0)
			{
				//写入flash 剩余长度ota_rev_len
				ota_shard_download_handle((char *)ota_buffer, ota_rev_len);

				ota_rev_len = 0;
			}
	
			printf("update from ota ok.\n");
			ota_send_ack(frametype, UART_ACK_OK);

			ota_status = OTA_STATE_END;
			LOG_I("\033[0B");
			LOG_I("Download firmware to flash success.");
			LOG_I("System now will restart...");
			rt_thread_delay(rt_tick_from_millisecond(5));
	        int ret = RT_EOK;
			ret = aic_upgrade_end();
			if (ret) {
				LOG_E("Aic upgrade end");
			}

            clock_gettime(CLOCK_REALTIME, &end);
            printf("ota time:%0.1f\n",time_diff(&start, &end));

            rt_thread_mdelay(2000);
            extern void cmd_reboot(int argc, char **argv);
            cmd_reboot(0, NULL);        //重启
			break;
	}
}


//服务器：等待MCU请求升级
static void uart_rx_demo_thread(void *param)
{
	int ret = RT_EOK;
    char *tmp_buf = NULL, *tmp_buffer = NULL;
	ret = aic_upgrade_start();
    if (ret) {
        LOG_E("Aic get os to upgrade");
        return;
    }

    tmp_buf = aicos_malloc_align(0, USER_OTA_BURN_BUFF_LEN, CACHE_LINE_SIZE);
    if (!tmp_buf) {
        LOG_E("malloc tmp_buf failed\n");
        ret = -RT_ERROR;
        goto __exit;
    }

    tmp_buffer = rt_malloc(USER_OTA_HEAD_LEN);
    if (!tmp_buffer) {
        LOG_E("malloc tmp_buffer failed\n");
        ret = -RT_ERROR;
        goto __exit;
    }
	ota_buf_init(&bhdr, tmp_buf, USER_OTA_BURN_BUFF_LEN);
    ota_buf_init(&shdr, tmp_buffer, USER_OTA_HEAD_LEN);

    ota_buffer = rt_malloc(OTA_BUFFER_LEN);
    memset(ota_buffer, 0, OTA_BUFFER_LEN);

	//打开并配置串口
	uint8_t uartrx[UUP_MAX_FRAME_LEN] = {0};
	volatile int ret_r = 0;
	int len = 0;
	int str_index = 0;

	int i = 0;
    unsigned int pack_len = 0;
    rt_int32_t wait_tick = rt_tick_from_millisecond(1000);
    uint8_t time_out = 0;
	//接收串口数据
	for (;;) {
        memset(uartrx, 0, sizeof(uartrx));
		ret_r = rt_device_read(serial, -1, uartrx, 1);

        if(ret_r > 0)
        {
            if(uartrx[0] == 0x55)
            {
                static bool flag = false;
                if(flag == false){
                    flag = true;
                    clock_gettime(CLOCK_REALTIME, &start);
                }

                uart_rx_ptr = &uart_rx_buf[uart_rx_head][0];
                len = 0;
                str_index = 1;
                while(len < 2)
                {
                    ret_r = rt_device_read(serial, -1, uartrx+str_index, 2-len);
                    if(ret_r > 0)
                    {
                        len += ret_r;
                        str_index+=ret_r;
                    }
                }
                str_index = 3;
                if(uartrx[1] == 0x81 && uartrx[2] == 0xC6)
                {
                    len = 0;
                    while(len < 2)
                    {
                        ret_r = rt_device_read(serial, -1, uartrx+str_index, 2-len);
                        if(ret_r > 0)
                        {
                            len += ret_r;
                            str_index+=ret_r;
                        }
                    }
                    pack_len = (uartrx[3] << 8) | uartrx[4];
                    str_index = 5;
                    len = 0;
                    while(len < pack_len+1)
                    {
                        ret_r = rt_device_read(serial, -1, uartrx+str_index, pack_len+1-len);
                        if(ret_r > 0)
                        {
                            len += ret_r;
                            str_index+=ret_r;
                        }
                    }

                    memcpy(uart_rx_ptr, uartrx+1, pack_len+5);
                    uart_rx_head = (uart_rx_head + 1) % OTA_RX_FRAME_NUM;
                }
            }else{
                printf("uartrx[0] error\n");
            }
        }

		//处理并解析串口数据
		if (uart_rx_tail != uart_rx_head) {
			unsigned char *buf;
			unsigned int checksum = 0;

			buf = &uart_rx_buf[uart_rx_tail][0];

			len = (buf[2] << 8) | buf[3];

			//BCC校验
			for (i = 0; i < len + 4; i++)
				checksum ^= buf[i];

			if (checksum == buf[len + 4]) {
				//解析串口数据
				_ota_update(buf + 4, len);	//buf + 3 指针指向数据域
                
			} else {
				printf("rev frame checksum err.\n");
			}
			uart_rx_tail = (uart_rx_tail + 1) % OTA_RX_FRAME_NUM;
            len = 0;
		}
        
		if(RT_EOK != rt_sem_take(&rx_sem, wait_tick))// 串口接收超时处理
        {
            if(++time_out >= 10)
            {
                rt_device_close(serial);
                rt_sem_detach(&rx_sem);
                goto __exit;
            }
        }
	}
__exit:

    file_offset = 0;

    if (tmp_buf)
        aicos_free_align(0, tmp_buf);

    if (tmp_buffer)
        rt_free(tmp_buffer);

    return;
}

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem);

	return RT_EOK;
}


int uart_rx_demo(char *zx_uart)
{
	rt_err_t ret = RT_EOK;

    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

	serial = rt_device_find(zx_uart);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", zx_uart);
        ret = RT_ERROR;
        goto exit;
    }

    //串口接收线程
	rt_thread_t thread = rt_thread_create("serial", uart_rx_demo_thread, RT_NULL, 1024*8, 10, 10);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
        goto exit;
    }
    
    ret = rt_device_open(serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	if (ret != RT_EOK)
    {
        rt_kprintf("open %s failed : %d !\n", zx_uart, ret);
        ret = RT_ERROR;
        goto exit;
    }
    rt_kprintf("open %s finish\n", zx_uart);
    rt_device_set_rx_indicate(serial, uart_input);

exit:
    rt_sem_detach(&rx_sem);
	return 0;
}

void zx_uart_ota(uint8_t argc, char **argv)
{
    if (argc < 2) {
        rt_kprintf("using: zx_uart_ota uart7\n");
    } else {
        uart_rx_demo(argv[1]);
    }
}
MSH_CMD_EXPORT(zx_uart_ota, Use Uart to download the firmware);
