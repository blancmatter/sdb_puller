/*******************************************************************************
** Module Name:
**    Hti.c
**
** Purpose:
**    Provides common functions for accessing Header-to-ID conversions.
**
** Description:
**    Library of functions for using the Header-to-ID lookups. There is a suite
**    of functions to both resolve textual labels from numerical identifiers, 
**    and vice-versa.
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
**    $Id: Hti.c,v 0.19 2005/06/20 13:29:34 mjf Exp $
**
** History:
**    $Log: Hti.c,v $
**    Revision 0.19  2005/06/20 13:29:34  mjf
**    Tidied to aid porting to Linux - no functional changes.
**
**    Revision 0.18  2005/06/17 09:26:34  mjf
**    Aim to open HTI tables in directory '/ram' and default to '/ttl/sw/etc'
**    if not located there.
**
**    Revision 0.17  2004/01/15 16:34:39  sxf
**    Corrected function prototype definition.
**
**    Revision 0.16  2003/02/05 11:04:19  dws
**    Fixed way in which oids are counted and read now counts from one
**    cilid to the next instead of in between Eol and Bol this to enable
**    all oids from the comms in and reporter process on the nodes to be
**    retrieved.
**
**    Revision 0.15  2003/01/13 15:07:34  dws
**    Broke oid fucntion into seperate smaller functions.
**    made couting of Oids for Cil an external fucntion and creating
**    list of Oid labels and values an external.
**
**    Revision 0.14  2003/01/09 16:05:20  dws
**    Added functions for Oid label and value retrieval.
**
**    Revision 0.13  2002/10/31 15:00:01  dws
**    Fixed eHtiGetOidDescriptionString to use IdTable not the
**    full table
**
**    Revision 0.12  2002/10/21 09:06:40  dws
**    Updated comments in eHtiGetDescriptionString.
**    /
**
**    Revision 0.11  2002/10/21 09:00:29  dws
**    Changed function names for OiD functions to Oid. Fixed
**    #defines  for Status tables from HIT to HTI. moved location
**    of functions to keep them grouped.
**
**    Revision 0.10  2002/10/17 09:35:39  dws
**    Moved internal definitions of default lookup files to
**    Hti.h from HtiPrivate.h
**
**    Revision 0.9  2002/10/17 08:37:49  dws
**    Added function to look up status label. fixed comments for
**    status code description lookup.
**
**    Revision 0.8  2002/10/16 10:16:03  dws
**    Fixed file being opened by eHtiGetStatusCodeDescriptionString
**    to be the binary file rather than text file
**
**    Revision 0.7  2002/10/16 09:46:43  dws
**    First attempt at fixing binary lookup to work with
**    new Erorr code status binary files
**
**    Revision 0.6  2002/10/15 15:47:18  dws
**    Added function to lookup Status code comment from an errorcode
**
**    Revision 0.5  2002/10/09 10:05:19  dws
**    Added new function eHtiGetOiDDescriptionString to get a
**    comment asscoiated with an OiD
**
**    Revision 0.4  2002/04/18 15:22:45  djm
**    Major work to include look-up of the binary index files.
**
**    Revision 0.3  2001/08/09 16:02:21  mjf
**    Fix ensuring that the first entry in the ID table may be retrieved
**    correctly.
**
**    Revision 0.2  2001/06/08 09:22:55  mjf
**    Fix to look up labels correctly when the description string is empty.
**
**    Revision 0.1  2001/02/14 13:45:20  mjf
**    Initial revision.
**
**
*******************************************************************************/


/*
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*
** System include files
*/

#include "TtlSystem.h"
#include "Hti.h"
#include "Log.h"
#include "Cil.h"

/*
** Local include files
*/

#include "HtiPrivate.h"


/*
** Symbolic constants
*/

#define M_HTI_USE_BINARY   /* Define to use the binary index files */

/*
** Type definitions
*/

typedef struct mHtiBinRecord_s
{
   int AccessNum;                      /* Keeps track of latest rec' accessed */
   Int32_t CilId;                      /* Extracted CIL ID from a record */
   Int32_t Value;                      /* Extracted value from a record */
   char CilName[E_HTI_MAX_CILSTR_LEN]; /* Extracted CIL name from a record */
   char Label[E_HTI_MAX_STRING_LEN];   /* Extracted label from a record */
   char Descr[E_HTI_MAX_STRING_LEN];   /* Extracted description from a record */
}
mHtiBinRecord_t;


/*
** Module Variables 
*/

/* The Cilid of the last looked up Oid list for a Cil */
static Int32_t mHtiOidCil = E_CIL_BOL; 
/* The start record of the last looked up Oid list for a Cil  */
static Int32_t mHtiOidStartPos = I_HTI_INVALID_POSITION; 
/* The End record of the last looked up Oid list for a Cil  */
static Int32_t mHtiOidEndPos   = I_HTI_INVALID_POSITION;

/* The count of the last looked up Oid list */
static Int32_t mHtiCountOids   = 0;

/* Sze of the dyanamically allocated arrays for the last looked up Oid list */
static Int32_t mHtiSizeOfOidArrays = 0;

/* To show if memory has been allocated for an Oid list */
static Bool_t  mHtiOidMemAlloc = FALSE;


#ifdef _MEMCHECK_

static Int32_t mHtiNumberMallocs;
static Int32_t mHtiNumberFrees;


void *mHtiMallocWrap(size_t Size);
void mHtiFreeWrap(void *Aptr);


#endif

/*
** Local functions
*/
Status_t mHtiReadRecord   (FILE            *FilePtr,
                           mHtiBinRecord_t *Rec,
                           Int32_t         RecordNumber,
                           Int32_t         RecSize,
                           size_t         HeaderSize);
Status_t mHtiOpenBinFile(FILE    **FilePtr,
                         Int32_t *RecordSize,
                         Int32_t *NumberRecords,
                         size_t  *HeaderSize
                        );
Status_t mHtiFindStartOids(Int32_t CilId);
/*******************************************************************************
** Function Name:
**    eHtiGetDataId
**
** Purpose:
**    Function to resolve a data ID using a supplied CIL ID and label name. 
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the label is resolved correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    Int32_t CilId                    (in)
**       Process for which the supplied label is to be resolved.
**    const char * Label               (in)
**       Label string to resolve.
**    Int32_t * Value                  (out)
**       Resolved value to return.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetDataId       ( Int32_t CilId, const char * Label, 
                               Int32_t * Value )
{

   /* Resolve the label using the default lookup table */
   return eHtiGetIdentifier( eHtiIdTableName(), CilId, Label, Value );

}


/*******************************************************************************
** Function Name:
**    eHtiGetUnitId
**
** Purpose:
**    Function to resolve an SDB unit ID using a supplied label name. 
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the label is resolved correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    const char * Label               (in)
**       Label string to resolve.
**    Int32_t * Value                  (out)
**       Resolved value to return.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetUnitId       ( const char * Label, Int32_t * Value )
{

   /* Resolve the label using the default lookup table */
   return eHtiGetIdentifier( eHtiUnitTableName(), I_HTI_CIL_SDB, Label, Value );

}


/*******************************************************************************
** Function Name:
**    eHtiGetGeneralId
**
** Purpose:
**    Function to resolve a general ID using a supplied CIL ID and label name. 
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the label is resolved correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    Int32_t CilId                    (in)
**       Process for which the supplied label is to be resolved.
**    const char * Label               (in)
**       Label string to resolve.
**    Int32_t * Value                  (out)
**       Resolved value to return.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetGeneralId    ( Int32_t CilId, const char * Label, 
                               Int32_t * Value )
{

   /* Resolve the label using the default lookup table */
   return eHtiGetIdentifier( eHtiFullTableName(), CilId, Label, Value );

}


/*******************************************************************************
** Function Name:
**    eHtiGetIdentifier
**
** Purpose:
**    Function to resolve an ID using a supplied lookup table file, CIL ID and 
**    label name. 
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the label is resolved correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    const char * Filename            (in)
**       Filename to use to attempt to resolve the label.
**    Int32_t CilId                    (in)
**       Process for which the supplied label is to be resolved.
**    const char * Label               (in)
**       Label string to resolve.
**    Int32_t * Value                  (out)
**       Resolved value to return.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetIdentifier   ( const char * Filename, Int32_t CilId, 
                               const char * Label, Int32_t * Value )
{
   int      Count;                     /* Number of strings read from file */
   Bool_t   Matched;                   /* Set TRUE when match found in file */
   char     LineBuffer                 /* Buffer for line read from ID file */
               [ E_HTI_MAX_STRING_LEN ];
   char     ProcessStr                 /* String containing process */
               [ E_HTI_MAX_STRING_LEN ];
   char     ValueStr                   /* String containing value */
               [ E_HTI_MAX_STRING_LEN ];
   char     LabelStr                   /* String containing label */
               [ E_HTI_MAX_STRING_LEN ];
   FILE   * FileId;                    /* Pointer to file */

   /* Initial conditions */
   Matched = FALSE;

   /* Attempt to open the file */
   FileId = fopen( Filename, "r" );

   /* If file not opened successfully */
   if ( FileId == NULL )
   {
      /* return a file error */
      return E_HTI_FILE_ERROR;
   }

   /* Read lines looking for a match, until one found or EOF */
   do
   {
      /* Read in a line from the file */
      if ( fgets( LineBuffer, sizeof( LineBuffer ), FileId ) != NULL )
      {
         /* Line read in OK, convert arguments */
         Count = sscanf( LineBuffer, I_HTI_ENTRY_STR, 
                         ProcessStr, ValueStr, LabelStr );

         /* Check for required number of strings found */
         if ( Count == I_HTI_ENTRY_PARAMS )
         {
            /* If process and string name match */
            if ( ( strcmp( ProcessStr, eCilNameString( CilId ) ) == 0 ) 
                  && ( strcmp( LabelStr, Label ) == 0 ) )
            {
               /* Convert the associated value */
               *Value = strtol( ValueStr, NULL, 0 );

               /* Set flag to indicate label matched */
               Matched = TRUE;
            }
         }
      }
      /* Else line not read correctly */
      else
      {
         /* Set count to EOF to exit loop */
         Count = EOF;
      }
   }
   while ( ( Matched == FALSE ) && ( Count != EOF ) );
 
   /* Close the file */
   fclose ( FileId );

   /* Return the appropriate status */
   if ( Matched == FALSE )
   {
      return E_HTI_UNRESOLVED;
   }
   else
   {
      return SYS_NOMINAL;
   }

}


