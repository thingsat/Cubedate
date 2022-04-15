/*
 * Copyright (C) 2015 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Showing minimum memory footprint of gnrc network stack
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "net/ipv6/addr.h"
#include "net/nanocoap_sock.h"
#include "csp/csp_id.h"

#define COAP_INBUF_SIZE (256U)

/* Extend stacksize of nanocoap server thread */
#define NANOCOAP_SERVER_QUEUE_SIZE     (2)
static msg_t _nanocoap_server_msg_queue[NANOCOAP_SERVER_QUEUE_SIZE];


/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);

int main(void)
{
    puts("CSP minimal CoAP server application");

    csp_iface_t *i = csp_iflist_get();

    while (i) {
        ipv6_addr_t addr;
        memset(&addr, '\0', sizeof(ipv6_addr_t));
        addr.u8[sizeof(ipv6_addr_t) - 1] = (uint8_t)i->addr;
        addr.u8[sizeof(ipv6_addr_t) - 2] = (uint8_t)(i->addr >> 8);

        char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];
        ipv6_addr_to_str(ipv6_addr, &addr, IPV6_ADDR_MAX_STR_LEN);
        printf("My address is %s\n", ipv6_addr);
        i = i->next;
    }
    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_nanocoap_server_msg_queue, NANOCOAP_SERVER_QUEUE_SIZE);

    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port = COAP_PORT, .family = AF_INET6 };

    nanocoap_server(&local, buf, sizeof(buf));

    return 0;
}
