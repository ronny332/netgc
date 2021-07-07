#include "bba.h"
#include "ftpd.h"

char FTPD_cmd[256] = {0};
struct
{
	char localip[16];
	char netmask[16];
	char gateway[16];
} FTPD_network = {"", "", ""};
struct
{
	int port;
	int sock;
} FTPD_server = {21, 0};
BOOL FTPD_standalone = TRUE;
int FTPD_started = 0;

BOOL FTPD_create_socket()
{
	int *sock = &FTPD_server.sock;
	*sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (*sock == INVALID_SOCKET)
	{
		return FALSE;
	}

	// printf("\nsocket: %i %i\nstarted: %i\n\n", *sock, INVALID_SOCKET, FTPD_started);

	return FALSE;
}

void FTPD_init()
{
	FTPD_set_started();

	FTPD_log("\n\n\nLogging to screen");

	if (FTPD_initialize_network())
	{
		char log[254];
		//sprintf(log, "IP:\t\t\t%s\nNetmask:\t%s\nGateway:\t%s\n\n", FTPD_network.localip, FTPD_network.netmask, FTPD_network.gateway);
		sprintf(log, "IP:\t%s\n\n", bba_ip);
		FTPD_log(log);

		FTPD_create_socket();
	}
	else
	{
		FTPD_log("network initialization failed.");
	}
}

BOOL FTPD_initialize_network()
{
	FTPD_log("initializing network...");
	init_network();

	return (net_initialized == 1) ? TRUE : FALSE;

	// FTPD_log("initializing network...");
	// int ret = if_config(FTPD_network.localip, FTPD_network.netmask, FTPD_network.gateway, TRUE, 20);
	// if (ret != 0)
	// {
	// 	return FALSE;
	// }

	// return TRUE;
}

void FTPD_log(char *str)
{
	if (FTPD_standalone == TRUE)
	{
		printf("\n%s\n", str);
	}
}

BOOL FTPD_read_command(int sock)
{

	return FALSE;
}

void FTPD_set_started()
{
	if (FTPD_started == 0)
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		FTPD_started = tv.tv_sec;
	}
}