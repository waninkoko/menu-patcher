#include <stdio.h>
#include <ogcsys.h>

#include "sysmenu.h"
#include "sys.h"
#include "video.h"
#include "wpad.h"

/* Constants */
#define CONSOLE_XCOORD		20
#define CONSOLE_YCOORD		100

#define REGION_MIN		0
#define REGION_MAX		2


void Restart(void)
{
	printf("\n    Restarting Wii...");
	fflush(stdout);

	/* Load system menu */
	Sys_LoadMenu();
}

void Restart_Wait(void)
{
	printf("\n    Press any button to restart...");
	fflush(stdout);

	/* Wait for button */
	Wpad_WaitButtons();

	printf(" Restarting Wii...");
	fflush(stdout);

	/* Load system menu */
	Sys_LoadMenu();
}

void ShowBanner(void)
{
	extern char banner_data[];

	PNGUPROP imgProp;
	IMGCTX ctx;

	s32 ret;

	/* Select PNG data */
	ctx = PNGU_SelectImageFromBuffer(banner_data);
	if (!ctx)
		return;

	/* Get image properties */
	ret = PNGU_GetImageProperties(ctx, &imgProp);
	if (ret != PNGU_OK)
		return;

	/* Draw image */
	Video_DrawPng(ctx, imgProp, 0, 0);

	/* Free image context */
	PNGU_ReleaseImageContext(ctx);
}

int main(int argc, char **argv)
{
	s32 ret;

	/* Load Custom IOS */
	ret = IOS_ReloadIOS(249);

	/* Initialize subsystems */
	Sys_Init();

	/* Set video mode */
	Video_SetMode();

	/* Show banner */
	ShowBanner();

	/* Initialize console */
	Con_Init(CONSOLE_XCOORD, CONSOLE_YCOORD);

	/* Initialize Wiimote */
	Wpad_Init();

	/* Check if Custom IOS is loaded */
	if (ret < 0) {
		printf("[+] Custom IOS could not be loaded! (ret = %d)\n", ret);
		goto out;
	}

	/* Launch System Menu */
	Sysmenu_Launch();

out:
	/* Restart console */
	Restart_Wait();

	return 0;
}
