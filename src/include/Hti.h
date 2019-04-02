/*******************************************************************************
** Header File Name:
**    Hti.h
**
** Purpose:
**    Header file for the Header-to-ID conversion library.
**
** Description:
**    Contains the macros, type definitions and function prototypes needed to 
**    use the Header-to-ID conversion library.
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
**    $Id: Hti.h,v 0.14 2005/06/20 13:29:34 mjf Exp $
**
** History:
**    $Log: Hti.h,v $
**    Revision 0.14  2005/06/20 13:29:34  mjf
**    Tidied to aid porting to Linux - no functional changes.
**
**    Revision 0.13  2005/05/13 12:45:18  mjf
**    Consistent with TQB_0_44.
**
**    Revision 0.12  2004/04/06 10:48:25  sxf
**    Ported to QNX v6.
**
**    Revision 0.11  2004/01/15 16:34:21  sxf
**    Corrected enumeration definition.
**
**    Revision 0.10  2003/02/05 11:04:19  dws
**    Added defintion for Bol marker.
**
**    Revision 0.9  2003/01/13 15:09:31  dws
**    Added function prototypes for eHtiCountIds.
**
**    Revision 0.8  2003/01/09 16:03:41  dws
**    Added function definitions for Oid retrieval related functions.
**
**    Revision 0.7  2002/10/21 09:00:29  dws
**    Changed function names for OiD functions to Oid. Fixed
**    #defines  for Status tables from HIT to HTI. moved location
**    of functions to keep them grouped.
**
**    Revision 0.6  2002/10/17 09:37:39  dws
**    Moved internal definitions for default files to external.
**    moved from HtiPrivate.h to Hti.h
**
**    Revision 0.5  2002/10/17 08:41:16  dws
**    added Function prototype for Status label lookup.
**
**    Revision 0.4  2002/10/15 15:58:15  dws
**    Added prototype for status error code comment lookup function
**
**    Revision 0.3  2002/10/09 10:05:19  dws
**    Added new function eHtiGetOiDDescriptionString to get a
**    comment asscoiated with an OiD
**
**    Revision 0.2  2002/04/18 15:22:45  djm
**    Added error codes for the new binary look-up functions.
**    Also moved some private symbolic constants to here for
**    global access. Added the function prototype for the new
**    binary data look-up function.
**
**    Revision 0.1  2001/02/14 13:45:20  mjf
**    Initial revision.
**
**
*******************************************************************************/

#ifndef HTI_H_DEFINED
#define HTI_H_DEFINED


/*
** Returned status values
*/

typedef enum eHtiStatus_t {

   E_HTI_GEN_ERR = STATUS_START( HTI ),/* Miscellaneous error */
   E_HTI_UNRESOLVED,                   /* Unable to resolve label or value */
   E_HTI_FILE_ERROR,                   /* Unable to open lookup table file */
   E_HTI_INDEX_TOO_LOW,                /* Index below expected minimum */
   E_HTI_INDEX_TOO_HIGH,               /* Index above expected maximum */
   E_HTI_BIN_HEADER_FAILURE,           /* Unable to read binary file header */
   E_HTI_BIN_READ_ERROR,               /* Unable to read binary record */
   E_HTI_NO_BIN_MATCH,                 /* Binary look-up failed to find entry */
   E_HTI_OID_LOOKUP_FAIL,              /* Failed to lookup Oids List */
   E_HTI_MALLOC_FAIL,                  /* Failed to allocate memory */
   E_HTI_RECORD_OUTRANGE,              /* Record out of range  */
   E_HTI_INVALID_OID_LIST,             /* List of Oids doesnt have an EOL */
   E_HTI_NO_OIDS,                      /* NO Oids are found for requested Cil */
   E_HTI_EO_ERROR_LIST                 /* Do not use */
} eHtiStatus_e;


/* Maximum length of HTI label/description strings */

#define E_HTI_MAX_STRING_LEN   128
#define E_HTI_MAX_CILSTR_LEN   E_CIL_IDLEN

/* 
** Function prototypes
*/

/* To resolve a numeric identifier from a textual label */

Status_t eHtiGetDataId       ( Int32_t CilId, const char * Label, 
                               Int32_t * Value );
Status_t eHtiGetUnitId       ( const char * Label, Int32_t * Value );
Status_t eHtiGetGeneralId    ( Int32_t CilId, const char * Label, 
                               Int32_t * Value );
Status_t eHtiGetIdentifier   ( const char * Filename, Int32_t CilId, 
                               const char * Label, Int32_t * Value );

/* To resolve a textual label from a numeric identifier */

Status_t eHtiGetDataLabel    ( Int32_t CilId, Int32_t Value, char * Buffer );
Status_t eHtiGetUnitLabel    ( Int32_t Value, char * Buffer );
Status_t eHtiGetGeneralLabel ( Int32_t CilId, Int32_t Value, char * Buffer );
Status_t eHtiGetLabel        ( const char * Filename, Int32_t CilId, 
                               Int32_t Value, char * Buffer );

/* To resolve a label from an identifier, returning number if unresolved */

char *   eHtiGetDataString   ( Int32_t CilId, Int32_t Value, char * Buffer );
char *   eHtiGetUnitString   ( Int32_t Value, char * Buffer );
char *   eHtiGetGeneralString( Int32_t CilId, Int32_t Value, char * Buffer );
char *   eHtiGetString       ( const char * Filename, Int32_t CilId, 
                               Int32_t Value, char * Buffer );
char *   eHtiGetStatusCodeString( Int32_t Value,
                                             char * Buffer); 
/* To look up a description for a specified identifier */

Status_t eHtiGetDescription  ( const char * Filename, Int32_t CilId, 
                               Int32_t Value, char * Buffer );

/* To look up a description, returning an empty string if unresolved */
char *   eHtiGetOidDescriptionString( Int32_t CilId, 
                                   Int32_t Value, char * Buffer );
char *   eHtiGetDescriptionString( const char * Filename, Int32_t CilId, 
                               Int32_t Value, char * Buffer );
char *   eHtiGetStatusCodeDescriptionString( Int32_t Value,
                                             char * Buffer );



/* To count the number of Oids for a specified Cil */
Status_t eHtiCountOids( Int32_t CilId,
                        Int32_t *Count
                       );  
/* To retrieve a list of Oids */
Status_t eHtiGetAllOids(Int32_t CilId,
                       char     ***OidLabelsPtr,
                       Int32_t  **OidValuesPtr, 
                       Int32_t  *Size);

/* To cleanup memory after using the Oid function */
Status_t eHtiCleanupOids( char ***OidLabels,
                          Int32_t **OidValues);
                          

/* To look-up the binary file */

Status_t eHtiBinGetData ( const char  *FileNamePtr, Int32_t CilId,
                          Int32_t Value, char *LabelPtr, char *DescrPtr );

/* To return a pointer to the appropriate filename */

char * eHtiIdTableName       ( void );
char * eHtiUnitTableName     ( void );
char * eHtiFullTableName     ( void );
char * eHtiStatusTableName   ( void );

char * eHtiBinIdTableName    ( void );
char * eHtiBinUnitTableName  ( void );
char * eHtiBinFullTableName  ( void );
char * eHtiBinStatusTableName( void );


#ifdef _MEMCHECK_

void eHtiResetMemCounters( void );
Status_t eHtiPrintMemCheck( void );

#endif

#endif

/*
** EOF
*/