/*******************************************************************************
** Function Name:
**    eHtiGetDataLabel
**
** Purpose:
**    Function to retrieve a textual data ID label using a supplied CIL ID, 
**    value to be used, and a buffer to write the returned label into.
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the label is returned correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    Int32_t CilId                    (in)
**       Process for which the label is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned label. If the label is unresolved then this 
**       buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetDataLabel    ( Int32_t CilId, Int32_t Value, char * Buffer )
{

   /* Lookup the text using the default lookup table */
#ifdef M_HTI_USE_BINARY
   return eHtiBinGetData( eHtiBinIdTableName(), CilId, Value, Buffer, NULL );
#else
   return eHtiGetLabel( eHtiIdTableName(), CilId, Value, Buffer );
#endif

}


/*******************************************************************************
** Function Name:
**    eHtiGetUnitLabel
**
** Purpose:
**    Function to retrieve a textual SDB unit label using the SDB unit value
**    and a buffer to write the returned label into.
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the label is returned correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned label. If the label is unresolved then this 
**       buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetUnitLabel    ( Int32_t Value, char * Buffer )
{

   /* Lookup the text using the default lookup table */
#ifdef M_HTI_USE_BINARY
   return eHtiBinGetData( eHtiBinUnitTableName(), I_HTI_CIL_SDB,
                          Value, Buffer, NULL );
#else
   return eHtiGetLabel( eHtiUnitTableName(), I_HTI_CIL_SDB, Value, Buffer );
#endif
}


/*******************************************************************************
** Function Name:
**    eHtiGetGeneralLabel
**
** Purpose:
**    Function to retrieve a textual data ID label using a supplied CIL ID, 
**    value to be used, and a buffer to write the returned label into.
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the label is returned correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    Int32_t CilId                    (in)
**       Process for which the label is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned label. If the label is unresolved then this 
**       buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetGeneralLabel ( Int32_t CilId, Int32_t Value, char * Buffer )
{

   /* Lookup the text using the default lookup table */
#ifdef M_HTI_USE_BINARY
   return eHtiBinGetData( eHtiBinFullTableName(), CilId, Value, Buffer, NULL );
#else
   return eHtiGetLabel( eHtiFullTableName(), CilId, Value, Buffer );
#endif

}


/*******************************************************************************
** Function Name:
**    eHtiGetLabel
**
** Purpose:
**    Function to retrieve a textual label using a supplied lookup table file, 
**    a supplied CIL ID, the value to be used, and a buffer to write the 
**    returned label into.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the label is returned correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    const char * Filename            (in)
**       Filename to use to attempt to resolve the label.
**    Int32_t CilId                    (in)
**       Process for which the label is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned label. If the label is unresolved then this 
**       buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetLabel        ( const char * Filename, Int32_t CilId, 
                               Int32_t Value, char * Buffer )
{
   Int32_t  ValueRead;                 /* Value read from the file */
   int      Count;                     /* Number of strings read from file */
   Bool_t   Matched;                   /* Set TRUE when match found in file */
   char     LineBuffer                 /* Buffer for line read from ID file */
               [ E_HTI_MAX_STRING_LEN ];
   char     ProcessStr                 /* String containing process */
               [ E_HTI_MAX_STRING_LEN ];
   char     ValueStr                   /* String containing value */
               [ E_HTI_MAX_STRING_LEN ];
   char     LabelStr                   /* String containing label */
               [ E_HTI_MAX_STRING_LEN ];
   FILE   * FileId;                    /* Pointer to file */

   /* Initial conditions */
   Matched = FALSE;

   /* Clear the supplied buffer */
   strcpy( Buffer, "\0" );

   /* Attempt to open the file */
   FileId = fopen( Filename, "r" );

   /* If file not opened successfully */
   if ( FileId == NULL )
   {
      /* return a file error */
      return E_HTI_FILE_ERROR;
   }

   /* Read lines looking for a match, until one found or EOF */
   do
   {
      /* Read in a line from the file */
      if ( fgets( LineBuffer, sizeof( LineBuffer ), FileId ) != NULL )
      {
         /* Line read in OK, convert arguments */
         Count = sscanf( LineBuffer, I_HTI_ENTRY_STR, 
                         ProcessStr, ValueStr, LabelStr );

         /* Check for required number of strings found */
         if ( Count == I_HTI_ENTRY_PARAMS )
         {
            /* Convert the value read from the file */
            ValueRead = strtol( ValueStr, NULL, 0 );

            /* If process and value match */
            if ( ( strcmp( ProcessStr, eCilNameString( CilId ) ) == 0 ) 
                  && ( ValueRead == Value ) )
            {
               /* Take copy of the label */
               strcpy( Buffer, LabelStr );

               /* Set flag to indicate label matched */
               Matched = TRUE;
            }
         }
      }
      /* Else line not read correctly */
      else
      {
         /* Set count to EOF to exit loop */
         Count = EOF;
      }
   }
   while ( ( Matched == FALSE ) && ( Count != EOF ) );
 
   /* Close the file */
   fclose ( FileId );

   /* Return the appropriate status */
   if ( Matched == FALSE )
   {
      return E_HTI_UNRESOLVED;
   }
   else
   {
      return SYS_NOMINAL;
   }

}


/*******************************************************************************
** Function Name:
**    eHtiGetDataString
**
** Purpose:
**    Function to retrieve a textual data ID label using a supplied CIL ID, 
**    value to be used, and a buffer to write the returned label into. If the 
**    label is unresolved then the number itself is written into the buffer.
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    char *
**       Returns a pointer to the supplied buffer.
**
** Arguments:
**    Int32_t CilId                    (in)
**       Process for which the label is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned label. If the label is unresolved then the  
**       number itself is written into the buffer.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

char *   eHtiGetDataString   ( Int32_t CilId, Int32_t Value, char * Buffer )
{

   /* Lookup the text using the default lookup table */
#ifdef M_HTI_USE_BINARY
   eHtiBinGetData(eHtiBinIdTableName(), CilId, Value, Buffer, NULL);
   return Buffer;
#else
   return eHtiGetString( eHtiIdTableName(), CilId, Value, Buffer );
#endif

}


/*******************************************************************************
** Function Name:
**    eHtiGetUnitString
**
** Purpose:
**    Function to retrieve a textual SDB unit label using the SDB unit value
**    and a buffer to write the returned label into. If the label is unresolved
**    then the number itself is written into the buffer.
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    char *
**       Returns a pointer to the supplied buffer.
**
** Arguments:
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned label. If the label is unresolved then the 
**       number itself is written into the buffer.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

char *   eHtiGetUnitString   ( Int32_t Value, char * Buffer )
{

   /* Lookup the text using the default lookup table */
#ifdef M_HTI_USE_BINARY
   eHtiBinGetData(eHtiBinUnitTableName(), I_HTI_CIL_SDB, Value, Buffer, NULL);
   return Buffer;
#else
   return eHtiGetString( eHtiUnitTableName(), I_HTI_CIL_SDB, Value, Buffer );
#endif

}


/*******************************************************************************
** Function Name:
**    eHtiGetGeneralString
**
** Purpose:
**    Function to retrieve a textual data ID label using a supplied CIL ID, 
**    value to be used, and a buffer to write the returned label into. If the 
**    label is unresolved then the number itself is written into the buffer.
**    This function uses the default lookup table file.
**
** Description:
**    ...
**
** Return Type:
**    char *
**       Returns a pointer to the supplied buffer.
**
** Arguments:
**    Int32_t CilId                    (in)
**       Process for which the label is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned label. If the label is unresolved then the 
**       number itself is written into the buffer.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

char *   eHtiGetGeneralString( Int32_t CilId, Int32_t Value, char * Buffer )
{

   /* Lookup the text using the default lookup table */
#ifdef M_HTI_USE_BINARY
   eHtiBinGetData(eHtiBinFullTableName(), CilId, Value, Buffer, NULL);
   return Buffer;
#else
   return eHtiGetString( eHtiFullTableName(), CilId, Value, Buffer );
#endif


}
/*******************************************************************************
** Function Name:
**    eHtiGetStatusCodeString
**
** Purpose:
**    Function to retrieve a textual label for a Status code.   using a supplied 
**    status code to lookup, and a buffer to write the  returned label into.
**
** Description:
**    ...
**
** Return Type:
**    char *
**       Returns a pointer to the supplied buffer.
**
** Arguments:
**    Int32_t Value                    (in)
**       Status code to look up.
**    char * Buffer                    (out)
**       Buffer holding returned Label. If the label is unresolved 
**       then this buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Daniel W. Smith (dws)
**
*******************************************************************************/

