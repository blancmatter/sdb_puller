/*
** Header File Name:
**    Sfr.h
**
** Purpose:
**    Public header file for the SDB File Recovery (SFR) task
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay
**
** Version:
**    $Id: Sfr.h,v 0.11 2005/06/20 13:42:01 mjf Exp $
**
** History:
**    $Log: Sfr.h,v $
**    Revision 0.11  2005/06/20 13:42:01  mjf
**    Tidied for porting to Linux - no functional changes.
**
**    Revision 0.10  2002/08/16 05:42:40  mjf
**    Define the location of SDB data files in the SFR public header.
**
**    Revision 0.9  2001/10/23 09:23:22  mjf
**    Addition of functionality to retrieve the last 'n' measurements between
**    two time-stamps, in the same way the first 'n' measurements are already
**    retrieved.
**
**    Revision 0.8  2000/09/27 08:37:22  djm
**    Added additional error codes for unit handling.
**
**    Revision 0.7  2000/08/31 09:04:17  djm
**    Added sequence number handling.
**
**    Revision 0.6  2000/08/24 16:16:13  djm
**    Added new error codes and the file block size symbolic constant.
**
**    Revision 0.5  2000/08/22 17:53:05  djm
**    Added error codes and constants.
**
**    Revision 0.4  2000/08/18 18:07:54  djm
**    Added more error codes.
**
**    Revision 0.3  2000/08/17 10:37:11  djm
**    Added more error status codes. Also made global symbolic constants for the
**    command line switchs that are specific to the SFR application. These are
**    used not only here, but also in the SDB from where this application is spawned.
**
**    Revision 0.2  2000/08/15 17:12:53  djm
**    Added more error codes.
**
**    Revision 0.1  2000/08/15 14:31:49  djm
**    Initial creation.
**
**
*/


#ifndef SFR_H_DEFINED
#define SFR_H_DEFINED


/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_SFR_MAIN
   #define E_SFR_EXTERN
   #define E_SFR_INIT(Initialisation) = Initialisation
#else
   #define E_SFR_EXTERN extern
   #define E_SFR_INIT(Initaliasation)
#endif


/* Required include files */

#include "TtlSystem.h"       /* For Status_t & eTtlTime_t definition */
#include "Sdb.h"

/* Enumerate list of status/error values */

typedef enum eSfrStatus_e
{
   E_SFR_GEN_ERR = STATUS_START(SFR),  /* Miscellaneous error */
   E_SFR_CLA_PARSE_ERROR,    /* Unable to parse a command line argument */
   E_SFR_NO_SNEDTO,          /* No datum request specified on command line */
   E_SFR_NO_REQUEST,         /* No datum request specified on command line */
   E_SFR_REQ_PARSE_ERROR,    /* Unable to interpret the request */
   E_SFR_REQ_WRONG_SIZE,     /* Number of chars in req not what was expected */
   E_SFR_OUT_OF_MEMORY,      /* No memory available for dynamic allocation */
   E_SFR_EMPTY_FILELIST,     /* The linked list of SDB files is empty */
   E_SFR_OPENDIR_FAIL,       /* Unable to open the storage directory */
   E_SFR_NO_FILES,           /* There are no SDB files in the storage path */
   E_SFR_NOT_SDB_FILE,       /* File does not have SDB storage file format */
   E_SFR_FILE_OPEN_ERROR,    /* Error attempting to open a file */
   E_SFR_FILE_READ_ERROR,    /* Error attempting to read data from a file */
   E_SFR_FILE_CLOSE_ERROR,   /* Error attempting to close a file */
   E_SFR_EMPTY_LIST,         /* Linked list of filenames unexpectedly empty */
   E_SFR_NO_HEADER,          /* File header missing from SDB storage file */
   E_SFR_NO_FILE_TIME,       /* No SDB file timestamp could be read */
   E_SFR_UNITS_FILE_O_ERR,   /* Error opening units file */
   E_SFR_UNITS_FILE_R_ERR,   /* Error reading data from units file */
   E_SFR_UNITS_FILE_P_ERR,   /* Error parsing data entry in units file */
   E_SFR_NO_UNITS,           /* Unable to determine the units for the data */

   E_SFR_EOERR_LIST          /* End error list marker (DON'T USE FOR STATUS) */
} eSfrStatus_t;


/* Size of the header file to read */

#define E_SFR_FILE_HDR_SIZE  sizeof(E_SDB_HEADER_STRING) - 1
#define E_SFR_FILE_BLK_SIZE  sizeof(eSdbRawFmt_t)


/* Other constants */

#define E_SFR_FILE_DURATION 3600  /* Duration of data in a file (seconds) */


/* Define command line switch text (exlcuding the "-" switch marker) */

#define E_SFR_REQUESTSTR "request"
#define E_SFR_SENDTOSTR  "sendto"
#define E_SFR_SEQNUMSTR  "seqnum"
#define E_SFR_LASTSTR    "lastdata"


/* Path for location of the data-files */

#define E_SFR_DB_FILE_PATH   "//1/opt/ttl/data/"


#endif


/* EOF */
