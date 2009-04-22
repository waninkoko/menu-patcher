#include <stdio.h>
#include <string.h>
#include <ogcsys.h>


s32 __Sysmenu_PatchApply(u8 *dol, u32 len, const u8 *orig, const u8 *patched, u32 patch_len)
{
	u32 cnt;

	/* Find code and patch it */
	for (cnt = 0; cnt < (len - patch_len); cnt++) {
		u8 *ptr = dol + cnt;

		if (!memcmp(ptr, orig, patch_len)) {
			/* Replace code */
			memcpy(ptr, patched, patch_len);

			return cnt;
		}
	}

	return -1;
}

void __Sysmenu_PatchRegionJap(u8 *dol, u32 len)
{
	const u8 pattern[] = { 0x2C, 0x1B, 0x00, 0x00 };
	const u8 patch[]   = { 0x60, 0x00, 0x00, 0x00 };

	u32 cnt;

	/* Find pattern */
	for (cnt = 0; cnt < (len - sizeof(pattern)); cnt +=4) {
		u8 *ptr = dol + cnt;

		/* Pattern found */
		if (!memcmp(ptr, pattern, sizeof(pattern))) {
			/* Patch JAP region */
			memcpy(ptr + 4, patch, sizeof(patch));

			return;
		}
	}
}

void __Sysmenu_PatchRegionUsa(u8 *dol, u32 len)
{
	const u8 pattern[] = { 0x28, 0x1B, 0x00, 0x01 };
	const u8 patch[]   = { 0x60, 0x00, 0x00, 0x00 };

	u32 cnt;

	/* Find pattern */
	for (cnt = 0; cnt < (len - sizeof(pattern)); cnt +=4) {
		u8 *ptr = dol + cnt;

		/* Pattern found */
		if (!memcmp(ptr, pattern, sizeof(pattern))) {
			/* Patch USA region */
			memcpy(ptr + 4, patch, sizeof(patch));

			return;
		}
	}
}

void __Sysmenu_PatchRegionPal(u8 *dol, u32 len)
{
	const u8 pattern1[] = { 0x28, 0x1B, 0x00, 0x02 };
	const u8 pattern2[] = { 0x40, 0x82, 0x00, 0x1C };

	const u8 patch1[]   = { 0x60, 0x00, 0x00, 0x00 };
	const u8 patch2[]   = { 0x48, 0x00, 0x00, 0x1C };

	u32 cnt;

	/* Find pattern */
	for (cnt = 0; cnt < (len - sizeof(pattern1)); cnt += 4) {
		u8 *ptr = dol + cnt;

		/* Pattern 1 found */
		if (!memcmp(ptr, pattern1, sizeof(pattern1)))
			memcpy(ptr + 4, patch1, sizeof(patch1));

		/* Pattern 2 found */
		if (!memcmp(ptr, pattern2, sizeof(pattern2))) {
			memcpy(ptr + 4, patch2, sizeof(patch2));

			return ;
		}
	}
}


void Sysmenu_PatchIOSReload(u8 *dol, u32 len)
{
	const u8 oldcode[] = { 0x7F, 0x06, 0xC3, 0x78, 0x7F, 0x25, 0xCB, 0x78, 0x38, 0x7E, 0x02, 0xC0, 0x4C, 0xC6, 0x31, 0x82 };
	const u8 newcode[] = { 0x3B, 0x20, 0x00, 0x01, 0x3B, 0x00, 0x00, 0xF9, 0x38, 0x7E, 0x02, 0xC0, 0x4C, 0xC6, 0x31, 0x82 };

	/* Patch IOS reload */
	__Sysmenu_PatchApply(dol, len, oldcode, newcode, sizeof(oldcode));
}

void Sysmenu_PatchUpdateCheck(u8 *dol, u32 len)
{
	const u8 pattern[] = { 0x80, 0x65, 0x00, 0x50, 0x80, 0x85, 0x00, 0x54, 0xA0, 0xA5, 0x00, 0x58 };

	u32 cnt;

	/* Find pattern */
	for (cnt = 0; cnt < (len - sizeof(pattern)); cnt += 4) {
		u8 *ptr = dol + cnt;

		/* Pattern found */
		if (!memcmp(ptr, pattern, sizeof(pattern))) {
			const u8 oldcode[] = { 0x40, 0x82, 0x00, 0x20 };
			const u8 newcode[] = { 0x60, 0x00, 0x00, 0x00 };

			u32 size = len - cnt;

			/* Patch update check */
			__Sysmenu_PatchApply(ptr, size, oldcode, newcode, sizeof(oldcode));
		}
	}
}

