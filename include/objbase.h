/*
 * Copyright (C) 1998 Fran�ois Gouget
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

#ifndef __WINE_OBJBASE_H
#define __WINE_OBJBASE_H

#define _OBJBASE_H_

#include "unknwn.h"

/* the following depend only on obj_base.h */
#include "wine/obj_base.h"
#include "wine/obj_misc.h"
#include "wine/obj_channel.h"
#include "wine/obj_clientserver.h"
#include "wine/obj_storage.h"

/* the following depend on obj_storage.h */
#include "wine/obj_marshal.h"
#include "wine/obj_moniker.h"
#include "wine/obj_propertystorage.h"

/* the following depend on obj_moniker.h */
#include "wine/obj_dataobject.h"

#include "wine/obj_dragdrop.h"

#ifndef RC_INVOKED
/* For compatibility only, at least for now */
#include <stdlib.h>
#endif

#ifndef INITGUID
#include "cguid.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

HRESULT WINAPI GetClassFile(LPCOLESTR filePathName,CLSID *pclsid);

#ifdef __cplusplus
}
#endif

#ifndef __WINE__
/* These macros are msdev's way of defining COM objects.
 * They are provided here for use by Winelib developpers.
 */
#define FARSTRUCT
#define HUGEP

#define WINOLEAPI        STDAPI
#define WINOLEAPI_(type) STDAPI_(type)

#if defined(__cplusplus) && !defined(CINTERFACE)
#define interface struct
#define STDMETHOD(method)       virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method) virtual type STDMETHODCALLTYPE method
#define PURE                    = 0
#define THIS_
#define THIS                    void
#define DECLARE_INTERFACE(iface)    interface iface
#define DECLARE_INTERFACE_(iface, baseiface)    interface iface : public baseiface

#define BEGIN_INTERFACE
#define END_INTERFACE

#else

#define interface               struct
#define STDMETHOD(method)       HRESULT STDMETHODCALLTYPE (*method)
#define STDMETHOD_(type,method) type STDMETHODCALLTYPE (*method)
#define PURE
#define THIS_                   INTERFACE FAR* This,
#define THIS                    INTERFACE FAR* This

#ifdef CONST_VTABLE
#undef CONST_VTBL
#define CONST_VTBL const
#define DECLARE_INTERFACE(iface) \
         typedef interface iface { const struct iface##Vtbl FAR* lpVtbl; } iface; \
         typedef const struct iface##Vtbl iface##Vtbl; \
         const struct iface##Vtbl
#else
#undef CONST_VTBL
#define CONST_VTBL
#define DECLARE_INTERFACE(iface) \
         typedef interface iface { struct iface##Vtbl FAR* lpVtbl; } iface; \
         typedef struct iface##Vtbl iface##Vtbl; \
         struct iface##Vtbl
#endif
#define DECLARE_INTERFACE_(iface, baseiface)    DECLARE_INTERFACE(iface)

#define BEGIN_INTERFACE
#define END_INTERFACE

#endif /* __cplusplus && !CINTERFACE */

#endif /* __WINE__ */

#endif /* __WINE_OBJBASE_H */
