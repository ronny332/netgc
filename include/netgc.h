#ifndef __NET_H
#define __NET_H

#define NET_NONETWORK -1
#define NET_HOSTISIPADDRESS -2
#define NET_INVALIDHOST -3
#define NET_SOCKERROR -4
#define NET_CONNECTERROR -5
#define NET_SENDINGFAILED -6
#define NET_INVALIDRESPONSE -7
#define NET_INVALIDTRANSITIONID -8
#define NET_RESPONSEHASERRRORS -9
#define NET_INVALIDIPADDRESS -10
#define NET_SUCCESS 1

int get_ipbyhost(const char *host, char *ip);
int get_tsfromntp(const char *host);

u16 _get_transitionid(void);
BOOL _is_ipaddress(const char *ip);
BOOL _is_validhost(const char *host);

#endif