char *   eHtiGetStatusCodeString( Int32_t Value,
                                             char * Buffer )
{
   Int32_t Package;  /* The package Id that the error code belongs to */ 
   Status_t Status;

   Package = STATUS_PACKAGE(Value); /* Convert the Error code to a package code
                                       this is the 'CilId' for the bin lookup 
                                       function */
   
   /* Look up the description */
   Status = eHtiBinGetData( eHtiBinStatusTableName(), Package, Value, Buffer, NULL );


   /* Return pointer to the supplied buffer */
   return Buffer;
}
/*******************************************************************************
** Function Name:
**    eHtiGetString
**
** Purpose:
**    Function to retrieve a textual label using a supplied lookup table file, 
**    a supplied CIL ID, the value to be used, and a buffer to write the 
**    returned label into. If the label is unresolved then the number itself is 
**    written into the buffer.
**
** Description:
**    ...
**
** Return Type:
**    char *
**       Returns a pointer to the supplied buffer.
**
** Arguments:
**    const char * Filename            (in)
**       Filename to use to attempt to resolve the label.
**    Int32_t CilId                    (in)
**       Process for which the label is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned label. If the label is unresolved then the 
**       number itself is written into the buffer.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

char *   eHtiGetString       ( const char * Filename, Int32_t CilId, 
                               Int32_t Value, char * Buffer )
{
   Status_t Status;

   /* If attempt to look up the label is unsuccessful */
#ifdef M_HTI_USE_BINARY
   Status = eHtiBinGetData( Filename, CilId, Value, Buffer, NULL );
#else
   Status = eHtiGetLabel( Filename, CilId, Value, Buffer );
#endif
   if ( Status != SYS_NOMINAL )
   {
      /* Write the number into the buffer, in hex */
      sprintf( Buffer, "%4.4x (%x)", Value, Status );
   }

   /* If value matched for SDB, attempt to remove trailing _UNITS */
   if ( ( CilId == I_HTI_CIL_SDB ) 
         && ( strstr( Buffer, I_HTI_STR_UNITS ) != NULL ) )
   {
      *( strstr( Buffer, I_HTI_STR_UNITS ) ) = '\0';
   }

   /* Return pointer to the supplied buffer */
   return Buffer;

}


/*******************************************************************************
** Function Name:
**    eHtiGetDescription
**
** Purpose:
**    Function to retrieve a textual description using a supplied lookup table 
**    file, a supplied CIL ID, the value to be used, and a buffer to write the 
**    returned description into.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns SYS_NOMINAL if the description is returned correctly, else 
**       returns one of the errors in the public header file.
**
** Arguments:
**    const char * Filename            (in)
**       Filename to use to attempt to resolve the description.
**    Int32_t CilId                    (in)
**       Process for which the description is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned description. If the description is unresolved 
**       then this buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eHtiGetDescription  ( const char * Filename, Int32_t CilId, 
                               Int32_t Value, char * Buffer )
{
   Int32_t  ValueRead;                 /* Value read from the file */
   int      Count;                     /* Number of strings read from file */
   Bool_t   Matched;                   /* Set TRUE when match found in file */
   char     LineBuffer                 /* Buffer for line read from ID file */
               [ E_HTI_MAX_STRING_LEN ];
   char     ProcessStr                 /* String containing process */
               [ E_HTI_MAX_STRING_LEN ];
   char     ValueStr                   /* String containing value */
               [ E_HTI_MAX_STRING_LEN ];
   char     LabelStr                   /* String containing label */
               [ E_HTI_MAX_STRING_LEN ];
   char     DescriptionStr             /* String containing description */
               [ E_HTI_MAX_STRING_LEN ];
   FILE   * FileId;                    /* Pointer to file */

   /* Initial conditions */
   Matched = FALSE;

   /* Clear the supplied buffer */
   strcpy( Buffer, "\0" );

   /* Attempt to open the file */
   FileId = fopen( Filename, "r" );

   /* If file not opened successfully */
   if ( FileId == NULL )
   {
      /* return a file error */
      return E_HTI_FILE_ERROR;
   }

   /* Read lines looking for a match, until one found or EOF */
   do
   {
      /* Read in a line from the file */
      if ( fgets( LineBuffer, sizeof( LineBuffer ), FileId ) != NULL )
      {
         /* Line read in OK, convert arguments */
         Count = sscanf( LineBuffer, I_HTI_DESCR_STR, 
                         ProcessStr, ValueStr, LabelStr, DescriptionStr );

         /* Check for required number of strings found */
         if ( Count == I_HTI_DESCR_PARAMS )
         {
            /* Convert the value read from the file */
            ValueRead = strtol( ValueStr, NULL, 0 );

            /* If process and value match */
            if ( ( strcmp( ProcessStr, eCilNameString( CilId ) ) == 0 ) 
                  && ( ValueRead == Value ) )
            {
               /* Take copy of the description */
               strcpy( Buffer, DescriptionStr );

               /* Set flag to indicate label matched */
               Matched = TRUE;
            }
         }
      }
      /* Else line not read correctly */
      else
      {
         /* Set count to EOF to exit loop */
         Count = EOF;
      }
   }
   while ( ( Matched == FALSE ) && ( Count != EOF ) );
 
   /* Close the file */
   fclose ( FileId );

   /* Return the appropriate status */
   if ( Matched == FALSE )
   {
      return E_HTI_UNRESOLVED;
   }
   else
   {
      return SYS_NOMINAL;
   }

}

/*******************************************************************************
** Function Name:
**    eHtiGetStatusCodeDescriptionString
**
** Purpose:
**    Function to retrieve a textual description for a Status code   using a supplied 
**    Status code and a buffer to write the  returned description into.
**
** Description:
**    ...
**
** Return Type:
**    char *
**       Returns a pointer to the supplied buffer.
**
** Arguments:
**    Int32_t Value                    (in)
**       Status code to look up.
**    char * Buffer                    (out)
**       Buffer holding returned description. If the description is unresolved 
**       then this buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Daniel W. Smith (dws)
**
*******************************************************************************/

char *   eHtiGetStatusCodeDescriptionString( Int32_t Value,
                                             char * Buffer )
{
   Int32_t Package;  /* The package Id that the error code belongs to */ 
   Status_t Status;

   Package = STATUS_PACKAGE(Value); /* Convert the Error code to a package code
                                       this is the 'CilId' for the bin lookup 
                                       function */
   
   /* Look up the description */
   Status = eHtiBinGetData( eHtiBinStatusTableName(), Package, Value, NULL, Buffer );


   /* Return pointer to the supplied buffer */
   return Buffer;

}

/*******************************************************************************
** Function Name:
**    eHtiGetOiDDescriptionString
**
** Purpose:
**    Function to retrieve a textual description for a Oid   using a supplied 
**    CIL ID, the value to be used, and a buffer to write the  returned d
**    description into.
**
** Description:
**    ...
**
** Return Type:
**    char *
**       Returns a pointer to the supplied buffer.
**
** Arguments:
**    Int32_t CilId                    (in)
**       Process for which the description is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned description. If the description is unresolved 
**       then this buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Daniel W. Smith (dws)
**
*******************************************************************************/

char *   eHtiGetOidDescriptionString( Int32_t CilId, 
                                   Int32_t Value, char * Buffer )
{

   /* Look up the description */
#ifdef M_HTI_USE_BINARY
   eHtiBinGetData( eHtiBinIdTableName(), CilId, Value, NULL, Buffer );
#else
   eHtiGetDescription( eHtiIdTableName(), CilId, Value, Buffer );
#endif

   /* Return pointer to the supplied buffer */
   return Buffer;

}

/*******************************************************************************
** Function Name:
**    eHtiGetDescriptionString
**
** Purpose:
**    Function to retrieve a textual description using a supplied lookup table 
**    file, a supplied CIL ID, the value to be used, and a buffer to write the 
**    returned description into.
**
** Description:
**    ...
**
** Return Type:
**    char *
**       Returns a pointer to the supplied buffer.
**
** Arguments:
**    const char * Filename            (in)
**       Filename to use to attempt to resolve the description.
**    Int32_t CilId                    (in)
**       Process for which the description is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char * Buffer                    (out)
**       Buffer holding returned description. If the description is unresolved 
**       then this buffer is returned empty.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

char *   eHtiGetDescriptionString( const char * Filename, Int32_t CilId, 
                                   Int32_t Value, char * Buffer )
{

/*******************************************************************
** This function must not be Called with a Textfile as filename
** if this was to happen the lookup would fail as only the binary
** lookup is used and if a text file was passed the binary lookup
** would fail.
*******************************************************************/
   /* Look up the description */
#ifdef M_HTI_USE_BINARY
   eHtiBinGetData( Filename, CilId, Value, NULL, Buffer );
#else
   eHtiGetDescription( Filename, CilId, Value, Buffer );
#endif

   /* Return pointer to the supplied buffer */
   return Buffer;

}




