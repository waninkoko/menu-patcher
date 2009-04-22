#ifndef _TITLE_H_
#define _TITLE_H_

/* Constants */
#define BLOCK_SIZE	1024

/* Macros */
#define round_up(x,n)	(-(-(x) & -(n)))

/* Prototypes */
s32 Title_GetTitleKey(tik *, u8 *);
s32 Title_SetTitleKey(tik *, u8 *);
s32 Title_ZeroSignature(signed_blob *);
s32 Title_GetList(u64 **, u32 *);
s32 Title_GetTicketViews(u64, tikview **, u32 *);
s32 Title_GetTMD(u64, signed_blob **, u32 *);
s32 Title_GetVersion(u64, u16 *);
s32 Title_GetSize(u64, u32 *);

#endif
