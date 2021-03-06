#ifndef __NETGC_H
#define __NETGC_H

#define NETGC_NONETWORK -1
#define NETGC_HOSTISIPADDRESS -2
#define NETGC_INVALIDHOST -3
#define NETGC_SOCKERROR -4
#define NETGC_CONNECTERROR -5
#define NETGC_SENDINGFAILED -6
#define NETGC_INVALIDRESPONSE -7
#define NETGC_INVALIDTRANSITIONID -8
#define NETGC_RESPONSEHASERRRORS -9
#define NETGC_INVALIDIPADDRESS -10
#define NETCG_FALSE 0
#define NETGC_SUCCESS 1

int get_ipbyhost(const char *host, char *ip);
s32 set_gctimefromntp(const char *hosti, s16 gmt);

u16 _get_transitionid(void);
BOOL _is_ipaddress(const char *ip);
BOOL _is_validhost(const char *host);
s32 _time_gmttosec(s16 gmt);
u32 _time_ntptounix(u32 ntp);
u32 _time_unixtogc(u32 ts);

#endif