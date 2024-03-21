/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <aic_core.h>
#include <aic_drv.h>
#include "aic_hal_uart.h"
#include "aic_drv_uart.h"

#ifndef AIC_CLK_UART0_FREQ
#define AIC_CLK_UART0_FREQ 48000000 /* default 48M*/
#endif
#ifndef AIC_CLK_UART1_FREQ
#define AIC_CLK_UART1_FREQ 48000000 /* default 48M*/
#endif
#ifndef AIC_CLK_UART2_FREQ
#define AIC_CLK_UART2_FREQ 48000000 /* default 48M*/
#endif
#ifndef AIC_CLK_UART3_FREQ
#define AIC_CLK_UART3_FREQ 48000000 /* default 48M*/
#endif
#ifndef AIC_CLK_UART4_FREQ
#define AIC_CLK_UART4_FREQ 48000000 /* default 48M*/
#endif
#ifndef AIC_CLK_UART5_FREQ
#define AIC_CLK_UART5_FREQ 48000000 /* default 48M*/
#endif
#ifndef AIC_CLK_UART6_FREQ
#define AIC_CLK_UART6_FREQ 48000000 /* default 48M*/
#endif
#ifndef AIC_CLK_UART7_FREQ
#define AIC_CLK_UART7_FREQ 48000000 /* default 48M*/
#endif

#ifndef AIC_UART0_PA_RS485_CTL_NAME
#define AIC_UART0_PA_RS485_CTL_NAME
#endif
#ifndef AIC_UART1_PA_RS485_CTL_NAME
#define AIC_UART1_PA_RS485_CTL_NAME
#endif
#ifndef AIC_UART2_PA_RS485_CTL_NAME
#define AIC_UART2_PA_RS485_CTL_NAME
#endif
#ifndef AIC_UART3_PA_RS485_CTL_NAME
#define AIC_UART3_PA_RS485_CTL_NAME
#endif
#ifndef AIC_UART4_PA_RS485_CTL_NAME
#define AIC_UART4_PA_RS485_CTL_NAME
#endif
#ifndef AIC_UART5_PA_RS485_CTL_NAME
#define AIC_UART5_PA_RS485_CTL_NAME
#endif
#ifndef AIC_UART6_PA_RS485_CTL_NAME
#define AIC_UART6_PA_RS485_CTL_NAME
#endif
#ifndef AIC_UART7_PA_RS485_CTL_NAME
#define AIC_UART7_PA_RS485_CTL_NAME
#endif

struct aic_uart_485_dev
{
    char *uart_rts_name;
    unsigned int uart_rts_pin;
    usart_handle_t handle;
};

struct aic_uart_485_dev uart_485_dev[AIC_UART_DEV_NUM];

void drv_usart_irqhandler(int irq, void * data);

struct aic_uart_dev
{
    uint16_t uart_dma_flag;
    uint8_t dma_enable_flag;
};

struct aic_uart_dev uart_dev[AIC_UART_MAX_NUM];

struct
{
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const drv_usart_config[AIC_UART_DEV_NUM] =
{
#if (AIC_UART_DEV_NUM >= 4)
    {UART0_BASE, UART0_IRQn, drv_usart_irqhandler},
    {UART1_BASE, UART1_IRQn, drv_usart_irqhandler},
    {UART2_BASE, UART2_IRQn, drv_usart_irqhandler},
    {UART3_BASE, UART3_IRQn, drv_usart_irqhandler},
#endif
#if (AIC_UART_DEV_NUM >= 8)
    {UART4_BASE, UART4_IRQn, drv_usart_irqhandler},
    {UART5_BASE, UART5_IRQn, drv_usart_irqhandler},
    {UART6_BASE, UART6_IRQn, drv_usart_irqhandler},
    {UART7_BASE, UART7_IRQn, drv_usart_irqhandler},
#endif
};

static  usart_handle_t uart_handle[AIC_UART_DEV_NUM];
static struct rt_serial_device  serial[AIC_UART_DEV_NUM];
static uint32_t uart_rx_fifo[2048] __attribute__((aligned(64)));
static uint32_t uart_tx_fifo[2048] __attribute__((aligned(64)));
static uint32_t rx_size = 0;
static rt_base_t level;
static rt_base_t tx_level;

int32_t drv_usart_target_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= AIC_UART_DEV_NUM)
    {
        return -1;
    }

    if (base != NULL)
    {
        *base = drv_usart_config[idx].base;
    }

    if (irq != NULL)
    {
        *irq = drv_usart_config[idx].irq;
    }

    if (handler != NULL)
    {
        *handler = drv_usart_config[idx].handler;
    }

    return idx;
}

