/*-------------------------------------------------------------

wiilaunch.h -- Wii NAND title launching and argument passing

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

#ifndef __STATE_H__
#define __STATE_H__

#include <gctypes.h>
#include <gcutil.h>

// not initialized
#define STATE_ENOTINIT	-0x9001
// internal error
#define STATE_EINTERNAL	-0x9002
// checksum error
#define STATE_ECHECKSUM	-0x9003
// required title not installed
#define STATE_EINSTALL	-0x9004
// argument list too big
#define STATE_E2BIG	-0x9005

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

s32 State_Initialize(void);
s32 State_ReturnToMenu(void);

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif

#endif
