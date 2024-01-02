#include <stdio.h>
#include <utility.h>

#ifdef AIC_UTILITY_PRINT_BUF

void print_buf_U8(u8 *buf, u32 byteLen, char *name)
{
    u32 i;

    printf("\r\n %s: %08x\r\n  ", name, (u32)buf); //fflush(stdout);
    for (i = 0; i < byteLen; i++) {
        //if(i%16 ==0 && i>0)
        //    printf("\r\n");
        //printf("%02x", buf[byteLen-1-i]);
        printf("%02x", buf[i]);
    }

    printf("\r\n");
}

void print_buf_U32(u32 *buf, u32 wordLen, char *name)
{
    u32 i;

    printf("\r\n %s: %08x\r\n", name, (u32)buf); //fflush(stdout);
    for (i = 0; i < wordLen; i++) {
        //if(i%16 ==0 && i>0)
        //    (void)printf("\r\n");
        //printf("%08x", buf[wordLen-1-i]);
        printf("%08x", buf[i]); //fflush(stdout);
    }

    printf("\r\n"); //fflush(stdout);
}

void print_BN_buf_U32(u32 *buf, u32 wordLen, char *name)
{
    u32 i;

    printf("\r\n %08x %s: ", (u32)buf, name); //fflush(stdout);
    for (i = 0; i < wordLen; i++) {
        //if(i%16 ==0 && i>0)
        //    printf("\r\n");
        printf("%08x", buf[wordLen - 1U - i]);
    }
    printf("\r\n"); //fflush(stdout);
}
#endif
