#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>

#include "sys.h"
#include "title.h"


s32 __Identify_GenerateTik(signed_blob **outbuf, u32 *outlen)
{
	signed_blob *buffer   = NULL;

	sig_rsa2048 *signature = NULL;
	tik         *tik_data  = NULL;

	u32 len;

	/* Set ticket length */
	len = STD_SIGNED_TIK_SIZE;

	/* Allocate memory */
	buffer = (signed_blob *)memalign(32, len);
	if (!buffer)
		return -1;

	/* Clear buffer */
	memset(buffer, 0, len);

	/* Generate signature */
	signature       = (sig_rsa2048 *)buffer;
	signature->type = ES_SIG_RSA2048;

	/* Generate ticket */
	tik_data  = (tik *)SIGNATURE_PAYLOAD(buffer);

	strcpy(tik_data->issuer, "Root-CA00000001-XS00000003");
	memset(tik_data->cidx_mask, 0xFF, 32);

	/* Set values */
	*outbuf = buffer;
	*outlen = len;

	return 0;
}


s32 Identify_AsTitle(u64 tid)
{
	signed_blob *p_certs = NULL, *p_tik = NULL, *p_tmd = NULL;

	u32 certs_len, tik_len, tmd_len;
	s32 ret;

	/* Retrieve certificates */
	ret = Sys_GetCerts(&p_certs, &certs_len);
	if (ret < 0)
		goto out;

	/* Retrieve ticket */
	ret = __Identify_GenerateTik(&p_tik, &tik_len);
	if (ret < 0)
		goto out;

	/* Retrieve TMD */
	ret = Title_GetTMD(tid, &p_tmd, &tmd_len);
	if (ret < 0)
		goto out;

	/* Identify as system menu */
	ret = ES_Identify(p_certs, certs_len, p_tmd, tmd_len, p_tik, tik_len, NULL);

out:
	/* Free memory */
	if (p_tik)
		free(p_tik);
	if (p_tmd)
		free(p_tmd);

	return ret;
}
