#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>

#include "bba.h"
#include "netgc.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void *Initialise();

int main(int argc, char **argv)
{

	xfb = Initialise();

	// make the output visible outside of overscan of CRT
	printf("\n\n\n");

	init_network();

	if (net_initialized)
	{
		// char *hosts[] = {"www.test.com", "www.facebook.com", "www.twitter.com", "www.amazon.com", "www.amazon.co.uk", "www.ebay.com", "www.test.com"};
		// char ip[16] = {0};

		// for (int i = 0; i < sizeof(hosts) / sizeof(char *); i++)
		// {
		// 	if (get_ipbyhost(hosts[i], &(ip[0])) == NET_SUCCESS)
		// 	{
		// 		printf("host:\t%s\nip:\t%s\n\n", hosts[i], ip);
		// 	}
		// }

		char *host = "0.us.pool.ntp.org";
		
		printf("ts: %d\n", get_tsfromntp(host));
	}
	else
	{
		printf("no network\n");
	}

	while (1)
	{

		VIDEO_WaitVSync();
		PAD_ScanPads();

		int buttonsDown = PAD_ButtonsDown(0);

		if (buttonsDown & PAD_BUTTON_A)
		{
			printf("Button A pressed.\n");
		}

		if (buttonsDown & PAD_BUTTON_START)
		{
			exit(0);
		}
	}

	return 0;
}

void *Initialise()
{

	void *framebuffer;

	VIDEO_Init();
	PAD_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();

	return framebuffer;
}
