#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>

#include "title.h"


s32 Title_GetTitleKey(tik *p_tik, u8 *key)
{
	static u8 iv[16]  ATTRIBUTE_ALIGN(32);
	static u8 enc[16] ATTRIBUTE_ALIGN(32);
	static u8 dec[16] ATTRIBUTE_ALIGN(32);

	s32 ret;

	/* Set IV */
	memset(iv, 0, sizeof(iv));
	memcpy(iv, &p_tik->titleid, sizeof(u64));

	/* Set encrypted key */
	memset(enc, 0, sizeof(enc));
	memcpy(enc, &p_tik->cipher_title_key, sizeof(enc));

	/* Clear output buffer */
	memset(dec, 0, sizeof(dec));

	/* Decrypt title key */
	ret = ES_Decrypt(ES_KEY_COMMON, iv, enc, sizeof(enc), dec);
	if (ret < 0)
		return ret;

	/* Copy key */
	memcpy(key, dec, sizeof(dec));

	return 0;
}

s32 Title_SetTitleKey(tik *p_tik, u8 *key)
{
	static u8 iv[16]  ATTRIBUTE_ALIGN(32);
	static u8 enc[16] ATTRIBUTE_ALIGN(32);
	static u8 dec[16] ATTRIBUTE_ALIGN(32);

	s32 ret;

	/* Set IV */
	memset(iv, 0, sizeof(iv));
	memcpy(iv, &p_tik->titleid, sizeof(u64));

	/* Set decrypted key */
	memset(dec, 0, sizeof(dec));
	memcpy(dec, key, sizeof(dec));

	/* Clear output buffer */
	memset(enc, 0, sizeof(enc));

	/* Encrypt title key */
	ret = ES_Encrypt(ES_KEY_COMMON, iv, dec, sizeof(dec), enc);
	if (ret < 0)
		return ret;

	/* Copy key */
	memcpy(p_tik->cipher_title_key, enc, sizeof(enc));

	return 0;
}

s32 Title_ZeroSignature(signed_blob *p_sig)
{
	u8 *ptr = (u8 *)p_sig;

	/* Fill signature with zeroes */
	memset(ptr + 4, 0, SIGNATURE_SIZE(p_sig) - 4);

	return 0;
}

s32 Title_GetList(u64 **outbuf, u32 *outlen)
{
	u64 *titles = NULL;

	u32 len, nb_titles;
	s32 ret;

	/* Get number of titles */
	ret = ES_GetNumTitles(&nb_titles);
	if (ret < 0)
		return ret;

	/* Calculate buffer lenght */
	len = round_up(sizeof(u64) * nb_titles, 32);

	/* Allocate memory */
	titles = memalign(32, len);
	if (!titles)
		return -1;

	/* Get titles */
	ret = ES_GetTitles(titles, nb_titles);
	if (ret < 0)
		goto err;

	/* Set values */
	*outbuf = titles;
	*outlen = nb_titles;

	return 0;

err:
	/* Free memory */
	if (titles)
		free(titles);

	return ret;
}

s32 Title_GetTicketViews(u64 tid, tikview **outbuf, u32 *outlen)
{
	tikview *views = NULL;

	u32 nb_views;
	s32 ret;

	/* Get number of ticket views */
	ret = ES_GetNumTicketViews(tid, &nb_views);
	if (ret < 0)
		return ret;

	/* Allocate memory */
	views = (tikview *)memalign(32, sizeof(tikview) * nb_views);
	if (!views)
		return -1;

	/* Get ticket views */
	ret = ES_GetTicketViews(tid, views, nb_views);
	if (ret < 0)
		goto err;

	/* Set values */
	*outbuf = views;
	*outlen = nb_views;

	return 0;

err:
	/* Free memory */
	if (views)
		free(views);

	return ret;
}

s32 Title_GetTMD(u64 tid, signed_blob **outbuf, u32 *outlen)
{
	void *p_tmd = NULL;

	u32 len;
	s32 ret;

	/* Get TMD size */
	ret = ES_GetStoredTMDSize(tid, &len);
	if (ret < 0)
		return ret;

	/* Allocate memory */
	p_tmd = memalign(32, round_up(len, 32));
	if (!p_tmd)
		return -1;

	/* Read TMD */
	ret = ES_GetStoredTMD(tid, p_tmd, len);
	if (ret < 0)
		goto err;

	/* Set values */
	*outbuf = p_tmd;
	*outlen = len;

	return 0;

err:
	/* Free memory */
	if (p_tmd)
		free(p_tmd);

	return ret;
}

s32 Title_GetVersion(u64 tid, u16 *outbuf)
{
	signed_blob *p_tmd = NULL;
	tmd      *tmd_data = NULL;

	u32 len;
	s32 ret;

	/* Get title TMD */
	ret = Title_GetTMD(tid, &p_tmd, &len);
	if (ret < 0)
		return ret;

	/* Retrieve TMD info */
	tmd_data = (tmd *)SIGNATURE_PAYLOAD(p_tmd);

	/* Set values */
	*outbuf = tmd_data->title_version;

	/* Free memory */
	free(p_tmd);

	return 0;
}

s32 Title_GetSize(u64 tid, u32 *outbuf)
{
	signed_blob *p_tmd = NULL;
	tmd      *tmd_data = NULL;

	u32 cnt, len, size = 0;
	s32 ret;

	/* Get title TMD */
	ret = Title_GetTMD(tid, &p_tmd, &len);
	if (ret < 0)
		return ret;

	/* Retrieve TMD info */
	tmd_data = (tmd *)SIGNATURE_PAYLOAD(p_tmd);

	/* Calculate title size */
	for (cnt = 0; cnt < tmd_data->num_contents; cnt++) {
		tmd_content *content = &tmd_data->contents[cnt];

		/* Add content size */
		size += content->size;
	}

	/* Set values */
	*outbuf = size;

	/* Free memory */
	free(p_tmd);

	return 0;
}