/*******************************************************************************
** Function Name:
**    eHtiBinGetData
**
** Purpose:
**    Binary file lookup function.
**
** Description:
**    Function to retrieve a textual label and/or description using a supplied
**    lookup table file, a supplied CIL ID, the value to be used, and buffers
**    into which to write the returned data.
**
** Return Type:
**    Status_t
**       Returns SYS_NOMINAL if the label is returned correctly, else returns
**       one of the errors in the public header file.
**
** Arguments:
**    const char *FileNamePtr          (in)
**       A pointer to a "constant" ASCII string containing the filename
**       of the file to use to attempt to resolve the label.
**    Int32_t CilId                    (in)
**       Process for which the label is to be returned.
**    Int32_t Value                    (in)
**       Value to look up.
**    char *LabelPtr                  (in/out)
**       Buffer holding returned label. If the label is unresolved then this
**       buffer is returned empty. If this pointer is set to NULL, then no
**       look-up is attempted.
**    char *DescrPtr                  (in/out)
**       Buffer holding returned description. If the description is
**       unresolved then this buffer is returned empty. If this pointer is
**       set to NULL, then no look-up is attempted.
**
** Author(s):
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t eHtiBinGetData (
   const char  *FileNamePtr,
   Int32_t     CilId,
   Int32_t     Value,
   char        *LabelPtr,
   char        *DescrPtr
)
{
   /* Local variables */
   FILE *FilePtr;            /* Pointer to the opened file */
   Int32_t HeaderBuffer[2];  /* Buffer for the two elements of the header */
   Int32_t NumRecs;          /* Number of records in the file */
   Int32_t RecSize;          /* Size (in bytes) of a record from the file */
   size_t  NumElts;          /* The number of elements read with fread() */
   Int32_t JumpSize;         /* This is the number of records to jump */
   int Direction;            /* Direction to jump within the file */
   char Record[E_HTI_MAX_STRING_LEN];/* Buffer in which to hold a record */
   size_t Pos;               /* Position within a single record of values */
   int SingleIncrCtr;        /* Single increment counter */
   static mHtiBinRecord_t Rec[I_HTI_MAX_BUFFERED_RECORDS];  /* History */
   int RecIdx = -1;          /* Record index */
   int RecCtr;               /* Record loop counter */
   int MaxAccessNum;         /* Oldest record that has been accessed */
   static Bool_t FirstTime = TRUE;   /* FOr initialisation */


   /*
   ** Initialisation
   */

   /* Initialisation (reset the history buffer on first call */
   if(FirstTime == TRUE)
   {
/*
printf("\n\nBINARY LOOK-UP\n\n\n");
*/
      for(RecCtr = 0; RecCtr < I_HTI_MAX_BUFFERED_RECORDS; RecCtr++)
      {
         Rec[RecCtr].AccessNum = 1;
         Rec[RecCtr].CilId = -1;
         Rec[RecCtr].Value = -1;
         Rec[RecCtr].CilName[0] = '\0';
         Rec[RecCtr].Label[0] = '\0';
         Rec[RecCtr].Descr[0] = '\0';
      }
      FirstTime = FALSE;
   }


   /* Clear the supplied bufffers (in case of premature return) */
   if(LabelPtr != NULL) *LabelPtr = '\0';
   if(DescrPtr != NULL) *DescrPtr = '\0';


   /*
   ** We may have recently read this entry from the table. We now go
   ** through our history buffer to see if the current request is there.
   ** This will save us needing to access the file (and hence save a lot
   ** of time).
   */


   /* Reset the record index */
   RecIdx = -1;

   /* Loop over all the existing history entries */
   for(RecCtr = 0; RecCtr < I_HTI_MAX_BUFFERED_RECORDS; RecCtr++)
   {
      /* Increment the record's access number */
      Rec[RecCtr].AccessNum += 1;

      /* Check for a match between this record and the request */
      if( (CilId == Rec[RecCtr].CilId) && (Value == Rec[RecCtr].Value) )
      {
         /* If a label is requested, then enter it into the argument buffer */
         if(LabelPtr != NULL)
         {
            strcpy(LabelPtr, Rec[RecCtr].Label);
         }

         /* Do the same for the description */
         if(DescrPtr != NULL)
         {
            strcpy(DescrPtr, Rec[RecCtr].Descr);
         }

         /* Make a note of the record number of this match */
         RecIdx = RecCtr;

         /* Set the access number to zero - i.e. this is the most recent one */
         Rec[RecCtr].AccessNum = 0;
      }

      /* Clip the Access number to prevent integer-wrapping causing problems */
      if(Rec[RecCtr].AccessNum > I_HTI_MAX_BUFFERED_RECORDS)
      {
         Rec[RecCtr].AccessNum = I_HTI_MAX_BUFFERED_RECORDS;
      }
   }

   /* If we found a match, then return */
   if(RecIdx != -1) return SYS_NOMINAL;


   /*
   ** If we get to this point, then no record with matching CIL and Value
   ** arguments has been found in the history. We will later look up the
   ** data from the binary record file. This data will replace the oldest
   ** record in our history buffer. This next section determines the one
   ** (well, the index thereof) that needs to be replaced.
   */

   /* Reset the maximum (i.e. oldest) access number */
   MaxAccessNum = -1;

   /* Loop through the history, and get the index of the oldest record there */
   for(RecCtr = 0; RecCtr < I_HTI_MAX_BUFFERED_RECORDS; RecCtr++)
   {
      if(Rec[RecCtr].AccessNum > MaxAccessNum)
      {
         RecIdx = RecCtr;
         MaxAccessNum = Rec[RecCtr].AccessNum;
      }
   }

   /* Check we have a valid record to modify */
   if(RecIdx < 0) return E_HTI_INDEX_TOO_LOW;
   if(RecIdx >= I_HTI_MAX_BUFFERED_RECORDS) return E_HTI_INDEX_TOO_HIGH;


   /* Reset the access number of the one we end up using */
   Rec[RecIdx].AccessNum = 0;


   /*
   ** The index of the  oldest record in the history has been noted. This
   ** record will be replaced when we look up the file (which we must do
   ** as the requested entry was not in our history buffer).
   */


   /* Open the file */
   FilePtr = fopen( FileNamePtr, "rb");
   if(FilePtr == NULL) return E_HTI_FILE_ERROR;

   /* Read in the header to determine the number and size of the records */
   NumElts = fread(HeaderBuffer, sizeof(Int32_t), 2, FilePtr);
   if(NumElts != 2)
   {
      fclose(FilePtr);
      return E_HTI_BIN_HEADER_FAILURE;
   }
   NumRecs = HeaderBuffer[0];
   RecSize = HeaderBuffer[1];


   /* Determine the "jump" size, for "binary-searching" the file */
   JumpSize = NumRecs / 2;
   Direction = 1;

   /* Limit the number of searches that may be made with this loop */
   for(SingleIncrCtr = 0; SingleIncrCtr < 15; /* No default increment */ )
   {

      /* Jump to a record */
      fseek(FilePtr, RecSize*(JumpSize*Direction - 1), SEEK_CUR);

      /* Read the record */
      NumElts = fread(Record, RecSize, 1, FilePtr);
      if(NumElts != 1)
      {
         fclose(FilePtr);
         return E_HTI_BIN_READ_ERROR;
      }

      /* Extract the components */
      Pos = 0;
      memcpy((void *)&Rec[RecIdx].CilId,
             (void *)&Record[Pos], sizeof(Rec[RecIdx].CilId));
      Pos += sizeof(Rec[RecIdx].CilId);
      memcpy((void *)&Rec[RecIdx].Value,
             (void *)&Record[Pos], sizeof(Rec[RecIdx].Value));
      Pos += sizeof(Rec[RecIdx].Value);
      strcpy((void *)&Rec[RecIdx].CilName, (void *)&Record[Pos]);
      Pos += strlen(Rec[RecIdx].CilName) + 1;
      strcpy((void *)&Rec[RecIdx].Label, (void *)&Record[Pos]);
      Pos += strlen(Rec[RecIdx].Label) + 1;
      strcpy((void *)&Rec[RecIdx].Descr, (void *)&Record[Pos]);

      /* Check if it matches, return if it does! */
      if( (CilId == Rec[RecIdx].CilId) && (Value == Rec[RecIdx].Value) )
      {
         /* If a label-buffer is supplied, copy in the relevant field */
         if(LabelPtr != NULL)
         {
            strcpy(LabelPtr, Rec[RecIdx].Label);
         }
         /* If a description-buffer is supplied, copy in the relevant field */
         if(DescrPtr != NULL)
         {
            strcpy(DescrPtr, Rec[RecIdx].Descr);
         }
         /* Close the file, and return success */
         fclose(FilePtr);
         return SYS_NOMINAL;
      }


      /* Determine which way to jump */
      if(CilId > Rec[RecIdx].CilId)
      {
         Direction = 1;
      }
      else if (CilId < Rec[RecIdx].CilId)
      {
         Direction = -1;
      }
      else
      {
         if(Value > Rec[RecIdx].Value)
         {
            Direction = 1;
         }
         else if (Value < Rec[RecIdx].Value)
         {
            Direction = -1;
         }
         else
         {
            printf("Search error\n");
            break;
         }
      }

      /* Determine the new jump size */
      JumpSize = JumpSize / 2;
      if(JumpSize < 1)
      {
         JumpSize = 1;
         SingleIncrCtr++;
      }

   }  /* End of for-loop */

   /* Close the file */
   fclose(FilePtr);

   /* If we get this far, it was unsuccessful - return an error code */
   return E_HTI_NO_BIN_MATCH;

}  /* End of eHtiBinGetData() */
/*******************************************************************************
** Function Name: 
**    eHtiGetAllOids
** 
** 
** Purpose:
**    To read in all Oid labels and values for a specified cil into to 2 
**    specifed arrays.
** 
** 
** 
** Description: 
**
**    Function to retrieve the textual labels and Oid values for a specified
**    CildId and stores these in the 2 given buffers these buffers are cleared
**    and allocated in this function the new size of the buffers is returned 
**    as well.
** 
** Return Type:
**    Status_t
**      Returns SYS_NOMINAL if a success and given arrays will be filled with 
**      the specified Cil' Oids values and labels and *size will contain the
**      size of the new array.
** 
** Arguements:
**    Int32_t CilId,
**       CilId to retrieve the Oid details for.
**
**    char     ***OidLabelsPtr
**       A pointer to a pointer to a pointer to store the Oid labels in. 
**       Effectively this becomes a pointer to array of unallocated char *.         
** 
**    Int32_t  **OidValuesPtr, 
**       A pointer to a pointer to store the Oid values in. Effectively this 
**       is a dynamically sizeable array of Int32_t's.
**
**    Int32_t  *Size
**       A pointer to an Int32_t to store the Size of the 2 arrays in.
** 
** 
** 
** Author(s):
**    Daniel W. Smith
** 
*******************************************************************************/
Status_t eHtiGetAllOids(Int32_t CilId,
                       char     ***OidLabelsPtr,
                       Int32_t  **OidValuesPtr, 
                       Int32_t  *Size)
{
   /* Local variables */
   
   Status_t Status = SYS_NOMINAL;
   FILE    *OidFilePtr;         /* Pointer to the opened file */
   Int32_t RecordNumber = 0; /* Which Record currently being looked at */
   mHtiBinRecord_t Rec;      /* struture to store retrieved record */
   Int32_t NumRecs = 0;      /* Number of records in the file */
   Int32_t RecSize = 0;      /* Size (in bytes) of a record from the file */
   Int32_t Position = 0;     /* The current position in the two arrays to 
                                store label and value */
   size_t HeaderSize;       /* The size of the file Header */
   char    **LocalOidLabelsPtr; /*Local Pointer to store read in Oid Labels. */
   Int32_t *LocalOidsValuesPtr; /* Local Pointer to read in Values */
   Int32_t Count;            /* Count of Oids for specified Cil */



   /* 
   ** If the two arrays of labels and Values has been succesfully created 
   ** we need to free the previous arrays.
   */
   if( mHtiOidMemAlloc == TRUE)
   {
     
      /* Clean up previously allocated memory */
      eHtiCleanupOids(OidLabelsPtr, OidValuesPtr);
   }


   /* check if Oids need to be counted */
   if((mHtiOidEndPos == I_HTI_INVALID_POSITION) ||( CilId != mHtiOidCil) )
   {

      

      Status = eHtiCountOids(CilId, &Count);
      if(Status != SYS_NOMINAL)
      {
         eLogCrit(Status,"Failed To count number of Oids for specified Cil");
         /* Set the count of Oids to 0 */
         *Size = 0;
         mHtiSizeOfOidArrays = 0;

         /* Show that memory hasnt been allocated */
         mHtiOidMemAlloc = FALSE;

         return(Status);
      }


   }


   
  
   
   /* Open the Bin Oid table file and read header */
   Status = mHtiOpenBinFile(     
                         &OidFilePtr,
                         &RecSize,
                         &NumRecs,
                         &HeaderSize
                        );
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status,"Failed to open file");
      
      /* Set the count of Oids to 0 */
      *Size = 0;
      mHtiSizeOfOidArrays = 0;

      /* Show that memory hasnt been allocated */
      mHtiOidMemAlloc = FALSE;
      return( Status ); 
   }
 



   eLogDebug("Allocating memory for OID values");  
              
   /* Allocate memory to store all the Oid values */

