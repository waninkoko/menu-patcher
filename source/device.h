#ifndef _FATDEV_H_
#define _FATDEV_H_

/* 'FAT Device' structure */
typedef struct {
	/* Device name */
	char *name;

	/* Mode value */
	u32 mode;

	/* Un/mount command */
	u32 mount;
	u32 umount;
} fatDevice; 


/* Prototypes */
s32 Device_Mount(fatDevice *);
s32 Device_Unmount(fatDevice *);

s32 Device_SetOverride(fatDevice *);
s32 Device_UnsetOverride(void);

#endif
