#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcbool.h>
#include <network.h>

#include "netgc.h"
#include "bba.h"
#include "map_lib.h"

const char *dns_ip = "1.1.1.1";
const u32 epoch_offset = 2208988800ull; // seconds since 1900
const BOOL netgc_debug = FALSE;

struct map_t *dns_cache = NULL;

/* one solution for the missing hostname to ip address implementation of libogc */
// TODO remove debug messages
int get_ipbyhost(const char *host, char *ip)
{
    /* no functionality without network */
    if (net_initialized == FALSE)
    {
        if (netgc_debug)
            printf("network not initialized\n");
        return NET_NONETWORK;
    }

    if (netgc_debug)
        printf("network initialized: %d\n", net_initialized);
    if (netgc_debug)
        printf("\n#### > debug start\n");

    /* pre checks */
    if (_is_ipaddress(host) == TRUE)
    {
        if (netgc_debug)
            printf("host is ip address\n");
        return NET_HOSTISIPADDRESS;
    }
    if (_is_validhost(host) == FALSE)
    {
        if (netgc_debug)
            printf("host invalid\n");
        return NET_INVALIDHOST;
    }

    /* initialize dns_cache, if not already done */
    if (dns_cache == NULL)
    {
        dns_cache = map_create();
    }

    /* check cache for values */
    char *ip_cached = map_get(dns_cache, host);

    if (strlen(ip_cached) > 0)
    {
        memset(ip, 0, 16);
        strncpy(ip, ip_cached, 16);

        if (netgc_debug)
            printf("found cached ip %s for host %s\n", ip, host);

        return TRUE;
    }
    {
        if (netgc_debug)
            printf("no cached ip for host %s found\n", host);
    }

    /* create dns request */
    // dns request has 18 extra bytes + host name length + 16 bytes for the response
    int req_len = 18 + strlen(host);
    int buffer_len = 1024;
    char buffer[buffer_len];
    memset(buffer, 0, buffer_len);
    int req_id = _get_transitionid();

    if (netgc_debug)
        printf("request transition id: %x\n", req_id);

    // (bytes 0+1) transition id
    u16 *u16_req = (u16 *)buffer;
    *u16_req = (u16)htonl(req_id);
    // (bytes 2+3) flags
    u16_req += 1;
    *u16_req = (u16)htonl(0x0100);
    // (bytes 4+5) questions
    u16_req += 1;
    *u16_req = (u16)htonl(0x0001);
    // (bytes 6+7) answer rrs
    u16_req += 1;
    *u16_req = (u16)htonl(0x0000);
    // (bytes 8+9) authority rrs
    u16_req += 1;
    *u16_req = (u16)htonl(0x0000);
    // (bytes 10+11) additional rrs
    u16_req += 1;
    *u16_req = (u16)htonl(0x0000);
    // (byte 12) "length of first domain part"
    u8 *u8_req = (u8 *)(buffer + 12);
    int dot_value = 0;
    for (int i = 0; i < strlen(host); i++)
    {
        if (host[i] == '.')
        {
            break;
        }
        dot_value += 1;
    }
    *u8_req = htons(dot_value);

    // add domain to request
    for (int i = 0; i < strlen(host); i++)
    {
        u8_req = (u8 *)(buffer + 13 + i);
        if (host[i] == '.')
        {
            dot_value = 0;
            int j = i;
            while (j < strlen(host) - 1)
            {
                j += 1;
                if (host[j] != '.')
                {
                    dot_value += 1;
                }
                else
                {
                    break;
                }
            }
            *u8_req = dot_value;
        }
        else
        {
            *u8_req = host[i];
        }
    }

    // (byte -5) "zero"
    u8_req = (u8 *)(buffer + req_len - 5);
    *u8_req = htons(0x00);
    // (bytes -4+-3) type
    u16_req = (u16 *)(buffer + req_len - 4);
    *u16_req = (u16)htonl(0x0001);
    // (bytes -2+-1) class
    u16_req = (u16 *)(buffer + req_len - 2);
    *u16_req = (u16)htonl(0x0001);

    for (int i = 0; i < req_len; i++)
    {
        if (netgc_debug)
            printf("%02x ", buffer[i]);
    }

    if (netgc_debug)
        printf("\n");

    /* perform request */
    s32 sock = -1;

    sock = net_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0)
    {
        if (netgc_debug)
            printf("can't create socket\n");
        return NET_SOCKERROR;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(dns_ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(53);

    if (net_connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        if (netgc_debug)
            printf("connect error\n");
        return NET_CONNECTERROR;
    }

    if (netgc_debug)
        printf("connected to %s\n", dns_ip);

    if (net_send(sock, buffer, req_len, 0) < 0)
    {
        if (netgc_debug)
            printf("sending failed");
        return NET_SENDINGFAILED;
    }

    if (netgc_debug)
        printf("request sent\n");

    memset(buffer, 0, buffer_len);

    /* read response */
    s32 res_len = net_recv(sock, buffer, buffer_len, 0);

    if (res_len < 0)
    {
        if (netgc_debug)
            printf("invalid response\n");
        return NET_INVALIDRESPONSE;
    }

    if (netgc_debug)
        printf("\nresult:\n");

    for (int i = 0; i < res_len; i++)
    {
        if (buffer[i] != '\0')
        {
            if (netgc_debug)
                printf("%02x ", buffer[i]);
        }
    }

    if (netgc_debug)
        printf("\n");

    /* handle response */
    // (bytes 0+1) transition id
    u16 *s16_res = (u16 *)buffer;
    u16 res_id = ntohl(*s16_res);

    if (netgc_debug)
        printf("response transition id: %x\n", res_id);

    if (req_id != res_id)
    {
        if (netgc_debug)
            printf("invalid transition id in response\n");
        return NET_INVALIDTRANSITIONID;
    }

    s16_res += 1;
    u16 res_flags = ntohl(*s16_res);

    if (((res_flags & 0xf) | 0x0) != 0)
    {
        if (netgc_debug)
            printf("response has errors\n");
        return NET_RESPONSEHASERRRORS;
    }

    // last 4 bytes of response is the ip address
    u8 *u8_res = (u8 *)(buffer + res_len - 4);
    u32 res_ip = ntohl(*((u32 *)u8_res));

    memset(ip, 0, 16);
    sprintf(ip, "%d.%d.%d.%d", 0xff & (res_ip >> 24), 0xff & (res_ip >> 16), 0xff & (res_ip >> 8), 0xff & res_ip);

    map_set(dns_cache, host, ip);
    if (netgc_debug)
        printf("storing ip %s for host %s at dns_cache\n", ip, host);

    if (netgc_debug)
        printf("closing socket %d\n", sock);

    net_close(sock);

    if (netgc_debug)
        printf("#### /> debug end\n\n");

    return TRUE;
}

/* read unix timestamp from NTP server */
int get_tsfromntp(const char *host)
{
    int ret = 0;

    if (net_initialized == FALSE)
        return NET_NONETWORK;
    if (_is_validhost(host) == FALSE)
        return NET_INVALIDHOST;
    if (_is_ipaddress(host) == TRUE)
        return NET_HOSTISIPADDRESS;

    char ntp_server_ip[16];
    if ((ret = get_ipbyhost(host, &(ntp_server_ip[0]))) != NET_SUCCESS)
        return ret;

    s32 sock = -1;
    sock = net_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0)
        return NET_SOCKERROR;

    struct sockaddr_in ntp_server;
    memset(&ntp_server, 0, sizeof(ntp_server));
    ntp_server.sin_addr.s_addr = inet_addr(ntp_server_ip);
    ntp_server.sin_family = AF_INET;
    ntp_server.sin_port = htons(123);

    if (net_connect(sock, (struct sockaddr *)&ntp_server, sizeof(ntp_server)) < 0)
        return NET_CONNECTERROR;

    // the request buffer gets used for the request and response (both 48 bytes)
    int len_dns_request = 48;
    char dns_request[len_dns_request];

    memset(dns_request, 0, len_dns_request);
    // byte 0 with v3 dns request header
    dns_request[0] = 0b00011011;

    if (net_send(sock, dns_request, len_dns_request, 0) < 0)
        return NET_SENDINGFAILED;

    if (net_recv(sock, dns_request, len_dns_request - 1, 0) < 0)
        return NET_INVALIDRESPONSE;

    // the NTP response contains 2 identical timestamps
    u32 ts1 = ntohl(*((u32 *)(dns_request + 32))) - epoch_offset;
    u32 ts2 = ntohl(*((u32 *)(dns_request + 40))) - epoch_offset;

    if (ts1 == 0 || ts2 == 0 || ts1 != ts2)
        return NET_RESPONSEHASERRRORS;

    net_close(sock);

    return ts1;
}

/* helper function to create transaction ids, used in dns requests */
u16 _get_transitionid(void)
{
    srand(time(NULL));
    return (u16)(rand() % 5000 + 5000);
}

/* caused by missing regexp features, this should work as a simple IP address validator */
BOOL _is_ipaddress(const char *ip)
{
    int len = (ip != NULL) ? strlen(ip) : 0;
    if (len && len >= 7 && len <= 15 && ip[0] != '.' && ip[len - 1] != '.')
    {
        int occurrences = 0;
        for (int i = 0; i < strlen(ip); i++)
        {
            if (ip[i] == '.')
            {
                occurrences += 1;
            }
        }
        return (occurrences == 3) ? TRUE : FALSE;
    }

    return FALSE;
}

BOOL _is_validhost(const char *host)
{
    int len = (host != NULL) ? strlen(host) : 0;
    return (len < 3 || len > 255 || strstr(host, ".") == NULL || host[0] == '.' || host[len - 1] == '.') ? FALSE : TRUE;
}