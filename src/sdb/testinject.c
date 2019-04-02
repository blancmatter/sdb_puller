/*
** Module Name:
**    testinject.c
**
** Purpose:
**    A program for injecting test data into the SDB.
**
** Description:
**    This module contains code for a SDB testing program. It sends
**    a data submission to the SDB based on user input.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    mjf: Martyn J. Ford (TTL)
**
** Version:
**    $Id: testinject.c,v 0.10 2004/01/19 11:06:06 sxf Exp $
**
** History:
**    $Log: testinject.c,v $
**    Revision 0.10  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.9  2001/02/14 13:53:02  mjf
**    Use HTI library for Header-to-ID conversions in testlist and testinject.
**
**    Revision 0.8  2001/01/26 10:53:08  mjf
**    No need for '-names' switch on the command-line.
**
**    Revision 0.7  2001/01/23 13:20:00  mjf
**    Addition of functionality to inject with CIL names and labels for
**    datums, values and units.
**
**    Revision 0.6  2001/01/23 11:34:44  mjf
**    Integration of CLU, and specification of inject file on command-line
**    rather than piped.
**
**    Revision 0.5  2000/11/07 10:26:16  mjf
**    Testinject sends CIL messages directly to SDB, not via MCB.
**
**    Revision 0.4  2000/10/04 11:09:05  djm
**    Converted input from decimal to hexadecimal.
**
**    Revision 0.3  2000/09/05 14:23:37  djm
**    Changed Cil.map location constant.
**
**    Revision 0.2  2000/08/29 10:25:11  djm
**    Added multiple datum submission.
**
**    Revision 0.1  2000/08/22 15:44:11  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>


#include "TtlSystem.h"
#include "Wfl.h"
#include "Cil.h"
#include "Tim.h"
#include "Sdb.h"
#include "Clu.h"
#include "Log.h"
#include "Hti.h"


/* Application details - normally found in XxxPrivate.h */

#define I_XXX_PROGRAM_NAME   "testinject"
#define I_XXX_PROGRAM_ABOUT  "Inject data into the SDB"
#define I_XXX_RELEASE_DATE   "23 January 2001"
#define I_XXX_YEAR           "2000-01"
#define I_XXX_MAJOR_VERSION  0
#define I_XXX_MINOR_VERSION  1

/* Common arguments defaults */

#define M_XXX_DFLT_QUIET     FALSE
#define M_XXX_DFLT_VERBOSE   TRUE
#define M_XXX_DFLT_SYSLOG    TRUE
#define M_XXX_DFLT_DEBUG     E_LOG_NOTICE
#define M_XXX_DFLT_PRIORITY  10
#define M_XXX_DFLT_HELP      FALSE
#ifdef E_WFL_OS_QNX4
#define M_XXX_DFLT_CONFIG    "/opt/ttl/etc/testinject.cfg"
#define M_XXX_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#define M_XXX_DFLT_ID_PATH   "/opt/ttl/etc"    /* Default path for ID tables */
#else
#define M_XXX_DFLT_CONFIG    "/ttl/sw/etc/testinject.cfg"
#define M_XXX_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#define M_XXX_DFLT_ID_PATH   "/ttl/sw/etc"    /* Default path for ID tables */
#endif
#define M_XXX_DFLT_LOG       "testinject.txt"
#define M_XXX_DFLT_CIL       "TU0"

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define M_XXX_CUSTOM_FILE    0
#define M_XXX_CUSTOM_IDPATH  1

#define M_XXX_CUSTOM_ARGS    2


/* Symbolic constants */

#define M_XXX_ROUTER         E_CIL_SDB /* Choice router */
#define M_XXX_SERVER         E_CIL_SDB /* Choice server to send to */

#define M_XXX_NUM_ARGS       5         /* Number of arguments on inject line */
#define M_XXX_STRING_LEN     128       /* Maximum length of string */
#define M_XXX_IDTABLE_PARAMS 3         /* Number of params in ID table string */
#define M_XXX_IDTABLE_STR    "%s %s \"%[^\"]\""/* String for format of ID table */
#define M_XXX_ID_FILE        "FullTable.idx"   /* Full ID table file */
#define M_XXX_UNIT_FILE      "UnitsTable.idx"  /* Units table file */


