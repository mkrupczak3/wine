/*
 * Message Logging functions
 *
 * Copyright 1994 Bob Amstadt
 * Copyright 1995 Alex Korobka
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __WINE_SPY_H
#define __WINE_SPY_H

#include "windef.h"

#define SPY_DISPATCHMESSAGE16     0x0100
#define SPY_DISPATCHMESSAGE     0x0101
#define SPY_SENDMESSAGE16         0x0102
#define SPY_SENDMESSAGE         0x0103
#define SPY_DEFWNDPROC16          0x0104
#define SPY_DEFWNDPROC          0x0105

#define SPY_RESULT_OK16           0x0000
#define SPY_RESULT_OK           0x0001
#define SPY_RESULT_INVALIDHWND16  0x0002
#define SPY_RESULT_INVALIDHWND  0x0003
#define SPY_RESULT_DEFWND16       0x0004
#define SPY_RESULT_DEFWND       0x0005


extern const char *SPY_GetMsgName( UINT msg, HWND hWnd );
extern const char *SPY_GetVKeyName(WPARAM wParam);
extern void SPY_EnterMessage( INT iFlag, HWND hwnd, UINT msg,
                              WPARAM wParam, LPARAM lParam );
extern void SPY_ExitMessage( INT iFlag, HWND hwnd, UINT msg,
                             LRESULT lReturn, WPARAM wParam, LPARAM lParam );
extern int SPY_Init(void);

#endif /* __WINE_SPY_H */
