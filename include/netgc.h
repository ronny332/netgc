#ifndef __NET_H
#define __NET_H

#define NET_NONETWORK -1
#define NET_INVALIDHOST -2
#define NET_SOCKERROR -3
#define NET_CONNECTERROR -4
#define NET_SENDINGFAILED -5
#define NET_INVALIDRESPONSE -6
#define NET_INVALIDTRANSITIONID -7
#define NET_RESPONSEHASERRRORS -8
#define NET_SUCCESS 1

int getipbyhost(const char *host, char *ip);

#endif