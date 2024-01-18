/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBH_RNDIS_H
#define USBH_RNDIS_H

#include "usb_cdc.h"

#include "lwip/netif.h"
#include "lwip/pbuf.h"

struct usbh_rndis {
    struct usbh_hubport *hport;

    uint8_t ctrl_intf; /* Control interface number */
    uint8_t data_intf; /* Data interface number */
    uint8_t minor;
    usbh_pipe_t bulkin;          /* Bulk IN endpoint */
    usbh_pipe_t bulkout;         /* Bulk OUT endpoint */
    usbh_pipe_t intin;           /* Notify endpoint */
    struct usbh_urb bulkin_urb;  /* Bulk IN urb */
    struct usbh_urb bulkout_urb; /* Bulk OUT urb */
    uint32_t request_id;

    uint32_t link_speed;
    bool link_status;
    uint8_t mac[6];

    uint16_t bulkin_wMaxPacketSize;
    uint16_t bulkout_wMaxPacketSize;
};

#ifdef __cplusplus
extern "C" {
#endif

int usbh_rndis_bulk_out_transfer(struct usbh_rndis *rndis_class, uint8_t *buffer, uint32_t buflen, uint32_t timeout);
int usbh_rndis_bulk_in_transfer(struct usbh_rndis *rndis_class, uint8_t *buffer, uint32_t buflen, uint32_t timeout);

int usbh_rndis_keepalive(struct usbh_rndis *rndis_class);

void usbh_rndis_run(struct usbh_rndis *rndis_class);
void usbh_rndis_stop(struct usbh_rndis *rndis_class);

err_t usbh_rndis_linkoutput(struct netif *netif, struct pbuf *p);
void usbh_rndis_rx_thread(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* USBH_RNDIS_H */