void Sysmenu_PatchRegionFree(u8 *dol, u32 len)
{
	const u8 pattern[] = { 0x7C, 0x60, 0x07, 0x74, 0x2C, 0x00, 0x00, 0x01, 0x41, 0x82,
			       0x00, 0x30, 0x40, 0x80, 0x00, 0x10, 0x2C, 0x00, 0x00, 0x00 };

	u32 cnt;

	/* Find pattern */
	for (cnt = 0; cnt < (len - sizeof(pattern)); cnt += 4) {
		u8 *ptr = dol + cnt;

		/* Pattern found */
		if (!memcmp(ptr, pattern, sizeof(pattern))) {
			u32 size = len - cnt;

			/* Patch regions */
			__Sysmenu_PatchRegionJap(ptr, size);
			__Sysmenu_PatchRegionUsa(ptr, size);
			__Sysmenu_PatchRegionPal(ptr, size);
		}
	}
}

void Sysmenu_PatchNocopySaves(u8 *dol, u32 len)
{
	{
		const u8 oldcode[] = { 0x54, 0x00, 0x07, 0xFF, 0x41, 0x82, 0x00, 0x1C, 0x80, 0x63, 0x00, 0x68 };
		const u8 newcode[] = { 0x7C, 0x00, 0x00, 0x00, 0x41, 0x82, 0x00, 0x1C, 0x80, 0x63, 0x00, 0x68 };

		/* Patch "nocopy" savegame check (1) */
		__Sysmenu_PatchApply(dol, len, oldcode, newcode, sizeof(oldcode));
	}
	{
		const u8 oldcode[] = { 0x54, 0x00, 0x07, 0xFF, 0x41, 0x82, 0x00, 0x24, 0x38, 0x7E, 0x12, 0xE2 };
		const u8 newcode[] = { 0x7C, 0x00, 0x00, 0x00, 0x41, 0x82, 0x00, 0x24, 0x38, 0x7E, 0x12, 0xE2 };

		/* Patch "nocopy" savegame check (2) */
		__Sysmenu_PatchApply(dol, len, oldcode, newcode, sizeof(oldcode));
	}
	{
		const u8 oldcode[] = { 0x48, 0x00, 0x00, 0x58, 0x38, 0x61, 0x01, 0x00, 0x38, 0x8D, 0x9F, 0x68, 0x48, 0x03, 0xC9, 0x25 };
		const u8 newcode[] = { 0x48, 0x00, 0x00, 0x58, 0x38, 0x61, 0x01, 0x00, 0x38, 0x8D, 0x9F, 0x68, 0x38, 0x60, 0x00, 0x01 };

		/* Patch "nocopy" savegame check (3) */
		__Sysmenu_PatchApply(dol, len, oldcode, newcode, sizeof(oldcode));
	}
	{
		const u8 oldcode[] = { 0x80, 0x01, 0x00, 0x08, 0x2C, 0x00, 0x00, 0x00, 0x41, 0x82, 0x00, 0x0C, 0x3B, 0xE0, 0x00, 0x01 };
		const u8 newcode[] = { 0x80, 0x01, 0x00, 0x08, 0x2C, 0x00, 0x00, 0x00, 0x41, 0x82, 0x00, 0x0C, 0x3B, 0xE0, 0x00, 0x00 };

		/* Patch "nocopy" savegame check (4) */
		__Sysmenu_PatchApply(dol, len, oldcode, newcode, sizeof(oldcode));
	}
	{
		const u8 oldcode[] = { 0x41, 0x82, 0x00, 0x24, 0x38, 0x7E, 0x12, 0xE2, 0x4C, 0xC6, 0x31, 0x82, 0x48, 0x1B, 0xB4, 0xC9 };
		const u8 newcode[] = { 0x80, 0x1D, 0x00, 0x24, 0x54, 0x00, 0x00, 0x3C, 0x90, 0x1D, 0x00, 0x24, 0x48, 0x00, 0x00, 0x18 };

		/* Patch "nocopy" savegame check (5) */
		__Sysmenu_PatchApply(dol, len, oldcode, newcode, sizeof(oldcode));
	}
}
