/*
 * Int67 (EMS) emulation
 *
 * Copyright 2002 Jukka Heinonen
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

#include <assert.h>
#include "wine/winbase16.h"
#include "dosexe.h"
#include "miscemu.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(int);

/*
 * EMS page size == 16 kilobytes.
 */
#define EMS_PAGE_SIZE (16*1024)

/*
 * Linear address of EMS page.
 */
#define EMS_PAGE_ADDRESS(base,page) (((char*)base) + EMS_PAGE_SIZE * page)

/*
 * Maximum number of pages that can be allocated using EMS.
 */
#define EMS_MAX_PAGES 1024

/*
 * Maximum number of EMS handles (allocated blocks).
 */
#define EMS_MAX_HANDLES 256

/*
 * Global EMM Import Record.
 * Applications can get address of this record
 * and directly access allocated memory if they use
 * IOCTL interface.
 *
 * FIXME: Missing lots of fields, packing is not correct.
 */

struct {
  struct {
    UCHAR hindex;  /* handle number */
    BYTE  flags;   /* bit 0: normal handle rather than system handle */
    char  name[8]; /* handle name */
    WORD  pages;   /* allocated pages */
    void *address; /* physical address*/
  } handle[EMS_MAX_HANDLES];

  /* Wine specific fields... */

  int   used_pages;     /* Number of allocated pages. */
  void *frame_address;  /* Address of 64k EMS page frame */
  WORD  frame_selector; /* Segment of 64k EMS page frame */

  struct {
    UCHAR hindex;       /* handle number */
    WORD  logical_page; /* logical page */
  } mapping[4];

} *EMS_record = 0;

/**********************************************************************
 *          EMS_init
 *
 * Allocates and initialized page frame and EMS global import record.
 */
static void EMS_init(void)
{
  /*
   * FIXME: Should dynamically allocate upper memory block for EMS frame.
   */
  ULONG base = 0xd0000;

  if(EMS_record)
    return;

  EMS_record = HeapAlloc(GetProcessHeap(),
                         HEAP_ZERO_MEMORY,
                         sizeof(*EMS_record));

  EMS_record->frame_address = DOSMEM_MapDosToLinear(base);
  EMS_record->frame_selector = base >> 4;
}

/**********************************************************************
 *          EMS_alloc
 *
 * Get handle and allocate memory.
 */
static void EMS_alloc( CONTEXT86 *context )
{
  int hindex = 1; /* handle zero is reserved for system */

  while(hindex < EMS_MAX_HANDLES && EMS_record->handle[hindex].address)
    hindex++;

  if(hindex == EMS_MAX_HANDLES) {
    AH_reg(context) = 0x85; /* status: no more handles available */
  } else {
    int   pages = BX_reg(context);
    void *buffer = HeapAlloc( GetProcessHeap(), 0, pages * EMS_PAGE_SIZE );

    if(!buffer) {
      AH_reg(context) = 0x88; /* status: insufficient pages available */
    } else {
      EMS_record->handle[hindex].address = buffer;
      EMS_record->handle[hindex].pages = pages;
      EMS_record->used_pages += pages;

      DX_reg(context) = hindex; /* handle to allocated memory*/
      AH_reg(context) = 0;      /* status: ok */
    }
  }
}

/**********************************************************************
 *          EMS_access_name
 *
 * Get/set handle name.
 */
static void EMS_access_name( CONTEXT86 *context )
{
  char *ptr;
  int hindex = DX_reg(context);
  if(hindex < 0 || hindex >= EMS_MAX_HANDLES) {
    AH_reg(context) = 0x83; /* invalid handle */
    return;
  }

  switch AL_reg(context) {
  case 0x00: /* get name */
    ptr = MapSL(MAKESEGPTR(context->SegEs, DI_reg(context)));
    memcpy(ptr, EMS_record->handle[hindex].name, 8);
    AH_reg(context) = 0;
    break;

  case 0x01: /* set name */
    ptr = MapSL(MAKESEGPTR(context->SegDs, SI_reg(context)));
    memcpy(EMS_record->handle[hindex].name, ptr, 8);
    AH_reg(context) = 0;
    break;

  default:
    INT_BARF(context,0x67);
    break;
  }
}