#ifdef _MEMCHECK_
   LocalOidsValuesPtr =(Int32_t *)mHtiMallocWrap(sizeof(Int32_t) *
                                            (mHtiCountOids) ); 
#else 
   LocalOidsValuesPtr =(Int32_t *)TTL_MALLOC(sizeof(Int32_t) *
                                            (mHtiCountOids) );
#endif

   if (LocalOidsValuesPtr == NULL)
   {

#ifdef _MEMCHECK_
   mHtiNumberMallocs--;
#endif

      Status = E_HTI_MALLOC_FAIL;
      
      /* Log the Status */
      eLogCrit(Status, "Malloc Fail, Fatal Error,"
                       "mCurrentOidsPtr ");
         
      /* Close the file */
      fclose(OidFilePtr);

      /* Set the count of Oids to 0 */
      *Size = 0;
      mHtiSizeOfOidArrays = 0;

      /* Show that memory hasnt been allocated */
      mHtiOidMemAlloc = FALSE;
      return( Status );
   } /* End if malloc failure. */  

   

   eLogDebug("Allocating memory for OID Labels"); 
         
   /* Allocate memory to store all the Oid Labels */ 
#ifdef _MEMCHECK_
   LocalOidLabelsPtr = (void *)mHtiMallocWrap(sizeof(void *) * 
                                         (mHtiCountOids) );
#else
   LocalOidLabelsPtr = (void *)TTL_MALLOC(sizeof(void *) * 
                                         (mHtiCountOids) );
#endif

   if (LocalOidLabelsPtr == NULL)
   {
      Status = E_HTI_MALLOC_FAIL;
         
#ifdef _MEMCHECK_
   mHtiNumberMallocs--;
#endif
  
      /* Log the Status */
      eLogCrit(Status, "Malloc Fail, Fatal Error,"
                       "mCurrentOidLblsPtr ");
         
      /* Close the file */
      fclose(OidFilePtr);

      /* Clean up Memory allocated to  Oids */
      TTL_FREE(LocalOidLabelsPtr);
            
      /* Set the count of Oids to 0 */
      *Size = 0;
      mHtiSizeOfOidArrays = 0;

      /* Show that memory hasnt been allocated */
      mHtiOidMemAlloc = FALSE;
      return( Status );
   } /* End if malloc failure. */

   
   /* Start from the beginning of the specified Cil's Oid's */
   RecordNumber = mHtiOidStartPos;
   
   /* To show that we have currently copied nothing into the array */
   mHtiSizeOfOidArrays = 1;

   /* Read all the Oid data for the specified Cil */
   while(!feof(OidFilePtr))
   {
      /* 
      ** If an attempt was made to read past the last record and the
      ** requested Cil Hasnt been found close the file and return.
      */ 
      if(RecordNumber >= NumRecs)
      {
           /* Close the file */
           fclose(OidFilePtr);
           eLogCrit(E_HTI_OID_LOOKUP_FAIL,"Failed to Count Oids before reaching"
                    "end of records");

           /* Clean up Memory allocated to  Oids */
           eHtiCleanupOids(&LocalOidLabelsPtr,&LocalOidsValuesPtr);
           
           /* Set the count of Oids to 0 */
           *Size = 0;
           mHtiSizeOfOidArrays = 0;

           /* Show that memory hasnt been allocated */
           mHtiOidMemAlloc = FALSE;

           return E_HTI_OID_LOOKUP_FAIL;
      }

      if((RecordNumber >= mHtiOidStartPos ) || (RecordNumber <= mHtiOidEndPos))
      {
         
         /* Read record from Oid table */
         Status = mHtiReadRecord(
                        OidFilePtr,
                        &Rec,
                        RecordNumber,
                        RecSize,
                        HeaderSize
                        );
         if(Status != SYS_NOMINAL)
         {
            /* Close the file */
            fclose(OidFilePtr);
            /* Clean up Memory allocated to  Oids */
            eHtiCleanupOids(&LocalOidLabelsPtr,&LocalOidsValuesPtr);
            
            /* Set the count of Oids to 0 */
            *Size = 0;
            mHtiSizeOfOidArrays = 0;

            /* Show that memory hasnt been allocated */
            mHtiOidMemAlloc = FALSE;

            eLogCrit(E_HTI_OID_LOOKUP_FAIL,"Failed to read record %i",RecordNumber );
            return E_HTI_OID_LOOKUP_FAIL;
         }      
         eLogDebug("%i: %i, %i, %s, %s", RecordNumber,Rec.CilId,
           Rec.Value, Rec.CilName, Rec.Descr );
                
         /* Check if we have reached the next cil */
         if(Rec.CilId == CilId)
         {
         

            /* Check if we havent reached an Eol marker or Bol Marker */
            if((strcmp(Rec.Label, I_HTI_OID_EOL_MARKER) != 0) && 
            (strcmp(Rec.Label, I_HTI_OID_BOL_MARKER) != 0))
            {
               /* 
               ** Make sure we dont attempt to read beyond the number of
               ** records we counted and allocated space for in the arrays.
               */
               if(Position > mHtiCountOids - 1)
               {
                  eLogCrit(E_HTI_OID_LOOKUP_FAIL, 
                          "Too many Oids being retrieved");
      
                  /* Clean up Memory allocated to  Oids */
                  eHtiCleanupOids(&LocalOidLabelsPtr,&LocalOidsValuesPtr);


                  /* Set the count of Oids to 0 */
                  *Size = 0;
                  mHtiSizeOfOidArrays = 0;
            
                  /* Show that memory hasnt been allocated */
                  mHtiOidMemAlloc = FALSE;            
               
                  return(E_HTI_OID_LOOKUP_FAIL);
               }

               /* store Oid value in value array */
               LocalOidsValuesPtr[Position] = Rec.Value; 

               eLogDebug("Allocate space for label string");
               /* Allocate space for the Label string */
#ifdef _MEMCHECK_
               LocalOidLabelsPtr[Position] =(char  *)mHtiMallocWrap(strlen(Rec.Label) +1);
#else
               LocalOidLabelsPtr[Position] =(char  *)TTL_MALLOC(strlen(Rec.Label) +1);
#endif
               if (LocalOidLabelsPtr[Position] == NULL)
               {
                  Status = E_HTI_MALLOC_FAIL;
            
#ifdef _MEMCHECK_
         
                  mHtiNumberMallocs--;

#endif


                 /* Log the Status */
                  eLogCrit(Status, "Malloc Fail, Fatal Error,"
                                   " mCurrentOidLblsPtr[%i]",
                                   Position);
                  /* Close the file */
                  fclose(OidFilePtr);
                  /* Clean up Memory allocated to  Oids */
                  eHtiCleanupOids(&LocalOidLabelsPtr,&LocalOidsValuesPtr);
            
                  /* Set the count of Oids to 0 */
                  *Size = 0;
                  mHtiSizeOfOidArrays = 0;
   
                  /* Show that memory hasnt been allocated */
                  mHtiOidMemAlloc = FALSE;
   
                  return( Status );
               } /* End if malloc failure. */  
      


               /* Copy label to labels array */
               strcpy(LocalOidLabelsPtr[Position], Rec.Label); 
         
               /*
               ** Show that memory has been allocated for a pos in the label Array
               */
               mHtiSizeOfOidArrays++;

               /* Increment index in arrays to next position */
               Position++;

               /* 
               ** increment the record counter so that we read the 
               ** next record
               */
               RecordNumber++; 
            }
            else
            {
               /* If we are reading an Eol or Bol */


               /* 
               ** increment the record counter so that we read the 
               ** next record
               */
               RecordNumber++; 
            }
         }
         else
         {

            /* if we have reached the next Cil */
         

            /* Check that we have read all the Oids that we counted */ 
            if( ( RecordNumber = mHtiOidEndPos ) && (Position = mHtiCountOids))
            {

               eLogDebug("Oids succesfully read in");

               
               /*
               ** succesfully read all the Oids for specified Cil. Close the 
               ** file
               */
               fclose(OidFilePtr);


               for(Position=0; Position <= *Size -1; Position++)
               {
                  eLogDebug("in Function Oid: 0x%x  Oid Label: %s",
                            LocalOidsValuesPtr[Position],
                            LocalOidLabelsPtr[Position]); 
               }

                      
               /* 
               ** Point the passed in memory address to the allocated memory.
               */
               *OidLabelsPtr = LocalOidLabelsPtr;
               *OidValuesPtr = LocalOidsValuesPtr;

               /* The size is always one ahead */
               mHtiSizeOfOidArrays--;
                        
               /* Set the size of the array */
               *Size = mHtiSizeOfOidArrays;

               /* Show that memory has been allocated */
               mHtiOidMemAlloc = TRUE;
        

               eLogDebug("address labels: %i address Values: %i",
                             LocalOidLabelsPtr,
                             LocalOidsValuesPtr);


               return(SYS_NOMINAL);
            }
            else
            {
               eLogCrit(E_HTI_OID_LOOKUP_FAIL,
                     "Failed to read all Oids");
  
               /* Close the file */
               fclose(OidFilePtr);
               /* Clean up Memory allocated to  Oids */
               eHtiCleanupOids(&LocalOidLabelsPtr,&LocalOidsValuesPtr);
            
               /* Set the count of Oids to 0 */
               *Size = 0;
               mHtiSizeOfOidArrays = 0;
   
               /* Show that memory hasnt been allocated */
               mHtiOidMemAlloc = FALSE;
      
               return(E_HTI_OID_LOOKUP_FAIL);
                   
            }

         }
        
      }
      else
      {  
       /*  if attempting to read a record outside of the range found */

       eLogCrit(E_HTI_OID_LOOKUP_FAIL, 
                "Attempt to read invalid record");
      
       /* Clean up Memory allocated to  Oids */
       eHtiCleanupOids(&LocalOidLabelsPtr,&LocalOidsValuesPtr);


       /* Set the count of Oids to 0 */
       *Size = 0;
       mHtiSizeOfOidArrays = 0;
            
       /* Show that memory hasnt been allocated */
       mHtiOidMemAlloc = FALSE;            
               
       return(E_HTI_OID_LOOKUP_FAIL);



      }
   }

   /* 
   ** We have failed to read in all the oids before reaching the next
   ** EOF
   */
   eLogCrit(E_HTI_OID_LOOKUP_FAIL, "Failed to detect end of Oids");

   /* Close the file */
   fclose(OidFilePtr);

   return(E_HTI_OID_LOOKUP_FAIL);
     
} 



