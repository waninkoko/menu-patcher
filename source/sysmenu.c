#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>

#include "identify.h"
#include "state.h"
#include "sys.h"
#include "sysmenu_patches.h"
#include "title.h"

/* Externs */
extern void unstub_start(u32);

/* Dol Header structure */
typedef struct {
	u32 textoff[7];
	u32 dataoff[11];
	u8 *textmem[7];
	u8 *datamem[11];
	u32 textsize[7];
	u32 datasize[11];
	u8 *bssmem;
	u32 bsssize;
	u32 entry;
	u32 unused[7];
} dolhdr;

/* Constants */
#define SYSMENU_ADDR		0x8132FF80
#define SYSMENU_LEN		0x380000
#define SYSMENU_TITLEID		0x100000002ULL


void __Sysmenu_Patch(u8 *dol, u32 len)
{
	/* Apply system menu patches */
	Sysmenu_PatchVersion(dol, len);
	Sysmenu_PatchIOSReload(dol, len);
	Sysmenu_PatchRegion(dol, len);
	Sysmenu_PatchUpdate(dol, len);
	Sysmenu_PatchNocopy(dol, len);
}

s32 __Sysmenu_Load(u32 *entry, u16 index)
{
	static dolhdr dol ATTRIBUTE_ALIGN(32);

	u32 cnt;
	s32 fd, ret;

	/* Open content */
	fd = ES_OpenContent(index);
	if (fd < 0)
		return fd;

	/* Read content */
	ret = ES_ReadContent(fd, (u8 *)&dol, sizeof(dolhdr));
	if (ret < 0)
		return ret;

	memset(dol.bssmem, 0, dol.bsssize);

	/* Read data */
	for (cnt = 0; cnt < 7; cnt++)
		if (dol.textoff[cnt] >= sizeof(dolhdr)) {
			ret = ES_SeekContent(fd, dol.textoff[cnt], 0);
			if (ret != dol.textoff[cnt])
				return -1;

			ret = ES_ReadContent(fd, dol.textmem[cnt], dol.textsize[cnt]);
			if (ret != dol.textsize[cnt])
				return -1;
		}

	for (cnt = 0; cnt < 11; cnt++)
		if (dol.dataoff[cnt] >= sizeof(dolhdr)) {
			ret = ES_SeekContent(fd, dol.dataoff[cnt], 0);
			if (ret != dol.dataoff[cnt])
				return -1;

			ret = ES_ReadContent(fd, dol.datamem[cnt], dol.datasize[cnt]);
			if (ret != dol.datasize[cnt])
				return -1;
		}

	/* Close content */
	ES_CloseContent(fd);

	/* Set entry point */
	*entry = dol.entry;

	return 0;
}

s32 __Sysmenu_BootIndex(void)
{
	signed_blob *p_tmd = NULL;
	tmd      *tmd_data = NULL;

	u32 len;
	s32 ret;

	/* Retrieve TMD */
	ret = Title_GetTMD(SYSMENU_TITLEID, &p_tmd, &len);
	if (ret < 0)
		goto out;

	/* TMD data */
	tmd_data = (tmd *)SIGNATURE_PAYLOAD(p_tmd);

	/* Get boot index */
	ret = tmd_data->boot_index;

out:
	/* Free memory */
	if (p_tmd)
		free(p_tmd);

	return ret;
}


s32 Sysmenu_Launch(void)
{
	u32 entry, index;
	s32 ret;

	/* Read current state flags */
	ret = State_Initialize();
	if (ret < 0) {
		printf("[+] ERROR: Could not read current state flags! (ret = %d)\n", ret);
		return ret;
	}

	/* Identify as system menu */
	ret = Identify_AsTitle(SYSMENU_TITLEID);
	if (ret < 0) {
		printf("[+] ERROR: Could not identify as system menu! (ret = %d)\n", ret);
		return ret;
	}

	printf("[+] Loading System Menu into memory...");
	fflush(stdout);

	/* Get system menu boot index */
	ret = __Sysmenu_BootIndex();
	if (ret < 0) {
		printf(" ERROR! Could not retrieve boot index! (ret = %d)\n", ret);
		return ret;
	}

	index = ret;

	/* Load system menu */
	ret = __Sysmenu_Load(&entry, index);
	if (ret < 0) {
		printf(" ERROR! Could not load system menu executable! (ret = %d)\n", ret);
		return ret;
	}

	/* Patch system menu */
	__Sysmenu_Patch((u8 *)SYSMENU_ADDR, SYSMENU_LEN);

	/* Set state flags */
	ret = State_ReturnToMenu();
	if (ret < 0) {
		printf(" ERROR! Could not set state flags! (ret = %d)\n", ret);
		return ret;
	}

	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	DCFlushRange((u8 *)SYSMENU_ADDR, SYSMENU_LEN);

	/* Jump to entry point */
	unstub_start(entry);

	printf(" returned!\n");

	return 0;
}
