/*******************************************************************************
** Header File Name:
**    HtiPrivate.h
**
** Purpose:
**    Private header file for the Header-to-ID conversion library.
**
** Description:
**    Contains the private macros, type definitions and function prototypes 
**    needed to use the Header-to-ID conversion library.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    -
**
** Author(s):
**    Martyn J. Ford (mjf)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: HtiPrivate.h,v 0.11 2005/08/01 12:55:24 sxf Exp $
**
** History:
**    $Log: HtiPrivate.h,v $
**    Revision 0.11  2005/08/01 12:55:24  sxf
**    Include Wfl.h to correctly determine paths on QNX6.
**
**    Revision 0.10  2005/06/20 13:29:34  mjf
**    Tidied to aid porting to Linux - no functional changes.
**
**    Revision 0.9  2005/06/17 09:26:34  mjf
**    Aim to open HTI tables in directory '/ram' and default to '/ttl/sw/etc'
**    if not located there.
**
**    Revision 0.8  2003/02/05 11:04:19  dws
**    Added defintion for Bol marker.
**
**    Revision 0.7  2003/01/13 15:07:34  dws
**    Added defintions for use with Oid functions.
**
**    Revision 0.6  2003/01/09 16:05:47  dws
**    Added defintion for EOL string for Oid lists.
**
**    Revision 0.5  2002/10/17 09:38:25  dws
**     Moved internal definitions for default files to external.
**     moved from HtiPrivate.h to Hti.h
**
**    Revision 0.4  2002/10/15 15:48:41  dws
**    Added Define for status errorcode files. Added commandline switch
**    for genbinary program. this switch allows tell the system not to
**    convert the first column in the text index file to a Cilname.
**
**    Revision 0.3  2002/04/18 15:22:45  djm
**    Added symbolic constants for the filenames of the binary
**    index files. Also several other minor changes.
**
**    Revision 0.2  2001/06/08 09:22:55  mjf
**    Fix to look up labels correctly when the description string is empty.
**
**    Revision 0.1  2001/02/14 13:45:20  mjf
**    Initial revision.
**
**
*******************************************************************************/


#ifndef HTI_PRIVATE_H_DEFINED
#define HTI_PRIVATE_H_DEFINED


/*
** Compiler include files 
*/

/* for FILENAME_MAX */

#include <stdio.h>


/*
** System include files
*/

#include "Wfl.h"
#include "Cil.h"


/*
** Type definitions
*/

typedef struct
{
   char     IdTable       [ FILENAME_MAX ];
   char     UnitTable     [ FILENAME_MAX ];
   char     FullTable     [ FILENAME_MAX ];
   char     StatusTable   [ FILENAME_MAX ];
   char     BinIdTable    [ FILENAME_MAX ];
   char     BinUnitTable  [ FILENAME_MAX ];
   char     BinFullTable  [ FILENAME_MAX ];
   char     BinStatusTable[ FILENAME_MAX ];
} iHtiFilename_t;

/* 
** Macro definitions
*/

/* Number of alternate lookup table sets */

#define I_HTI_LOOKUP_SETS    2

iHtiFilename_t iHtiFilename[ I_HTI_LOOKUP_SETS ]
   = { { "/ram/IdTable.idx",
         "/ram/UnitsTable.idx",
         "/ram/FullTable.idx",
         "/ram/StatusTable.idx",
         "/ram/IdTable.idb",
         "/ram/UnitsTable.idb",
         "/ram/FullTable.idb",
         "/ram/StatusTable.idb" },
#ifdef E_WFL_OS_QNX4
       { "/opt/ttl/etc/IdTable.idx",
         "/opt/ttl/etc/UnitsTable.idx",
         "/opt/ttl/etc/FullTable.idx",
         "/opt/ttl/etc/StatusTable.idx",
         "/opt/ttl/etc/IdTable.idb",
         "/opt/ttl/etc/UnitsTable.idb",
         "/opt/ttl/etc/FullTable.idb",
         "/opt/ttl/etc/StatusTable.idb" }
#else
       { "/ttl/sw/etc/IdTable.idx",
         "/ttl/sw/etc/UnitsTable.idx",
         "/ttl/sw/etc/FullTable.idx",
         "/ttl/sw/etc/StatusTable.idx",
         "/ttl/sw/etc/IdTable.idb",
         "/ttl/sw/etc/UnitsTable.idb",
         "/ttl/sw/etc/FullTable.idb",
         "/ttl/sw/etc/StatusTable.idb" }
#endif
     };


/* The Command line options */

#define I_HTI_GENBIN_PACKAGE_SWITCH  "-package"

/* Default CIL ID of SDB for units */

#define I_HTI_CIL_SDB        E_CIL_SDB

/* Maximum number of history records */

#define I_HTI_MAX_BUFFERED_RECORDS   8

/* Number of parameters in ID table string */

#define I_HTI_DESCR_PARAMS   4         /* Including the description */    
#define I_HTI_ENTRY_PARAMS   3         /* Not including the description */

/* Format of line in lookup table */

#define I_HTI_DESCR_STR      "%s %s \"%[^\"]\" \"%[^\"]\""
#define I_HTI_ENTRY_STR      "%s %s \"%[^\"]\" %*s"

/* String for trailing units on SDB unit label names */

#define I_HTI_STR_UNITS      "_UNITS"

/* String for Eol of Oids */
#define I_HTI_OID_EOL_MARKER    "DATAID_EOL"

/* String for Bol of Oids */
#define I_HTI_OID_BOL_MARKER    "DATAID_BOL"

/* Value for an invalid position in a Binary file */
#define I_HTI_INVALID_POSITION -1

/* Value for an invalid count of Oids  in a Bin  file */
#define I_HTI_NO_OIDS 0


/* 
** Function prototypes
*/


#endif

/*
** EOF
*/

