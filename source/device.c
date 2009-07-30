#include <stdio.h>
#include <ogcsys.h>

#include "device.h"

/* IOCTL buffers */
static u32    buffer[8]  ATTRIBUTE_ALIGN(32);
static ioctlv vector[2] ATTRIBUTE_ALIGN(32);


s32 Device_Mount(fatDevice *dev)
{
	s32 fd, ret;

	/* Open FAT module */
	fd = IOS_Open("fat", 0);
	if (fd < 0)
		return fd;

	/* Mount device */
	ret = IOS_Ioctlv(fd, dev->mount, 0, 0, vector);

	/* Close FAT module */
	IOS_Close(fd);

	return ret;
}

s32 Device_Unmount(fatDevice *dev)
{
	s32 fd, ret;

	/* Open FAT module */
	fd = IOS_Open("fat", 0);
	if (fd < 0)
		return fd;

	/* Unmount device */
	ret = IOS_Ioctlv(fd, dev->umount, 0, 0, vector);

	/* Close FAT module */
	IOS_Close(fd);

	return ret;
}

s32 Device_SetOverride(fatDevice *dev)
{
	s32 fd, ret;

	/* Open /dev/es */
	fd = IOS_Open("/dev/es", 0);
	if (fd < 0)
		return fd;

	/* Set mode */
	buffer[0] = dev->mode;

	/* Setup vector */
	vector[0].data = buffer;
	vector[0].len  = 4;

	/* Set content override */
	ret = IOS_Ioctlv(fd, 0xE0, 1, 0, vector);

	/* Close /dev/es */
	IOS_Close(fd);

	return ret;
}

s32 Device_UnsetOverride(void)
{
	s32 fd, ret;

	/* Open /dev/es */
	fd = IOS_Open("/dev/es", 0);
	if (fd < 0)
		return fd;

	/* Set mode */
	buffer[0] = 0;

	/* Setup vector */
	vector[0].data = buffer;
	vector[0].len  = 4;

	/* Set content override */
	ret = IOS_Ioctlv(fd, 0xE0, 1, 0, vector);

	/* Close /dev/es */
	IOS_Close(fd);

	return ret;
}
