#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>

#include "state.h"
#include "sys.h"
#include "sysmenu_patches.h"

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

/* Ticket structure */
typedef struct {
	sig_rsa2048 signature;
	tik tik_data;
} ticket;

/* Constants */
#define SYSMENU_ADDR		0x8132FF80
#define SYSMENU_LEN		0x380000
#define SYSMENU_TITLEID		0x100000002ULL


void Sysmenu_Patch(u8 *dol, u32 len)
{
	/* Apply system menu patches */
	Sysmenu_PatchIOSReload(dol, len);
	Sysmenu_PatchRegionFree(dol, len);
	Sysmenu_PatchUpdateCheck(dol, len);
	Sysmenu_PatchNocopySaves(dol, len);
}

s32 Sysmenu_Load(u32 *entry, u16 index)
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

s32 Sysmenu_Identify(void)
{
	static ticket s_tik ATTRIBUTE_ALIGN(32);
	signed_blob *p_certs = NULL, *p_tik = NULL, *p_tmd = NULL;

	u32 certs_len, tik_len, tmd_len;
	s32 ret;

	/* Retrieve certificates */
	ret = Sys_GetCerts(&p_certs, &certs_len);
	if (ret < 0)
		return ret;

	/* Set ticket length */
	tik_len = sizeof(s_tik);

	memset(&s_tik, 0, tik_len);

	/* Generate ticket */
	strcpy(s_tik.tik_data.issuer, "Root-CA00000001-XS00000003");
	memset(s_tik.tik_data.cidx_mask, 0xFF, 32);

	s_tik.signature.type = ES_SIG_RSA2048;

	/* Set pointer */
	p_tik = (signed_blob *)&s_tik;

	/* Retrieve TMD length */
	ret = ES_GetStoredTMDSize(SYSMENU_TITLEID, &tmd_len);
	if (ret < 0)
		goto out;

	/* Allocate memory */
	p_tmd = (signed_blob *)memalign(32, tmd_len);
	if (!p_tmd) {
		ret = -1;
		goto out;
	}

	/* Retrieve TMD */
	ret = ES_GetStoredTMD(SYSMENU_TITLEID, p_tmd, tmd_len);
	if (ret < 0)
		goto out;

	/* Identify as system menu */
	ret = ES_Identify(p_certs, certs_len, p_tmd, tmd_len, p_tik, tik_len, NULL);

out:
	/* Free memory */
	if (p_tmd)
		free(p_tmd);

	return ret;
}

s32 Sysmenu_Bootindex(void)
{
	signed_blob *p_tmd = NULL;

	u32 len;
	s32 ret;

	/* Retrieve TMD length */
	ret = ES_GetStoredTMDSize(SYSMENU_TITLEID, &len);
	if (ret < 0)
		goto out;

	/* Allocate memory */
	p_tmd = (signed_blob *)memalign(32, len);
	if (!p_tmd)
		return -1;

	/* Retrieve TMD */
	ret = ES_GetStoredTMD(SYSMENU_TITLEID, p_tmd, len);
	if (ret < 0)
		goto out;

	/* Get boot index */
	ret = ((tmd *)SIGNATURE_PAYLOAD(p_tmd))->boot_index;

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
	ret = Sysmenu_Identify();
	if (ret < 0) {
		printf("[+] ERROR: Could not identify as system menu! (ret = %d)\n", ret);
		return ret;
	}

	printf("[+] Loading System Menu into memory...");
	fflush(stdout);

	/* Get system menu boot index */
	ret = Sysmenu_Bootindex();
	if (ret < 0) {
		printf("[+] ERROR: Could not retrieve boot index! (ret = %d)\n", ret);
		return ret;
	}

	index = ret;

	/* Load system menu */
	ret = Sysmenu_Load(&entry, index);
	if (ret < 0) {
		printf("[+] ERROR: Could not load system menu executable! (ret = %d)\n", ret);
		return ret;
	}
	printf(" OK!\n");

	/* Patch system menu */
	Sysmenu_Patch((u8 *)SYSMENU_ADDR, SYSMENU_LEN);

	/* Set state flags */
	ret = State_ReturnToMenu();
	if (ret < 0) {
		printf("[+] ERROR: Could not set state flags! (ret = %d)\n", ret);
		return ret;
	}

	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	DCFlushRange((u8 *)SYSMENU_ADDR, SYSMENU_LEN);

	/* Jump to entry point */
	unstub_start(entry);

	printf(" returned!\n");

	return 0;
}