/**********************************************************************
 *          EMS_map
 *
 * Map logical page into physical page.
 */
static void EMS_map( CONTEXT86 *context )
{
  int   physical_page = AL_reg(context);
  int   new_hindex = DX_reg(context);
  int   new_logical_page = BX_reg(context);

  int   old_hindex = EMS_record->mapping[physical_page].hindex;
  int   old_logical_page = EMS_record->mapping[physical_page].logical_page;

  void *physical_address = EMS_PAGE_ADDRESS(EMS_record->frame_address,
                                            physical_page);

  /* unmap old page */
  if(old_hindex) {
    void *ptr = EMS_PAGE_ADDRESS(EMS_record->handle[old_hindex].address,
                                 old_logical_page);
    memcpy(ptr, physical_address, EMS_PAGE_SIZE);
  }

  /* map new page */
  if(new_hindex && new_logical_page != 0xffff) {
    void *ptr = EMS_PAGE_ADDRESS(EMS_record->handle[new_hindex].address,
                                 new_logical_page);
    memcpy(physical_address, ptr, EMS_PAGE_SIZE);
    EMS_record->mapping[physical_page].hindex = new_hindex;
    EMS_record->mapping[physical_page].logical_page = new_logical_page;
  } else {
    EMS_record->mapping[physical_page].hindex = 0;
    EMS_record->mapping[physical_page].logical_page = 0;
  }

  AH_reg(context) = 0; /* status: ok */
}

/**********************************************************************
 *          EMS_free
 *
 * Free memory and release handle.
 */
static void EMS_free( CONTEXT86 *context )
{
  int hindex = DX_reg(context);
  int i;

  if(hindex < 0 || hindex >= EMS_MAX_HANDLES) {
    AH_reg(context) = 0x83; /* status: invalid handle */
    return;
  }

  if(!EMS_record->handle[hindex].address) {
    AH_reg(context) = 0; /* status: ok */
    return;
  }

  EMS_record->used_pages -= EMS_record->handle[hindex].pages;

  /* unmap pages */
  for(i=0; i<4; i++)
    if(EMS_record->mapping[i].hindex == hindex)
      EMS_record->mapping[i].hindex = 0;

  /* free block */
  HeapFree( GetProcessHeap(), 0, EMS_record->handle[hindex].address );
  EMS_record->handle[hindex].address = 0;

  AH_reg(context) = 0;    /* status: ok */
}

/**********************************************************************
 *          DOSVM_Int67Handler
 *
 * Handler for interrupt 67h EMS routines.
 */