void drv_usart_irqhandler(int irq, void * data)
{
    int index = irq - UART0_IRQn;
    uint8_t status= 0;

    if (index >= AIC_UART_DEV_NUM)
        return;

    status = hal_usart_get_irqstatus(index);

    if (uart_dev[index].dma_enable_flag == 1) {
        usart_handle_t uart;
        level = rt_hw_interrupt_disable();

        RT_ASSERT(serial != RT_NULL);
        uart = (usart_handle_t)serial[index].parent.user_data;
        RT_ASSERT(uart != RT_NULL);
        hal_usart_set_ier(uart, 0);
        switch (status)
        {
        case AIC_IIR_RECV_DATA:
        case AIC_IIR_CHAR_TIMEOUT:
            rx_size = hal_usart_get_rx_fifo_num(uart);
            // rt_kprintf("%d,%d\n",rx_size,status);
            hal_uart_rx_dma_config(uart, (uint8_t *)uart_rx_fifo, rx_size);
            break;

        default:
            break;
        }
    } else {
        switch (status)
        {
        case AIC_IIR_RECV_DATA:
            rt_hw_serial_isr(&serial[index],RT_SERIAL_EVENT_RX_IND);
            break;

        default:
            break;
        }
    }
}

static void drv_uart_get_dma_config(void)
{
#ifdef AIC_DEV_UART0_MODE_DMA
    uart_dev[0].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |
                                    RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
    uart_dev[0].dma_enable_flag = 1;
#else
    uart_dev[0].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX;
    uart_dev[0].dma_enable_flag = 0;
#endif
#ifdef AIC_DEV_UART1_MODE_DMA
    uart_dev[1].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |
                                    RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
    uart_dev[1].dma_enable_flag = 1;
#else
    uart_dev[1].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX;
    uart_dev[1].dma_enable_flag = 0;
#endif
#ifdef AIC_DEV_UART2_MODE_DMA
    uart_dev[2].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |
                                    RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
    uart_dev[2].dma_enable_flag = 1;
#else
    uart_dev[2].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX;
    uart_dev[2].dma_enable_flag = 0;
#endif
#ifdef AIC_DEV_UART3_MODE_DMA
    uart_dev[3].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |
                                    RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
    uart_dev[3].dma_enable_flag = 1;
#else
    uart_dev[3].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX;
    uart_dev[3].dma_enable_flag = 0;
#endif
#ifdef AIC_DEV_UART4_MODE_DMA
    uart_dev[4].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |
                                    RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
    uart_dev[4].dma_enable_flag = 1;
#else
    uart_dev[4].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX;
    uart_dev[4].dma_enable_flag = 0;
#endif
#ifdef AIC_DEV_UART5_MODE_DMA
    uart_dev[5].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |
                                    RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
    uart_dev[5].dma_enable_flag = 1;
#else
    uart_dev[5].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX;
    uart_dev[5].dma_enable_flag = 0;
#endif
#ifdef AIC_DEV_UART6_MODE_DMA
    uart_dev[6].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |
                                    RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
    uart_dev[6].dma_enable_flag = 1;
#else
    uart_dev[6].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX;
    uart_dev[6].dma_enable_flag = 0;
#endif
#ifdef AIC_DEV_UART7_MODE_DMA
    uart_dev[7].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |
                                    RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
    uart_dev[7].dma_enable_flag = 1;
#else
    uart_dev[7].uart_dma_flag |= RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX;
    uart_dev[7].dma_enable_flag = 0;
#endif
}

/*
 * UART interface
 */