/*******************************************************************************
** Function Name: 
**     eHtiCountOids
** 
** 
** Purpose:
**    To count the number of Oids for a Given CilId. mHtiFindStartOids
** 
** 
** 
** Description: 
**    Checks to see if the start position of given Cil is known. If it isnt
**    then it calls mHtiFindStartOids. Steps through the file line by line
**    from the Start position till it reachs an EOL marker. If it does then it
**    stores the record number in the mHtiEndPos.  
**
** 
** 
** Return Type:
**    Status_t
** 
** 
** 
** Arguements:
**    Int32_t CilId
**       Cilid to count the number of Oids for.
**
**    Int32_t *Count
**       Will store the number of counted oids for specifed Cil.
** 
** 
** 
** Author(s):
**    Daniel W. Smith
** 
*******************************************************************************/
Status_t eHtiCountOids( Int32_t CilId,
                        Int32_t *Count
                       )  
{
   Status_t Status = SYS_NOMINAL; /* Store ttl status */
   Int32_t  RecordNumber = 0;/* Which Record currently being looked at */
   mHtiBinRecord_t Rec;      /* struture to store retrieved record */
   Int32_t NumRecs = 0;      /* Number of records in the file */
   Int32_t RecSize = 0;      /* Size (in bytes) of a record from the file */
   size_t  HeaderSize;       /* Size of the files header */
   FILE    *OidFilePtr;      /* Pointer to the Oid table file */
   Int32_t LocalCount;       /* Store a count of how many Oids found */
   Int32_t EndOfOids;        /* Store the last position of a valid Oid */
   /*
   ** If the specfied Cilid doesnt match the one for which the last start 
   ** Position was found or if the Start position hasnt been found. then
   ** find the start position for specified Cil.
   */
   if( (CilId != mHtiOidCil) || (mHtiOidStartPos == I_HTI_INVALID_POSITION ) )
   {

    
      Status  = mHtiFindStartOids(CilId);      
      if(Status != SYS_NOMINAL)
      {
         eLogCrit(Status,"Failed to find specfied CilId");

         /* Show that CountOids failed */
         mHtiCountOids = I_HTI_NO_OIDS;
         mHtiOidEndPos = I_HTI_INVALID_POSITION;
         *Count =  I_HTI_NO_OIDS;
         return(Status);
      }


    }



   /* Open the Bin Oid table file and read header */
   Status = mHtiOpenBinFile(     
                         &OidFilePtr,
                         &RecSize,
                         &NumRecs,
                         &HeaderSize
                        );
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status,"Failed to open file");
      
      /* Show that CountOids failed */
      mHtiCountOids = I_HTI_NO_OIDS;
      mHtiOidEndPos = I_HTI_INVALID_POSITION;
      *Count =  I_HTI_NO_OIDS;

      return(Status);
   }


   LocalCount = 0;

   /* First record we read in is the located start position. */
   RecordNumber = mHtiOidStartPos;

   /* Make sure no attempts are made to read over the end of the file.*/
   while(!feof(OidFilePtr)) 
   {
      /* 
      ** If an attempt was made to read past the last record and the
      ** requested Cil Hasnt been counted close the file and return.
      */ 
      if(RecordNumber >= NumRecs)
      {
         Status = E_HTI_RECORD_OUTRANGE;
         eLogCrit(Status,"Attempt to read out of range record");                     
         
         /* Show that CountOids failed */
         mHtiCountOids = I_HTI_NO_OIDS;
         mHtiOidEndPos = I_HTI_INVALID_POSITION;
         *Count =  I_HTI_NO_OIDS;

         /* Close file and return */
         fclose(OidFilePtr);
         return Status;
      }

      /* Read record from Oid table */
      Status = mHtiReadRecord(
                        OidFilePtr,
                        &Rec,
                        RecordNumber,
                        RecSize,
                        HeaderSize
                        );
      if(Status != SYS_NOMINAL)
      {
         eLogCrit(Status,"Failed to read record %i",RecordNumber );
         
         /* Show that CountOids failed */
         mHtiCountOids = I_HTI_NO_OIDS;
         mHtiOidEndPos = I_HTI_INVALID_POSITION;
         *Count =  I_HTI_NO_OIDS;

         /* Close file and return */
         fclose(OidFilePtr);            
         
         return Status;
      }

      eLogDebug("%i: %i, %i, %s, %s", RecordNumber,Rec.CilId,
      Rec.Value, Rec.CilName, Rec.Descr );                                                                


      /* Make sure that we dont try to read an Oid for the next CilId */
      if( CilId == Rec.CilId )
      {
         /* make sure we havent reached an Eol marker or Bol Marker */
         if((strcmp(Rec.Label, I_HTI_OID_EOL_MARKER) != 0) && 
            (strcmp(Rec.Label, I_HTI_OID_BOL_MARKER) != 0))
         {
            /* If the current record isnt a Bol or Eol*/

            LocalCount++; /* Increment the Number of Oids found */
            

            EndOfOids = RecordNumber;

            /* 
            ** increment the record counter so that we read the 
            ** next record
            */
            RecordNumber++; 
         }
         else
         {
            
            /* 
            ** increment the record counter so that we read the 
            ** next record
            */
            RecordNumber++; 
         }

      }
      else
      {

         /* If there are no Oids found for this cil return */
         if(LocalCount == 0)
         {
            Status = E_HTI_NO_OIDS; 
            /* Close the file */
            fclose(OidFilePtr);
             eLogDebug("No Oids found for this Cil");
        
            /* Show that CountOids failed */
            mHtiCountOids = I_HTI_NO_OIDS;
            mHtiOidEndPos = I_HTI_INVALID_POSITION;
            LocalCount =  I_HTI_NO_OIDS;

               
            /* Close file and return */
            fclose(OidFilePtr);   

            return(Status);
         }


         

         /* Store the position of the last valid Oid
            and the number of Oids Found */
         mHtiOidEndPos  = EndOfOids;  
         mHtiCountOids = LocalCount;
         *Count = LocalCount;
         
         eLogDebug("Found %i Oids", LocalCount);
         eLogDebug("Last Record at Position %i", mHtiOidEndPos);
     


         /* Close file and return */
         fclose(OidFilePtr);    

         return(Status);
           
      }

   } 
 

   /*
   ** If we get this far then at attempt has been made to read beyond the end 
   ** of the file.
   */
   Status = E_HTI_RECORD_OUTRANGE; 
   eLogCrit(Status, "Fatal error attempt to read beyond end of file");
   
   /* Show that CountOids failed */
   mHtiCountOids = I_HTI_NO_OIDS;
   mHtiOidEndPos = I_HTI_INVALID_POSITION;
   *Count =  I_HTI_NO_OIDS;
                     

   /* Close file and return */
   fclose(OidFilePtr);
   
   return(Status);
} 


