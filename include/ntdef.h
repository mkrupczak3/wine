/*
 * Copyright (C) 1999 Juergen Schmied
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

#ifndef __WINE_NTDEF_H
#define __WINE_NTDEF_H

#include "basetsd.h"
#include "windef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NTAPI   __stdcall

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef VOID
#define VOID void
#endif

typedef LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;

typedef short CSHORT;
typedef CSHORT *PCSHORT;


/* NT lowlevel Strings (handled by Rtl* functions in NTDLL)
 * If they are zero terminated, Length does not include the terminating 0.
 */

typedef struct _STRING {
	USHORT	Length;
	USHORT	MaximumLength;
	PSTR	Buffer;
} STRING,*PSTRING,ANSI_STRING,*PANSI_STRING;

typedef struct _CSTRING {
	USHORT	Length;
	USHORT	MaximumLength;
	PCSTR	Buffer;
} CSTRING,*PCSTRING;

typedef struct _UNICODE_STRING {
	USHORT	Length;		/* bytes */
	USHORT	MaximumLength;	/* bytes */
	PWSTR	Buffer;
} UNICODE_STRING,*PUNICODE_STRING;

/*
	Objects
*/

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_VALID_ATTRIBUTES    0x000003F2L

typedef struct _OBJECT_ATTRIBUTES
{   ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        /* type SECURITY_DESCRIPTOR */
    PVOID SecurityQualityOfService;  /* type SECURITY_QUALITY_OF_SERVICE */
} OBJECT_ATTRIBUTES;

typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes(p,n,a,r,s) \
{	(p)->Length = sizeof(OBJECT_ATTRIBUTES); \
	(p)->RootDirectory = r; \
	(p)->Attributes = a; \
	(p)->ObjectName = n; \
	(p)->SecurityDescriptor = s; \
	(p)->SecurityQualityOfService = NULL; \
}


#ifdef __cplusplus
}
#endif

#endif
