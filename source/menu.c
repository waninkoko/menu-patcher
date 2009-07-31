#include <stdio.h>
#include <ogcsys.h>

#include "device.h"
#include "restart.h"
#include "video.h"
#include "wpad.h"

/* FAT device list */
static fatDevice devList[] = {
	{ "Disable",				0,	0x00,	0x00 },
	{ "SD/SDHC Card",			1,	0xF0,	0xF1 },
	{ "USB 2.0 Mass Storage Device",	2,	0xF2,	0xF3 },
};

/* Macros */
#define NB_DEVICES		(sizeof(devList) / sizeof(fatDevice))


void Menu_ContentDevice(void)
{
	fatDevice *dev = NULL;

	s32 selected = 0;
	s32 ret;

	/* Select source device */
	for (;;) {
		/* Clear console */
		Con_Clear();

		/* Selected device */
		dev = &devList[selected];

		printf("\t>> Select content override device: < %s >\n\n", dev->name);

		printf("\t   Press LEFT/RIGHT to change the selected device.\n\n");

		printf("\t   Press A button to continue.\n");
		printf("\t   Press HOME button to restart.\n\n");

		u32 buttons = Wpad_WaitButtons();

		/* LEFT/RIGHT buttons */
		if (buttons & WPAD_BUTTON_LEFT) {
			if ((--selected) <= -1)
				selected = (NB_DEVICES - 1);
		}
		if (buttons & WPAD_BUTTON_RIGHT) {
			if ((++selected) >= NB_DEVICES)
				selected = 0;
		}

		/* HOME button */
		if (buttons & WPAD_BUTTON_HOME)
			Restart();

		/* A button */
		if (buttons & WPAD_BUTTON_A)
			break;
	}

	/* Device selected */
	if (dev->mode) {
		printf("[+] Setting content override...");
		fflush(stdout);

		/* Mount device */
		ret = Device_Mount(dev);
		if (ret < 0) {
			printf(" ERROR! (ret = %d)\n", ret);
			goto err;
		}

		/* Set content override */
		ret = Device_SetOverride(dev);
		if (ret < 0) {
			printf(" ERROR! (ret = %d)\n", ret);
			goto err;
		} else
			printf(" OK!\n");
	}

	return;

err:
	/* Unmount device */
	Device_Unmount(dev);
}
