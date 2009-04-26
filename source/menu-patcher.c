#include <stdio.h>
#include <ogcsys.h>

#include "gui.h"
#include "restart.h"
#include "sysmenu.h"
#include "sys.h"
#include "video.h"
#include "wpad.h"


int main(int argc, char **argv)
{
	s32 ret;

	/* Load Custom IOS */
	ret = IOS_ReloadIOS(249);

	/* Initialize subsystems */
	Sys_Init();

	/* Set video mode */
	Video_SetMode();

	/* Initialize console */
	Gui_InitConsole();

	/* Draw background */
	Gui_DrawBackground();

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