static rt_err_t drv_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    int ret;
    usart_handle_t uart;

    uint32_t bauds;
    usart_mode_e mode;
    usart_parity_e parity;
    usart_stop_bits_e stopbits;
    usart_data_bits_e databits;

    RT_ASSERT(serial != RT_NULL);
    uart = (usart_handle_t)serial->parent.user_data;
    RT_ASSERT(uart != RT_NULL);

    /* set baudrate parity...*/
    bauds = cfg->baud_rate;
    mode = USART_MODE_ASYNCHRONOUS;

    if (cfg->parity == PARITY_EVEN)
        parity = USART_PARITY_EVEN;
    else if (cfg->parity == PARITY_ODD)
        parity = USART_PARITY_ODD;
    else
        parity = USART_PARITY_NONE;

    if (cfg->stop_bits == 1)
        stopbits = USART_STOP_BITS_1;
    else if (cfg->stop_bits == 2)
        stopbits = USART_STOP_BITS_2;
    else if (cfg->stop_bits == 3)
        stopbits = USART_STOP_BITS_1_5;
    else
        stopbits = USART_STOP_BITS_0_5;

    if (cfg->data_bits == 5)
        databits = USART_DATA_BITS_5;
    else if (cfg->data_bits == 6)
        databits = USART_DATA_BITS_6;
    else if (cfg->data_bits == 7)
        databits = USART_DATA_BITS_7;
    else if (cfg->data_bits == 8)
        databits = USART_DATA_BITS_8;
    else if (cfg->data_bits == 9)
        databits = USART_DATA_BITS_9;
    else
        databits = USART_DATA_BITS_8;

    ret = hal_usart_config(uart, bauds, mode, parity, stopbits, databits, cfg->function);

    if (ret < 0)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t drv_uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    usart_handle_t uart;
    aic_usart_priv_t *uart_data;
    unsigned int group, pin;

    RT_ASSERT(serial != RT_NULL);
    uart = (usart_handle_t)serial->parent.user_data;
    uart_data = serial->parent.user_data;
    RT_ASSERT(uart != RT_NULL);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* Disable the UART Interrupt */
        if ((uintptr_t)arg == RT_DEVICE_FLAG_INT_RX)
            hal_usart_set_interrupt(uart, USART_INTR_READ, 0);
        break;

    case RT_DEVICE_CTRL_SET_INT:
        /* Enable the UART Interrupt */
        if ((uintptr_t)arg == RT_DEVICE_FLAG_INT_RX)
            hal_usart_set_interrupt(uart, USART_INTR_READ, 1);
        break;

    case AIC_UART_485_CTL_SOFT_MODE0:
        hal_usart_rts_ctl_soft_mode_clr(uart);
        group = GPIO_GROUP(uart_485_dev[uart_data->idx].uart_rts_pin);
        pin = GPIO_GROUP_PIN(uart_485_dev[uart_data->idx].uart_rts_pin);
        hal_gpio_clr_output(group, pin);
        break;

    case AIC_UART_485_CTL_SOFT_MODE1:
        hal_usart_rts_ctl_soft_mode_set(uart);
        group = GPIO_GROUP(uart_485_dev[uart_data->idx].uart_rts_pin);
        pin = GPIO_GROUP_PIN(uart_485_dev[uart_data->idx].uart_rts_pin);
        hal_gpio_set_output(group, pin);
        break;
    }

    return (RT_EOK);
}

static int drv_uart_putc(struct rt_serial_device *serial, char c)
{
    usart_handle_t uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (usart_handle_t)serial->parent.user_data;
    RT_ASSERT(uart != RT_NULL);
    hal_usart_putchar(uart,c);

    return (1);
}

static int drv_uart_getc(struct rt_serial_device *serial)
{
    int ch;
    usart_handle_t uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (usart_handle_t)serial->parent.user_data;
    RT_ASSERT(uart != RT_NULL);


    ch = hal_uart_getchar(uart);

    return ch;
}

