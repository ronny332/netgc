#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <network.h>
#include <ogcsys.h>
#include <gccore.h>
#include <sys/time.h>

/* FTPD */
BOOL FTPD_create_socket();
void FTPD_init();
BOOL FTPD_initialize_network();
void FTPD_log(char *str);
BOOL FTPD_read_commmand(int sock);
void FTPD_set_started();

extern int FTPD_started;