void WINAPI DOSVM_Int67Handler( CONTEXT86 *context )
{
  switch AH_reg(context) {

  case 0x40: /* EMS - GET MANAGER STATUS */
    AH_reg(context) = 0; /* status: ok */
    break;

  case 0x41: /* EMS - GET PAGE FRAME SEGMENT */
    EMS_init();
    BX_reg(context) = EMS_record->frame_selector; /* segment of page frame */
    AH_reg(context) = 0;                          /* status: ok */
    break;

  case 0x42: /* EMS - GET NUMBER OF PAGES */
    EMS_init();
    /* unallocated 16k pages */
    BX_reg(context) = EMS_MAX_PAGES - EMS_record->used_pages;
    /* total number of 16k pages */
    DX_reg(context) = EMS_MAX_PAGES;
    /* status: ok */
    AH_reg(context) = 0;
    break;

  case 0x43: /* EMS - GET HANDLE AND ALLOCATE MEMORY */
    EMS_init();
    EMS_alloc(context);
    break;

  case 0x44: /* EMS - MAP MEMORY */
    EMS_init();
    EMS_map(context);
    break;

  case 0x45: /* EMS - RELEASE HANDLE AND MEMORY */
    EMS_init();
    EMS_free(context);
    break;

  case 0x46: /* EMS - GET EMM VERSION */
    AL_reg(context) = 0x40; /* version 4.0 */
    AH_reg(context) = 0;    /* status: ok */
    break;

  case 0x47: /* EMS - SAVE MAPPING CONTEXT */
  case 0x48: /* EMS - RESTORE MAPPING CONTEXT */
    INT_BARF(context,0x67);
    break;

  case 0x49: /* EMS - reserved - GET I/O PORT ADDRESSES */
  case 0x4a: /* EMS - reserved - GET TRANSLATION ARRAY */
    INT_BARF(context,0x67);
    break;

  case 0x4b: /* EMS - GET NUMBER OF EMM HANDLES */
    BX_reg(context) = EMS_MAX_HANDLES; /* EMM handles */
    AH_reg(context) = 0;               /* status: ok */
    break;

  case 0x4c: /* EMS - GET PAGES OWNED BY HANDLE */
  case 0x4d: /* EMS - GET PAGES FOR ALL HANDLES */
  case 0x4e: /* EMS - GET OR SET PAGE MAP */
  case 0x4f: /* EMS 4.0 - GET/SET PARTIAL PAGE MAP */
  case 0x50: /* EMS 4.0 - MAP/UNMAP MULTIPLE HANDLE PAGES */
  case 0x51: /* EMS 4.0 - REALLOCATE PAGES */
  case 0x52: /* EMS 4.0 - GET/SET HANDLE ATTRIBUTES */
    INT_BARF(context,0x67);
    break;

  case 0x53: /* EMS 4.0 - GET/SET HANDLE NAME */
    EMS_init();
    EMS_access_name(context);
    break;

  case 0x54: /* EMS 4.0 - GET HANDLE DIRECTORY */
  case 0x55: /* EMS 4.0 - ALTER PAGE MAP AND JUMP */
  case 0x56: /* EMS 4.0 - ALTER PAGE MAP AND CALL */
  case 0x57: /* EMS 4.0 - MOVE/EXCHANGE MEMORY REGION */
  case 0x58: /* EMS 4.0 - GET MAPPABLE PHYSICAL ADDRESS ARRAY */
  case 0x59: /* EMS 4.0 - GET EXPANDED MEMORY HARDWARE INFORMATION */
  case 0x5a: /* EMS 4.0 - ALLOCATE STANDARD/RAW PAGES */
  case 0x5b: /* EMS 4.0 - ALTERNATE MAP REGISTER SET */
  case 0x5c: /* EMS 4.0 - PREPARE EXPANDED MEMORY HARDWARE FOR WARM BOOT */
  case 0x5d: /* EMS 4.0 - ENABLE/DISABLE OS FUNCTION SET FUNCTIONS */
  default:
    INT_BARF(context,0x67);
  }
}


/**********************************************************************
 *          EMS_Ioctl_Handler
 *
 * Handler for interrupt 21h IOCTL routine for device "EMMXXXX0".
 */
void WINAPI EMS_Ioctl_Handler( CONTEXT86 *context )
{
  assert(AH_reg(context) == 0x44);

  switch AL_reg(context) {
  case 0x00: /* IOCTL - GET DEVICE INFORMATION */
      RESET_CFLAG(context); /* operation was successful */
      DX_reg(context) = 0x4080; /* bit 14 (support ioctl read) and
                                 * bit 7 (is_device) */
      break;

  case 0x02: /* EMS - GET MEMORY MANAGER INFORMATION */
      /*
       * This is what is called "Windows Global EMM Import Specification".
       * Undocumented of course! Supports three requests:
       * GET API ENTRY POINT
       * GET EMM IMPORT STRUCTURE ADDRESS
       * GET MEMORY MANAGER VERSION
       */
      INT_BARF(context,0x21);
      break;

  case 0x07: /* IOCTL - GET OUTPUT STATUS */
      RESET_CFLAG(context); /* operation was successful */
      AL_reg(context) = 0xff; /* device is ready */
      break;

  default:
      INT_BARF(context,0x21);
      break;
  }
}