/* Global variables */

char IdPath[ M_XXX_STRING_LEN ]; /* Path for ID lookup tables */

eCluArgSpec_t eCluCustomArg[ M_XXX_CUSTOM_ARGS + 1 ] = 
/*  arg [param],    match, description,                       FALSE, NULL */ 
{ { "file <filename>", 1, "Name of SDB file to read",         FALSE, NULL },
  { "idpath <path>",   3, "Alternate path for names index",   FALSE, NULL },
  { E_CLU_EOL,         0, E_CLU_EOL,                          FALSE, NULL }
};


/*
** MAIN FUNCTION
*/

int main (int argc, char *argv[])
{
/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    ...
**
** Description:
**    ...
**
** Arguments:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    22-Aug-2000 djm Initial creation.
**
*/

   /* Local variables */

   Status_t Status;          /* Function return status */
   Int32_t RxId;             /* CIL ID of sender of rx'd messages */
   char TxData[512];         /* Buffer for data to send */
   char RxData[512];         /* Data for received data */
   int Timeout = 300;        /* How long to wait before timing out on rx */
   eCilMsg_t TxMsg;          /* CIL message structure to send */
   eCilMsg_t RxMsg;          /* CIL message structure to receive */
   Int32_t CilTestUnit;      /* CIL test unit ID */
   char Another[ M_XXX_STRING_LEN ]; /* User request for more data */
   Uint32_t NumElts;         /* Number of data elements to submit/retrieve */
   eSdbDatum_t Datum;        /* Structure for Tx/Rx-ing data */
   int n;
   Status_t CluStatus;       /* Return value from called CLU functions */
   char     Switch[ E_CLU_SWITCH_LEN ];/* to report error parsing a switch */
   Bool_t   FileSupplied;    /* flag set TRUE if filename given */
   char     Filename[ M_XXX_STRING_LEN ]; /* supplied inject file name */
   FILE   * FileId;          /* pointer to inject file */
   int      NumArgs;         /* number of converted arguments from 'scanf' */

   char    StrSrc[ M_XXX_STRING_LEN ]; /* String containing SourceID */
   char    StrDat[ M_XXX_STRING_LEN ]; /* String containing DatumID */
   char    StrVal[ M_XXX_STRING_LEN ]; /* String containing Datum value */
   char    StrUts[ M_XXX_STRING_LEN ]; /* String containing Datum units */
   Int32_t ReqSrc;           /* SourceID, as requested by the user */
   Int32_t ReqDat;           /* DatumID, as requested by the user */
   Int32_t ReqVal;           /* Datum value, as requested by the user */
   Int32_t ReqUts;           /* Datum units, as requested by the user */


   /* defaults */
   strcpy( IdPath, M_XXX_DFLT_ID_PATH );
   FileSupplied = FALSE;

   /* data for command-line utilities */
   eCluProgNamePtr              = I_XXX_PROGRAM_NAME;
   eCluProgAboutPtr             = I_XXX_PROGRAM_ABOUT;
   eCluReleaseDatePtr           = I_XXX_RELEASE_DATE;
   eCluYearPtr                  = I_XXX_YEAR;
   eCluMajorVer                 = I_XXX_MAJOR_VERSION;
   eCluMinorVer                 = I_XXX_MINOR_VERSION;
  
   /* set default parameters before parsing command-line */
   eCluCommon.Quiet             = M_XXX_DFLT_QUIET;
   eCluCommon.Verbose           = M_XXX_DFLT_VERBOSE;
   eCluCommon.Syslog            = M_XXX_DFLT_SYSLOG;
   eCluCommon.DebugLevel        = M_XXX_DFLT_DEBUG;
   eCluCommon.Priority          = M_XXX_DFLT_PRIORITY;
   eCluCommon.Help              = M_XXX_DFLT_HELP;
   strcpy( eCluCommon.ConfigFile, M_XXX_DFLT_CONFIG  );
   strcpy( eCluCommon.LogFile,    M_XXX_DFLT_LOG     );
   strcpy( eCluCommon.CilName,    M_XXX_DFLT_CIL     );
   strcpy( eCluCommon.CilMap,     M_XXX_DFLT_CIL_MAP );

   /* register with the command-line utilities (CLU), ignore all except help */
   CluStatus = eCluSetup( argc, argv, 
                          E_CLU_IGN_QUIET | E_CLU_IGN_VERBOSE 
                           | E_CLU_IGN_SYSLOG | E_CLU_IGN_NOSYSLOG
                           | E_CLU_IGN_DEBUG | E_CLU_IGN_PRIORITY
                           | E_CLU_IGN_CONFIGFILE | E_CLU_IGN_LOGFILE );

   /* if setup was successful */
   if ( CluStatus == SYS_NOMINAL )
   {
      /* parse the incoming command-line */
      CluStatus = eCluParseArgs( Switch );

      /* if command-line parsed successfully */
      if ( CluStatus == SYS_NOMINAL )
      {  
         /* check for help requested */
         if ( eCluCommon.Help == TRUE )
         {
            /* exit the program */
            exit( EXIT_SUCCESS );
         }

         /* if filename specified */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_FILE ) == E_CLU_ARG_SUPPLIED )
         {
            /* set flag indicating file supplied */
            FileSupplied = TRUE;
            /* take copy of supplied filename */
            strcpy( Filename, eCluGetCustomParam( M_XXX_CUSTOM_FILE ) ) ;
         }
         
         /* check for existence of ID path */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_IDPATH ) == E_CLU_ARG_SUPPLIED )
         {
            strcpy( IdPath, eCluGetCustomParam( M_XXX_CUSTOM_IDPATH) );
         }
      }
      /* else command-line not parsed successfully */
      else
      {
         eCluShowUsage( NULL, NULL );
         printf( "Error parsing the command-line switch '%s'\n", Switch );
         /* exit the program */
         exit( EXIT_SUCCESS );
      }
   }
   /* else command-line not parsed successfully */
   else
   {
      printf( "Error parsing the command-line\n" );
      /* exit the program */
      exit( EXIT_SUCCESS );
   }

   Status = eCluSignOn( NULL, NULL );
   if(Status != SYS_NOMINAL)
   {
      printf
      (
         "Error: Failed to sign-on (%d = 0x%x)\n", 
         Status, Status
      );
      exit(EXIT_FAILURE);
   }

   /* attempt to determine the CIL ID */
   Status = eCilLookup( eCluCommon.CilMap, eCluCommon.CilName, &CilTestUnit );

   /* attempt to perform CIL setup now CIL ID has been determined */
   Status = eCilSetup( eCluCommon.CilMap, CilTestUnit );

   /* check status returned - report any errors */
   switch ( Status )
   {
      case SYS_NOMINAL :
         /* not an error - successful completion */
         break;

      default :
         printf( "Unable to perform CIL setup : error %X\n", Status );
         exit(EXIT_FAILURE);
         break;
   }

   /* if a filename to be used for input */
   if ( FileSupplied == TRUE )
   {
      /* attempt to open the file */
      FileId = fopen( Filename, "r" );

      /* if file not opened successfully, report error and exit */
      if ( FileId == NULL )
      {
         printf( "Unable to open inject file '%s'\n", Filename );
         exit( EXIT_FAILURE );
      }
   }

   do
   {
      /* if no file supplied, get input from the console */
      if ( FileSupplied == FALSE )
      {
         printf("           Enter the source ID (hex/label): ");
         scanf("%s", &StrSrc);
         printf("            Enter the datum ID (hex/label): ");
         scanf("%s", &StrDat);
         printf("           Enter the raw value (hex/label): ");
         scanf("%s", &StrVal);
         printf("Enter the units code (hex/label, 1 = none): ");
         scanf("%s", &StrUts);

         /* set number of arguments correctly so inject gets done */
         NumArgs = M_XXX_NUM_ARGS;
      }
      /* else get input from the file */
      else
      {
         /* clear string indicating another datum to follow */
         Another[ 0 ] = '\0';
         /* attempt to read input from the file */
         NumArgs = fscanf( FileId, "%s %s %s %s %s\n", 
                           &StrSrc, &StrDat, &StrVal, &StrUts, &Another );
      }

      if ( NumArgs == M_XXX_NUM_ARGS )
      {

         /* attempt to convert CIL name into a CIL ID */
         if ( eCilLookup( eCluCommon.CilMap, StrSrc, &ReqSrc ) != SYS_NOMINAL )
         {
            /* not converted - assume string contains a number */
            sscanf( StrSrc, "%x", &ReqSrc );
         }
         /* attempt to convert string into a datum ID */
         if ( eHtiGetDataId( ReqSrc, StrDat, &ReqDat ) != SYS_NOMINAL )
         {
            /* not converted - assume string contains a number */
            sscanf( StrDat, "%x", &ReqDat );
         }
         /* attempt to convert string into a value */
         if ( eHtiGetGeneralId( ReqSrc, StrVal, &ReqVal ) != SYS_NOMINAL )
         {
            /* not converted - assume string contains a number */
            sscanf( StrVal, "%x", &ReqVal );
         }
         /* attempt to convert string into units */
         if ( eHtiGetUnitId( StrUts, &ReqUts ) != SYS_NOMINAL )
         {
            /* not converted - assume string contains a number */
            sscanf( StrUts, "%x", &ReqUts );
         }


         /* Fill the data buffer with some characters
         memset (TxData, 0xaa, sizeof (TxData));
         */

         /* Setup the common CIL message structure */
         TxMsg.SourceId = ReqSrc;
         TxMsg.DestId = M_XXX_SERVER;
         TxMsg.Class = E_CIL_CMD_CLASS;
         TxMsg.SeqNum = 0;
         TxMsg.TimeStamp.t_sec = 0;
         TxMsg.TimeStamp.t_nsec = 0;

         TxMsg.Service = E_SDB_SUBMIT_1;
         NumElts = 1;
         TxMsg.DataLen = sizeof(NumElts) + sizeof(eSdbDatum_t) * NumElts;
         NumElts = htonl(NumElts);
         memcpy(TxData, &NumElts, sizeof(NumElts));

         for(n = 0; n < ntohl(NumElts); n++)
         {
            Datum.SourceId = htonl(ReqSrc);
            Datum.DatumId = htonl(ReqDat);
            Datum.Units = htonl(ReqUts);
            Status = eTimGetTime(&(Datum.Msrment.TimeStamp));
            Datum.Msrment.TimeStamp.t_sec  = htonl(Datum.Msrment.TimeStamp.t_sec);
            Datum.Msrment.TimeStamp.t_nsec = htonl(Datum.Msrment.TimeStamp.t_nsec);

            Datum.Msrment.Value = htonl(ReqVal);
            memcpy(
               TxData+sizeof(NumElts)+sizeof(eSdbDatum_t)*n,
               &Datum,  sizeof(Datum)
            );
         }

         printf("\nMessage packet content:\n");
         for(n = 0; n < TxMsg.DataLen; n++) {
            if(((n-4)%sizeof(eSdbDatum_t)) == 0) printf("\n");
            printf("%2.2x ", TxData[n]);
         }
         printf("\n\n");
         TxMsg.DataPtr = TxData;
  

         /* Actually send the message */
         Status = eCilSend (M_XXX_ROUTER, &TxMsg);
         if (Status != SYS_NOMINAL)
         {
            printf ("Send failure - %d\n", Status);
            exit(EXIT_FAILURE);
         }

         /* Prepare to receive some data */

         /* Actually receive the data */
         while(Status != E_CIL_TIMEOUT) {
            RxMsg.DataPtr = RxData;
            RxMsg.DataLen = sizeof (RxData);
            Status = eCilReceive (Timeout, &RxId, &RxMsg);
            printf("Receiving data...\n");
         }


         /* if no file supplied, get input from the console */
         if ( FileSupplied == FALSE )
         {
            printf("Enter another datum (y/n)?: ");
            scanf("%s", &Another);
         }
      }
   }
   while( (Another[0] == 'y') || (Another[0] == 'Y') );

   /* if the inject file is open, then close it */
   if ( FileId != NULL )
   {
      fclose( FileId );
   }

   printf("Complete\n");
   return EXIT_SUCCESS;

}  /* End of main() */


/* EOF */