/*******************************************************************************
** Function Name: 
**   mHtiFindStartOids
** 
** 
** Purpose:
**   With a given Cil to find the start Record of the Oids in the OidTable. 
** 
** 
** 
** Description: 
**   Opens the Oid table. Reads through it line by line to find the first Oid 
**   for the specified Cil Id when it finds stores the value in mHtiStartPos
**   closes the file and returns if it fails at any point reports failure closes
**   file and returns. 
**
** Return Type:
**    Status_t 
** 
** 
** 
** Arguements:
**    Int32_t CilId 
**       Cild id to search for.
**
**
**   Int32_t *StartPositionPtr
**       Pointer to integer to store position of first Oid of specified Cil.
** 
** 
** Author(s):
** 
** 
*******************************************************************************/
Status_t mHtiFindStartOids(Int32_t CilId)
{
   Status_t Status = SYS_NOMINAL; /* Store ttl status */
   Int32_t  RecordNumber = 0;/* Which Record currently being looked at */
   mHtiBinRecord_t Rec;      /* struture to store retrieved record */
   Int32_t NumRecs = 0;      /* Number of records in the file */
   Int32_t RecSize = 0;      /* Size (in bytes) of a record from the file */
   size_t  HeaderSize;       /* Size of the files header */
   FILE    *OidFilePtr;      /* Pointer to the Oid table file */


   Status = mHtiOpenBinFile(     
                         &OidFilePtr,
                         &RecSize,
                         &NumRecs,
                         &HeaderSize
                        );
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status,"Failed to open file");

      /* Show that Find StartOids Failed */
      mHtiOidStartPos = I_HTI_INVALID_POSITION;

      return(Status);
   }




   /* Make sure we start at the first Record */
   RecordNumber = 0;

   
   /*
   ** Search for the Cil and record the start position. Make sure no attempts
   ** are made to read beyond the end of the file.
   */
   while( !feof(OidFilePtr))
   {

        /* 
        ** If an attempt was made to read past the last record and the
        ** requested Cil Hasnt been found close the file and return.
        */ 
        if(RecordNumber >= NumRecs)
        {
            Status = E_HTI_RECORD_OUTRANGE;

                  
            eLogCrit(Status,"Reached end of records.");                     

            /* Show that Find StartOids Failed */
            mHtiOidStartPos = I_HTI_INVALID_POSITION;
            
            /* Close file and return */
            fclose(OidFilePtr);
            return Status;
        }

             
        /* Read record from Oid table */
        Status = mHtiReadRecord(
                        OidFilePtr,
                        &Rec,
                        RecordNumber,
                        RecSize,
                        HeaderSize
                        );
         if(Status != SYS_NOMINAL)
         {
            eLogCrit(Status,"Failed to read record %i",RecordNumber );

            /* Show that Find StartOids Failed */
            mHtiOidStartPos = I_HTI_INVALID_POSITION;

            /* Close file and return */
            fclose(OidFilePtr);            
            return Status;
         }
         eLogDebug("%i: %i, %i, %s, %s, %s", RecordNumber,Rec.CilId,
         Rec.Value, Rec.CilName, Rec.Label, Rec.Descr );



        

         /*
         **   If the CilId retrieved from the current record matches the 
         **   requested Cilid 
         */
         if( CilId == Rec.CilId )
         {

          
            
  


            /* Make sure this isnt a Bol or an Eol record */
            if((strcmp(Rec.Label, I_HTI_OID_EOL_MARKER) != 0) &&
            (strcmp(Rec.Label, I_HTI_OID_BOL_MARKER) != 0))
            {

               /* this record is the first record */
               mHtiOidStartPos = RecordNumber;

               /* Show which Cil we have found the start for */
               mHtiOidCil = CilId;

               eLogDebug("First useable Oid %i", mHtiOidStartPos);
               
               /* Close file and return */
               fclose(OidFilePtr);
               return(SYS_NOMINAL);

             }



         }
         
         /* Step to next record */
         RecordNumber++; 

   }

   /*
   ** If we get this far then at attempt has been made to read beyond the end 
   ** of the file.
   */
   Status = E_HTI_RECORD_OUTRANGE; 
   eLogCrit(Status, "Fatal error attempt to read beyond end of file");
   
   /* Show that Find StartOids Failed */
   mHtiOidStartPos = I_HTI_INVALID_POSITION;

   /* Close file and return */
   fclose(OidFilePtr);
   
   return(Status);

}

/*******************************************************************************
** Function Name: 
**    mHtiOpenBinFile
** 
** 
** Purpose:
**   To open a Hti binary file and retrieve the header information.
** 
** 
** 
** Description: 
**   Opens the file. headers the header and stores the record size, number of 
**   records and the size of the header in the given variables.
** 
** 
** 
** Return Type:
**    Status_t
** 
** 
** 
** Arguements:
**    FILE    **FilePtr
**       Pointer to address to store open file pointer.
**
**    Int32_t *RecordSizePtr
**       Pointer to integer to store record size of open file.
**                         
**    Int32_t *NumberRecordsPtr
**       Pointer to integer to store number of records in open file.      
**
**    size_t  *HeaderSizePtr
**       Pointer to integer to store Size of the header in open file.
** 
** 
** Author(s):
** 
** 
*******************************************************************************/
Status_t mHtiOpenBinFile(FILE    **FilePtr,
                         Int32_t *RecordSizePtr,
                         Int32_t *NumberRecordsPtr,
                         size_t  *HeaderSizePtr
                        )
{
   Int32_t HeaderBuffer[2];      /* Buffer for the two elements of the header */
   Int32_t NumElts;              /* the number of elements read by fread() */



   /* Open the file */
   *FilePtr = fopen( eHtiBinIdTableName(), "rb");
   if(*FilePtr == NULL) return E_HTI_FILE_ERROR;

   /* Read in the header to determine the number and size of the records */
   NumElts = fread(HeaderBuffer, sizeof(Int32_t), 2, *FilePtr);
   if(NumElts != 2)
   {
      fclose(*FilePtr);
      return E_HTI_BIN_HEADER_FAILURE;
   }

   /* store the number of records and size of the records in given variables.*/
   *NumberRecordsPtr = HeaderBuffer[0];
   *RecordSizePtr = HeaderBuffer[1];

   eLogDebug(" Num Recs: %i  Recsize: %i", NumberRecordsPtr, RecordSizePtr);

   /* the size of the header */
   *HeaderSizePtr = NumElts *(sizeof(Int32_t)); 

   return(SYS_NOMINAL);

}
/*******************************************************************************
** Function Name: 
**    eHtiCleanupOids
** 
** 
** Purpose:
**    To Free memory allocated to the Oid arrays.
** 
** 
** 
** Description: 
**    checks to see if the memory address pointed to by OidLabelsPtr is NULL
**    if it isnt then attmepts to free Size number of elements from the array 
**    that it Points to and then frees OidLabelsPtr . Then checks to see if
**    memory  address pointed to by OidValuesPtr is NULL if it isnt then 
**    frees it.
** 
** Return Type:
**    Status_t
** 
** 
** 
** Arguements:
**    char     ***OidLabelsPtr
**       A pointer to a pointer to a pointer of Oid labels.  Effectively this 
**       is a pointer to an array of char *'s.         
** 
**    Int32_t  **OidValuesPtr, 
**       A pointer to a pointer of Oid values . Effectively this 
**       is a array of Int32_t's.
**
** Author(s):
**    Daniel W. Smith
** 
*******************************************************************************/
Status_t eHtiCleanupOids( char ***OidLabelsPtr, Int32_t **OidValuesPtr) 
{
   

   Int32_t Position;
   char    **LocalOidLabelsPtr; /*Local Pointer to the passed in OidLabelsPtr */
   Int32_t *LocalOidsValuesPtr; /* Local Pointer to the passed in Oid  */


   /* 
   ** To make code easier to follow point local pointers
   ** to passed in Pointers 
   */
   LocalOidLabelsPtr = *OidLabelsPtr;
   LocalOidsValuesPtr = *OidValuesPtr;

   /* Free memory allocated for labels */
   if( LocalOidLabelsPtr != NULL)
   {
      eLogDebug("Checked that the label array has been allocated");
      
      for(Position = 0; Position <  mHtiSizeOfOidArrays; Position++)
      {  
         eLogDebug(" Position: %i", Position);
         
         if( LocalOidLabelsPtr[Position] != NULL)
         {
            eLogDebug("Checked that array position  is in use %s" 
                             ,LocalOidLabelsPtr[ Position ] );  
         
#ifdef _MEMCHECK_      
            mHtiFreeWrap( LocalOidLabelsPtr[ Position ] ) ;
#else
            TTL_FREE( LocalOidLabelsPtr[ Position ] ) ;
#endif

            eLogDebug("Freed LocalOidLabelsPtr[%i]", Position);             
         }

      }

      eLogDebug("Freed individual labels memory");

#ifdef _MEMCHECK_
      mHtiFreeWrap(LocalOidLabelsPtr);
#else

      TTL_FREE(LocalOidLabelsPtr);
#endif

      eLogDebug("Freed Oid labels");
   }

   /* Free memory allocated for values */
   if( LocalOidsValuesPtr != NULL)
   {
      eLogDebug("Checked that Oids value arrays in use");
#ifdef _MEMCHECK_
      mHtiFreeWrap(LocalOidsValuesPtr);
#else
      TTL_FREE(LocalOidsValuesPtr);
#endif

      eLogDebug("Freed Oid values");
   }



  
   /* 
   ** Make sure the 2 arrays point to the freed memory address.
   */
   *OidLabelsPtr = LocalOidLabelsPtr;
   *OidValuesPtr = LocalOidsValuesPtr;
 
   /* Show that memory hasnt been allocated */
   mHtiOidMemAlloc = FALSE;


   mHtiSizeOfOidArrays = 0;
   return(SYS_NOMINAL);
}
/*******************************************************************************
** Function Name: 
**    mHtiReadRecord
** 
** 
** Purpose:
**   To read a record from a Hti file pointed to by FilePtr.
** 
** 
** 
** Description: 
**    Jumps to the record specified by RecordNumber by seeking 
**    (RecordNumber * RecSize) + HeaderSize bytes in the file from the start of
**    file. Reads in the Record into a character array and then breaks the array
**    into its constituent fields and stores them in the structure pointed to by
**    Rec.
** 
** 
** Return Type:
**    
** 
** 
** 
** Arguements:
**    FILE            *FilePtr
**       A pointer to an open Binary Hti file
**
**    mHtiBinRecord_t *Rec
**       Pointer to a structure to store the read record into.
**
**    Int32_t         RecordNumber
**       The record to read in.
**    
**    Int32_t         RecSize
**       The Size of the records.
**
**    size_t         HeaderSize
**       The size of the header for the file.
** 
** 
** 
** Author(s):
**   Daniel W.Smith
** 
*******************************************************************************/
Status_t mHtiReadRecord   (FILE            *FilePtr,
                           mHtiBinRecord_t *Rec,
                           Int32_t         RecordNumber,
                           Int32_t         RecSize,
                           size_t        HeaderSize)
{
     size_t Pos;               /* Position within a single record of values */
     char Record[E_HTI_MAX_STRING_LEN];/* Buffer in which to hold a record */
     size_t  NumElts;          /* The number of elements read with fread() */
     Int32_t SeekCheck;

     eLogDebug("about to read record: %i",RecordNumber); 

      /* Clear the record */
      Rec->CilId = -1;
      Rec->Value = -1;
      Rec->CilName[0] = '\0';
      Rec->Label[0] = '\0';
      Rec->Descr[0] = '\0';
     
      /* Jump to a record */
      SeekCheck = fseek(FilePtr, (RecSize * RecordNumber) + 
                        HeaderSize , SEEK_SET);
      if(SeekCheck != 0)
      {
         eLogCrit(E_HTI_BIN_READ_ERROR,"Error seeking position in file");
         return(E_HTI_BIN_READ_ERROR);
      }

      /* Read the record */
      NumElts = fread(Record, RecSize, 1, FilePtr);
      if(NumElts != 1)
      {
         fclose(FilePtr);
         return E_HTI_BIN_READ_ERROR;
      }

      /* Extract the components of the record */
      Pos = 0;
      memcpy((void *)&Rec->CilId,
             (void *)&Record[Pos], sizeof(Rec->CilId));
      Pos += sizeof(Rec->CilId);
      memcpy((void *)&Rec->Value,
             (void *)&Record[Pos], sizeof(Rec->Value));
      Pos += sizeof(Rec->Value);
      strcpy((void *)&Rec->CilName, (void *)&Record[Pos]);
      Pos += strlen(Rec->CilName) + 1;
      strcpy((void *)&Rec->Label, (void *)&Record[Pos]);
      Pos += strlen(Rec->Label) + 1;
      strcpy((void *)&Rec->Descr, (void *)&Record[Pos]);

return(SYS_NOMINAL);

}


