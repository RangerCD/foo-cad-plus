#ifndef FOOCAD_PLUSMSG_H
#define FOOCAD_PLUSMSG_H

#include <WinUser.h>

enum PlusMsg
{
	PlusMsg_SetAttribute = WM_USER + 128,
	PlusMsg_GetAttribute = WM_USER + 129,

	PlusMsg_User = WM_USER + 255
};

enum PlusAttr
{
	PlusAttr_CoverNotRequired	= 0x0000,
	PlusAttr_CoverRequired		= 0x0001
};

#endif