#if defined (RT_SERIAL_USING_DMA)
#include <string.h>
static void drv_uart_callback(aic_usart_priv_t *uart, void *arg)
{
    unsigned long event = (unsigned long)arg;
    struct rt_serial_rx_fifo *rx_fifo;

    switch(event)
    {
    case AIC_UART_TX_INT:
        rt_hw_serial_isr(&serial[uart->idx], RT_SERIAL_EVENT_TX_DMADONE);
        break;

    case AIC_UART_RX_INT:
        rx_fifo = (struct rt_serial_rx_fifo *)serial[uart->idx].serial_rx;
        if (rx_fifo->put_index + rx_size < serial[uart->idx].config.bufsz) {
            memcpy((rx_fifo->buffer + rx_fifo->put_index), (rt_uint8_t *)uart_rx_fifo, rx_size);
        } else {
            memcpy((rx_fifo->buffer + rx_fifo->put_index), (rt_uint8_t *)uart_rx_fifo,
                    serial->config.bufsz - rx_fifo->put_index);
            memcpy((rx_fifo->buffer), ((rt_uint8_t *)uart_rx_fifo + serial->config.bufsz -
                    rx_fifo->put_index), rx_size + rx_fifo->put_index - serial->config.bufsz);
        }
        rt_hw_interrupt_enable(level);
        hal_usart_set_ier(uart, 1);
        rt_hw_serial_isr(&serial[uart->idx], RT_SERIAL_EVENT_RX_DMADONE | (rx_size << 8));
        break;

    default:
        hal_log_err("not support event\n");
        break;
    }
}

static rt_size_t drv_uart_dma_transmit(struct rt_serial_device *serial, rt_uint8_t *buf,
                                        rt_size_t size, int direction)
{
    usart_handle_t uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (usart_handle_t)serial->parent.user_data;

    if (direction == RT_SERIAL_DMA_TX) {
        tx_level = rt_hw_interrupt_disable();
        memcpy((rt_uint8_t *)uart_tx_fifo, buf, size);
        if (hal_uart_send_by_dma(uart, (rt_uint8_t *)uart_tx_fifo, size) ==  RT_EOK) {
            rt_hw_interrupt_enable(tx_level);
            return size;
        } else {
            rt_hw_interrupt_enable(tx_level);
            return 0;
        }
    } else {
        return 0;
    }
    return 0;
}
#endif

const struct rt_uart_ops drv_uart_ops =
{
    drv_uart_configure,
    drv_uart_control,
    drv_uart_putc,
    drv_uart_getc,
#if defined (RT_SERIAL_USING_DMA)
    drv_uart_dma_transmit,
#endif
};

struct drv_uart_dev_para
{
    uint32_t index                   :4;
    uint32_t data_bits               :4;
    uint32_t stop_bits               :2;
    uint32_t parity                  :2;
    uint32_t baud_rate;
    uint32_t clk_freq;
    uint32_t function;
    char * name;
    char * uart_rts_name;
};

const struct drv_uart_dev_para uart_dev_paras[] =
{
#ifdef AIC_USING_UART0
    {0, AIC_DEV_UART0_DATABITS, AIC_DEV_UART0_STOPBITS, AIC_DEV_UART0_PARITY, AIC_DEV_UART0_BAUDRATE, AIC_CLK_UART0_FREQ, AIC_DEV_UART0_MODE, "uart0", AIC_UART0_PA_RS485_CTL_NAME},
#endif
#ifdef AIC_USING_UART1
    {1, AIC_DEV_UART1_DATABITS, AIC_DEV_UART1_STOPBITS, AIC_DEV_UART1_PARITY, AIC_DEV_UART1_BAUDRATE, AIC_CLK_UART1_FREQ, AIC_DEV_UART1_MODE, "uart1", AIC_UART1_PA_RS485_CTL_NAME},
#endif
#ifdef AIC_USING_UART2
    {2, AIC_DEV_UART2_DATABITS, AIC_DEV_UART2_STOPBITS, AIC_DEV_UART2_PARITY, AIC_DEV_UART2_BAUDRATE, AIC_CLK_UART2_FREQ, AIC_DEV_UART2_MODE, "uart2", AIC_UART2_PA_RS485_CTL_NAME},
#endif
#ifdef AIC_USING_UART3
    {3, AIC_DEV_UART3_DATABITS, AIC_DEV_UART3_STOPBITS, AIC_DEV_UART3_PARITY, AIC_DEV_UART3_BAUDRATE, AIC_CLK_UART3_FREQ, AIC_DEV_UART3_MODE, "uart3", AIC_UART3_PA_RS485_CTL_NAME},
#endif
#ifdef AIC_USING_UART4
    {4, AIC_DEV_UART4_DATABITS, AIC_DEV_UART4_STOPBITS, AIC_DEV_UART4_PARITY, AIC_DEV_UART4_BAUDRATE, AIC_CLK_UART4_FREQ, AIC_DEV_UART4_MODE, "uart4", AIC_UART4_PA_RS485_CTL_NAME},
#endif
#ifdef AIC_USING_UART5
    {5, AIC_DEV_UART5_DATABITS, AIC_DEV_UART5_STOPBITS, AIC_DEV_UART5_PARITY, AIC_DEV_UART5_BAUDRATE, AIC_CLK_UART5_FREQ, AIC_DEV_UART5_MODE, "uart5", AIC_UART5_PA_RS485_CTL_NAME},
#endif
#ifdef AIC_USING_UART6
    {6, AIC_DEV_UART6_DATABITS, AIC_DEV_UART6_STOPBITS, AIC_DEV_UART6_PARITY, AIC_DEV_UART6_BAUDRATE, AIC_CLK_UART6_FREQ, AIC_DEV_UART6_MODE, "uart6", AIC_UART6_PA_RS485_CTL_NAME},
#endif
#ifdef AIC_USING_UART7
    {7, AIC_DEV_UART7_DATABITS, AIC_DEV_UART7_STOPBITS, AIC_DEV_UART7_PARITY, AIC_DEV_UART7_BAUDRATE, AIC_CLK_UART7_FREQ, AIC_DEV_UART7_MODE, "uart7", AIC_UART7_PA_RS485_CTL_NAME},
#endif
};

