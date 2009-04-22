/*-------------------------------------------------------------

wiilaunch.c -- Wii NAND title launching and argument passing

Copyright (C) 2008
Hector Martin (marcan)

Modified by Waninkoko


This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.

-------------------------------------------------------------*/

#if defined(HW_RVL)

#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ogcsys.h>

#include "state.h"

static char __stateflags[] ATTRIBUTE_ALIGN(32) = "/title/00000001/00000002/data/state.dat";
static int  __initialized = 0;

typedef struct {
	u32 checksum;
	u8 flags;
	u8 type;
	u8 discstate;
	u8 returnto;
	u32 unknown[6];
} StateFlags;

#define TYPE_RETURN 3
#define TYPE_NANDBOOT 4
#define TYPE_SHUTDOWNSYSTEM 5
#define RETURN_TO_MENU 0
#define RETURN_TO_SETTINGS 1
#define RETURN_TO_ARGS 2

static StateFlags stateflags ATTRIBUTE_ALIGN(32);


static u32 __CalcChecksum(u32 *buf, int len)
{
	u32 sum = 0;
	int i;
	len = (len/4);

	for(i=1; i<len; i++)
		sum += buf[i];

	return sum;
}

static void __SetChecksum(void *buf, int len)
{
	u32 *p = (u32*)buf;
	p[0] = __CalcChecksum(p, len);
}

static int __ValidChecksum(void *buf, int len)
{
	u32 *p = (u32*)buf;
	return p[0] == __CalcChecksum(p, len);
}

static s32 __State_ReadFlags(void)
{
	int fd;
	int ret;

	fd = IOS_Open(__stateflags, IPC_OPEN_READ);
	if(fd < 0) {
		memset(&stateflags, 0, sizeof(stateflags));
		return STATE_EINTERNAL;
	}

	ret = IOS_Read(fd, &stateflags, sizeof(stateflags));
	IOS_Close(fd);

	if(ret != sizeof(stateflags)) {
		memset(&stateflags, 0, sizeof(stateflags));
		return STATE_EINTERNAL;
	}

	if(!__ValidChecksum(&stateflags, sizeof(stateflags))) {
		memset(&stateflags, 0, sizeof(stateflags));
		return STATE_ECHECKSUM;
	}

	return 0;
}

static s32 __State_WriteFlags(void)
{
	int fd;
	int ret;

	__SetChecksum(&stateflags, sizeof(stateflags));

	fd = IOS_Open(__stateflags, IPC_OPEN_READ|IPC_OPEN_WRITE);
	if(fd < 0)
		return STATE_EINTERNAL;

	ret = IOS_Write(fd, &stateflags, sizeof(stateflags));
	IOS_Close(fd);
	if(ret != sizeof(stateflags))
		return STATE_EINTERNAL;

	return 0;
}

s32 State_Initialize(void)
{
	int ret;

	ret = __State_ReadFlags();
	if (ret < 0)
		return ret;

	__initialized = 1;

	return 0;
}

s32 State_ReturnToMenu(void)
{
	if(!__initialized)
		return STATE_ENOTINIT;

	stateflags.type = TYPE_RETURN;
	stateflags.returnto = RETURN_TO_MENU;

	return __State_WriteFlags();
}

#endif