char * eHtiIdTableName       ( void )
{
   int      LookupSet;
   char *   FilenamePtr;
   FILE *   FilePtr;

   LookupSet = 0;
   do
   {
      FilenamePtr = iHtiFilename[ LookupSet ].IdTable;
      FilePtr = fopen( FilenamePtr, "r" );
      LookupSet++;
   }
   while ( ( FilePtr == NULL ) && ( LookupSet < I_HTI_LOOKUP_SETS ) );

   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }

   eLogInfo( "Using ID table '%s'", FilenamePtr );

   return FilenamePtr;

}


char * eHtiUnitTableName     ( void )
{
   int      LookupSet;
   char *   FilenamePtr;
   FILE *   FilePtr;

   LookupSet = 0;
   do
   {
      FilenamePtr = iHtiFilename[ LookupSet ].UnitTable;
      FilePtr = fopen( FilenamePtr, "r" );
      LookupSet++;
   }
   while ( ( FilePtr == NULL ) && ( LookupSet < I_HTI_LOOKUP_SETS ) );

   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }

   eLogInfo( "Using units table '%s'", FilenamePtr );

   return FilenamePtr;

}


char * eHtiFullTableName     ( void )
{
   int      LookupSet;
   char *   FilenamePtr;
   FILE *   FilePtr;

   LookupSet = 0;
   do
   {
      FilenamePtr = iHtiFilename[ LookupSet ].FullTable;
      FilePtr = fopen( FilenamePtr, "r" );
      LookupSet++;
   }
   while ( ( FilePtr == NULL ) && ( LookupSet < I_HTI_LOOKUP_SETS ) );

   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }

   eLogInfo( "Using full table '%s'", FilenamePtr );

   return FilenamePtr;

}


char * eHtiStatusTableName   ( void )
{
   int      LookupSet;
   char *   FilenamePtr;
   FILE *   FilePtr;

   LookupSet = 0;
   do
   {
      FilenamePtr = iHtiFilename[ LookupSet ].StatusTable;
      FilePtr = fopen( FilenamePtr, "r" );
      LookupSet++;
   }
   while ( ( FilePtr == NULL ) && ( LookupSet < I_HTI_LOOKUP_SETS ) );

   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }

   eLogInfo( "Using status table '%s'", FilenamePtr );

   return FilenamePtr;

}


char * eHtiBinIdTableName    ( void )
{
   int      LookupSet;
   char *   FilenamePtr;
   FILE *   FilePtr;

   LookupSet = 0;
   do
   {
      FilenamePtr = iHtiFilename[ LookupSet ].BinIdTable;
      FilePtr = fopen( FilenamePtr, "r" );
      LookupSet++;
   }
   while ( ( FilePtr == NULL ) && ( LookupSet < I_HTI_LOOKUP_SETS ) );

   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }

   eLogInfo( "Using binary ID table '%s'", FilenamePtr );

   return FilenamePtr;

}


char * eHtiBinUnitTableName  ( void )
{
   int      LookupSet;
   char *   FilenamePtr;
   FILE *   FilePtr;

   LookupSet = 0;
   do
   {
      FilenamePtr = iHtiFilename[ LookupSet ].BinUnitTable;
      FilePtr = fopen( FilenamePtr, "r" );
      LookupSet++;
   }
   while ( ( FilePtr == NULL ) && ( LookupSet < I_HTI_LOOKUP_SETS ) );

   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }

   eLogInfo( "Using binary units table '%s'", FilenamePtr );

   return FilenamePtr;

}


char * eHtiBinFullTableName  ( void )
{
   int      LookupSet;
   char *   FilenamePtr;
   FILE *   FilePtr;

   LookupSet = 0;
   do
   {
      FilenamePtr = iHtiFilename[ LookupSet ].BinFullTable;
      FilePtr = fopen( FilenamePtr, "r" );
      LookupSet++;
   }
   while ( ( FilePtr == NULL ) && ( LookupSet < I_HTI_LOOKUP_SETS ) );

   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }

   eLogInfo( "Using binary full table '%s'", FilenamePtr );

   return FilenamePtr;

}


char * eHtiBinStatusTableName( void )
{
   int      LookupSet;
   char *   FilenamePtr;
   FILE *   FilePtr;

   LookupSet = 0;
   do
   {
      FilenamePtr = iHtiFilename[ LookupSet ].BinStatusTable;
      FilePtr = fopen( FilenamePtr, "r" );
      LookupSet++;
   }
   while ( ( FilePtr == NULL ) && ( LookupSet < I_HTI_LOOKUP_SETS ) );

   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }

   eLogInfo( "Using binary status table '%s'", FilenamePtr );

   return FilenamePtr;

}


#ifdef _MEMCHECK_
void *mHtiMallocWrap(size_t Size)
{

   mHtiNumberMallocs++;

   return(TTL_MALLOC(Size));

}

void mHtiFreeWrap(void *Aptr)
{
   mHtiNumberFrees++;

   eLogCrit(NULL," Mem freed");

   TTL_FREE(Aptr);

}

Status_t eHtiPrintMemCheck( void )
{
   eLogCrit(NULL,"HTI- Number allocs: %i Number Frees: %i", mHtiNumberMallocs,
                                                          mHtiNumberFrees);

 return(SYS_NOMINAL);
}   

void eHtiResetMemCounters( void )
{

   mHtiNumberMallocs = 0;

   mHtiNumberFrees = 0;
}

#endif
/*******************************************************************************
** End of File Name: Hti.c
*******************************************************************************/