int drv_usart_init(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    int u = 0;
    int i = 0;

#ifdef RT_SERIAL_USING_DMA
    drv_uart_get_dma_config();
#endif

    for (i=0; i<sizeof(uart_dev_paras)/sizeof(struct drv_uart_dev_para); i++) {
        u = uart_dev_paras[i].index;
        serial[u].ops                 = & drv_uart_ops;
        serial[u].config              = config;
        serial[u].config.bufsz        = 2048;
        serial[u].config.baud_rate    = uart_dev_paras[i].baud_rate;
        serial[u].config.data_bits    = uart_dev_paras[i].data_bits;
        serial[u].config.stop_bits    = uart_dev_paras[i].stop_bits;
        serial[u].config.parity       = uart_dev_paras[i].parity;
        serial[u].config.function     = uart_dev_paras[i].function;

        hal_clk_set_freq(CLK_UART0 + u, uart_dev_paras[i].clk_freq);
        hal_clk_enable(CLK_UART0 + u);
        hal_reset_assert(RESET_UART0 + u);
        aic_udelay(10000);
        hal_reset_deassert(RESET_UART0 + u);

#ifdef FINSH_POLL_MODE
        uart_handle[u] = hal_usart_initialize(u, NULL, NULL);
#else
        uart_handle[u] = hal_usart_initialize(u, NULL, drv_usart_irqhandler);
#endif
        if (uart_dev_paras[i].function == AIC_UART_DEV_MODE_RS485) {
            unsigned int group, pin;
            uart_485_dev[u].uart_rts_name = uart_dev_paras[i].uart_rts_name;
            uart_485_dev[u].uart_rts_pin = hal_gpio_name2pin(uart_485_dev[u].uart_rts_name);
            group = GPIO_GROUP(uart_485_dev[u].uart_rts_pin);
            pin = GPIO_GROUP_PIN(uart_485_dev[u].uart_rts_pin);
            hal_gpio_direction_output(group, pin);
            hal_gpio_clr_output(group, pin);
        }
        rt_hw_serial_register(&serial[u],
                              uart_dev_paras[i].name,
#ifdef FINSH_POLL_MODE
                              RT_DEVICE_FLAG_RDWR,
#endif
#ifdef RT_SERIAL_USING_DMA
                              uart_dev[u].uart_dma_flag,
#else
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
#endif
                              uart_handle[u]);

        if (uart_dev[u].dma_enable_flag == 1) {
            hal_uart_set_fifo((aic_usart_priv_t *)serial[u].parent.user_data);
            hal_uart_attach_callback((aic_usart_priv_t *)serial[u].parent.user_data,
                                        drv_uart_callback, NULL);
            hal_usart_set_ier((aic_usart_priv_t *)serial[u].parent.user_data, 1);
        }
    }

    return 0;
}
INIT_BOARD_EXPORT(drv_usart_init);
