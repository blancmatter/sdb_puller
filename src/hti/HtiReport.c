
/*******************************************************************************
**  File Name: 
**  HtiReport.c
** 
** 
** Purpose:
**  	To take an Id and return all data associated with that Id
** 
** 
** 
** Description: 
**    Main Program File.
** 
** 
** 
** 
** Author(s):
**    Daniel W. Smith (dws)
** 
** 
** Copyright (c) Telescope Technologies Limited (TTL), 2002
** 
** 
** 
** Version:
** $Id: HtiReport.c,v 0.11 2007/08/30 08:34:27 mjf Exp $
** 
** History:
** $Log: HtiReport.c,v $
** Revision 0.11  2007/08/30 08:34:27  mjf
** General tidy-up, including change of name of command-line arguments.
**
** Revision 0.10  2005/06/20 13:29:34  mjf
** Tidied to aid porting to Linux - no functional changes.
**
** Revision 0.9  2005/06/17 09:26:34  mjf
** Aim to open HTI tables in directory '/ram' and default to '/ttl/sw/etc'
** if not located there.
**
** Revision 0.8  2004/04/06 10:48:25  sxf
** Ported to QNX v6.
**
** Revision 0.7  2004/01/15 16:35:03  sxf
** Removed unnecessary header file.
**
** Revision 0.6  2002/10/31 15:22:43  dws
** Fxied so that user specfied values are read in for oid and
** unit lookups
**
** Revision 0.5  2002/10/21 09:00:29  dws
** updated functions call to Hti.lib to be inline with r0.10
** Changed Clu options so that you no longer specfiy a type
** and a value. changed output.
**
** Revision 0.4  2002/10/18 09:00:56  dws
** Fixed description for type switch.
**
** Revision 0.3  2002/10/17 13:31:13  dws
** Fixed comments
**
** Revision 0.2  2002/10/17 13:30:21  dws
** Fixed Rcs tags
**
** Revision 0.1  2002/10/17 13:28:08  dws
** initial working revision
**
** 
** 
*******************************************************************************/

/*
** Compiler include files
*/
#include <stdio.h>
#include <unistd.h>
#include <string.h>



/* 
** System Include files
*/


#include "TtlSystem.h"
#include "Cfu.h"
#include "Clu.h"
#include "Log.h"
#include "Cil.h"
#include "Hti.h"
/*
** Local Functions
*/

Status_t iHtiRInitClu(int argc, char *argv[]);
Status_t iHtiRInit(void);
Status_t  mHtiRDisplay(char *Label, char *Description);
/* 
** Clu and related settings
*/

#define M_HTREPORT_PROGRAM_TITLE  "HtiReport"
#define M_HTREPORT_PROGRAM_ABOUT  "HTI Report utility"
#define M_HTREPORT_YEAR           "2002-07"
#define M_HTREPORT_RELEASE_DATE   "29 August 2007"
#define M_HTREPORT_MAJOR_VERSION  0 
#define M_HTREPORT_MINOR_VERSION  3

/* defaults for common runtime parameters */

#define M_HTREPORT_DFLT_QUIET     FALSE
#define M_HTREPORT_DFLT_VERBOSE   !( M_HTREPORT_DFLT_QUIET )
#define M_HTREPORT_DFLT_SYSLOG    TRUE
#define M_HTREPORT_DFLT_DEBUG     E_LOG_NOTICE
#define M_HTREPORT_DFLT_PRIORITY  10
#define M_HTREPORT_DFLT_HELP      FALSE

/* define custom arguements for CLU */

enum mHtiRArguements_e{
   M_HTREPORT_ARG_STATUS = 0,
   M_HTREPORT_ARG_UNIT,
   M_HTREPORT_ARG_OID,
   M_HTREPORT_ARG_CILNAME,
   M_HTREPORT_ARG_CNT
};


/* custom arguments */



#define M_HTREPORT_SWITCH_STATUS    "status <value>"
#define M_HTREPORT_EXPL_STATUS    "Status code to lookup"
#define M_HTREPORT_SWITCH_OID    "unit <value>"
#define M_HTREPORT_EXPL_OID    "Value of unit to lookup" 
#define M_HTREPORT_SWITCH_UNIT    "datum <value>"
#define M_HTREPORT_EXPL_UNIT    "Value of datum to lookup (uses '-cil')" 
#define M_HTREPORT_SWITCH_CILNAME    "cilrep <CIL ID>"
#define M_HTREPORT_EXPL_CILNAME    "CIL ID of reporter for datum lookup"

/* Number of characters needed to be matched for custom arguements */
#define M_HTREPORT_CUSTOM_MATCH 3

/* setup of CLU arguements */
eCluArgSpec_t eCluCustomArg[]
 = { 
{ M_HTREPORT_SWITCH_STATUS, M_HTREPORT_CUSTOM_MATCH, M_HTREPORT_EXPL_STATUS,
  FALSE, NULL},
{ M_HTREPORT_SWITCH_OID, M_HTREPORT_CUSTOM_MATCH, M_HTREPORT_EXPL_OID,
  FALSE, NULL},
{ M_HTREPORT_SWITCH_UNIT,  M_HTREPORT_CUSTOM_MATCH, M_HTREPORT_EXPL_UNIT,
  FALSE, NULL},   
{ M_HTREPORT_SWITCH_CILNAME, M_HTREPORT_CUSTOM_MATCH,
  M_HTREPORT_EXPL_CILNAME,  FALSE, NULL},         
{ E_CLU_EOL,         0,                  E_CLU_EOL,       FALSE, NULL }
};


/*
** Program Specified defines 
*/

/* Used to identify what type of lookup the program is doing */
 typedef enum mHtiRLookupType_e{
      M_HTREPORT_LOOKUP_UNIT =0,
      M_HTREPORT_LOOKUP_OID,
      M_HTREPORT_LOOKUP_STATUS,
      M_HTREPORT_LOOKUP_EOL
      }mHtReportType_t;


#define M_HTREPORT_STRING_LENGTH  256 /* Maximum length of strings in
                                         application */
#ifdef E_WFL_OS_QNX4
#define M_HTREPORT_CIL_MAP "/opt/ttl/etc/Cil.map" /* Cil map to use */
#else
#define M_HTREPORT_CIL_MAP "/ttl/sw/etc/Cil.map" /* Cil map to use */
#endif

/* 
** Global variables 
*/
mHtReportType_t mLookupMode = M_HTREPORT_LOOKUP_EOL; /* stores what type
                     of lookup has been specified defaults to nothing */

Int32_t mHtiReportValue; /* Value requested to be looked up */
Int32_t mHtiReportCilId;  /* Cilid required when an oid lookup is 
                            performed */
char  mHtiReportCilname[M_HTREPORT_STRING_LENGTH]; /* Contains the Cilname specifed */
/*******************************************************************************
** Function Name :
**    main
**
** Purpose :
**    Main function for the module
**
** Description :
**    Main function of HtiReport program
**
** Return Type:
**    int
**
**   
** Arguments :
**    int  argc    (IN)
**          Number of arguments passed in from the command line.
**
**    char argv[]  (IN)
**          Array of (argc) strings representing the arguments passed 
**          in from the command line.
**
** Authors :
**    Daniel W. Smith(dws)
**
**
*******************************************************************************/ 
int main(int argc,char *argv[])
{
   Status_t Status;
    
   
   

   /* setup HtiReport by parsing commandline using CLU. */

   Status = iHtiRInitClu( argc, argv );
    if( Status != SYS_NOMINAL )
    {
       eLogReport(E_LOG_CRIT, Status, "Failed to setup CLU exiting");
       exit(EXIT_FAILURE);
    }


   /* Initilaise the HtiReport settings */

   Status=iHtiRInit();
   if( Status != SYS_NOMINAL )
   {
      eLogCrit(Status, "Failed to Setup program parameters exiting");
      exit(EXIT_FAILURE);
   }

   iHtiRPerformLookup(); /* Perform the Hti lookup */

   exit(EXIT_SUCCESS);

return(SYS_NOMINAL);  
}

/******************************************************************************
**  Function Name :
**      iHtiRInitClu
**
**
**  Purpose :
**      Initialise the CLU data.
**
**  Description :
**      Call the various functions required to set up Clu.
**
**  Return Type :
**      Status_t
**
**  Arguments :
**      int   argc   (in)
**          Number of command line arguments - passed into eCluSetup().
**
**      char *argv[] (in)
**          Array of (argc) text strings containing command line
**          arguments - passed into eCluSetup().
**
**  Authors :
**       Daniel W. Smith(dws)
**
**
******************************************************************************/
Status_t iHtiRInitClu(int argc, char *argv[])
{
    Status_t Status;


    /*
    ** Set up all data required by the CLU (Command Line Utilities) package.
    */

    eCluProgNamePtr       = M_HTREPORT_PROGRAM_TITLE;
    eCluProgAboutPtr      = M_HTREPORT_PROGRAM_ABOUT;
    eCluReleaseDatePtr    = M_HTREPORT_RELEASE_DATE;
    eCluYearPtr           = M_HTREPORT_YEAR;
    eCluMajorVer          = M_HTREPORT_MAJOR_VERSION;
    eCluMinorVer          = M_HTREPORT_MINOR_VERSION;

    eCluCommon.Quiet      = M_HTREPORT_DFLT_QUIET;
    eCluCommon.Verbose    = M_HTREPORT_DFLT_VERBOSE;
    eCluCommon.Syslog     = M_HTREPORT_DFLT_SYSLOG;
    eCluCommon.DebugLevel = M_HTREPORT_DFLT_DEBUG;
    eCluCommon.Priority   = M_HTREPORT_DFLT_PRIORITY;
    eCluCommon.Help       = M_HTREPORT_DFLT_HELP;

    /*
    ** Handle the command line arguments using the CLU package calls
    */

    Status = eCluSetup(
                argc,
                argv,
                (E_CLU_IGN_CIL_NAME | E_CLU_IGN_CIL_MAP | E_CLU_IGN_CONFIGFILE)
             );

    if (Status != SYS_NOMINAL)
    {
        fprintf(
            stderr,
            "Failed to setup command line argument parsing"
             );
        return(SYS_ERROR_FATAL);
    }

    
    Status = eCluParseArgs(NULL);
    if (Status != SYS_NOMINAL)
    {
        fprintf(
            stderr,
            "Unable to parse arguments"
            );
        return(SYS_ERROR_FATAL);
    }
    else
    {
       /* If help-only requested, go no further */
       if ( eCluCommon.Help == TRUE )
       {
          exit ( 0 );
       }
    }
    

    /* Display the sign-on text for this package. */

    Status = eCluSignOn (NULL, M_HTREPORT_PROGRAM_TITLE);
    if (Status != SYS_NOMINAL)
    {
        eCluShowUsage(
            NULL,
            ": <%x> Unable to sign-on program"
              );
        return(SYS_ERROR_FATAL);
    }
   /* If help is requested from the command line then display help and exit */
   
   if (eCluCommonArgExists(E_CLU_ARG_HELP) == E_CLU_ARG_SUPPLIED)
   {
        exit(EXIT_SUCCESS);
   }
   

    return(Status);
}

/*******************************************************************************
** Function Name :
**    iHtiRInit
**
** Purpose :
**    To setup and retrieve program parameters from the command line.
**
** Description :
**    Set program parameters
**
** Return Type:
**    Status_t
**
**   
**
** Authors :
**    Daniel W. Smith(dws)
**
**
*******************************************************************************/ 
Status_t iHtiRInit(void)
{

  Status_t Status = SYS_NOMINAL;        /* Function returns */




   /* Check if Status lookup  is specifed */
   if (eCluCustomArgExists(M_HTREPORT_ARG_STATUS) == E_CLU_ARG_SUPPLIED)
   {  
      mLookupMode =M_HTREPORT_LOOKUP_STATUS; /* set lookup type as unit */
      
      /* The value is supplied as a string convert it to integer */
      mHtiReportValue = strtol(eCluGetCustomParam( M_HTREPORT_ARG_STATUS), NULL, 
                                                   0);
   }  
   /* Check if OID lookup  is specifed */
   else if(eCluCustomArgExists(M_HTREPORT_ARG_OID) == E_CLU_ARG_SUPPLIED)
   {

         /* The value is supplied as a string convert it to integer */
         mHtiReportValue = strtol(eCluGetCustomParam( M_HTREPORT_ARG_OID), NULL, 
                                                    0);
         mLookupMode =M_HTREPORT_LOOKUP_OID; /* set lookup type as OID */

         /* If an Oid lookup is going to occur a Cilname must be supplied */
         if(eCluCustomArgExists(M_HTREPORT_ARG_CILNAME) == E_CLU_ARG_SUPPLIED)
         {  

           /* Check length of supplied Cilname */
           if(strlen( eCluGetCustomParam( M_HTREPORT_ARG_CILNAME ) )<= 
                                          M_HTREPORT_STRING_LENGTH)
           {
             /* Copy the Supplied cilname to module scope variable */
             strcpy(mHtiReportCilname, 
                    eCluGetCustomParam( M_HTREPORT_ARG_CILNAME));
           }
           else
           {
               /* Cil name exceeds string lenght */
               eLogCrit(0,"Cil name too large");
               return(SYS_ERROR_FATAL);
           }

           /* Resolve Cilname  to CilId */
           Status = eCilLookup( M_HTREPORT_CIL_MAP, mHtiReportCilname,
                                                    &mHtiReportCilId);
           if(Status != SYS_NOMINAL)
           {
               /* Error looking up Cil name */
               eLogCrit(0,"Failure to lookup Cil name");
               return(SYS_ERROR_FATAL); 
           }
         }
         else
         {
            /* Cilname not supplied */
            eLogCrit(0,"Cilname is required");
            return(SYS_ERROR_FATAL);
         }

 }  
 /* Check if Status lookup is specifed */
 else if(eCluCustomArgExists(M_HTREPORT_ARG_UNIT) == E_CLU_ARG_SUPPLIED)
 {
         /* The value is supplied as a string convert it to integer */
         mHtiReportValue = strtol(eCluGetCustomParam( M_HTREPORT_ARG_UNIT), NULL, 
                                                   0);
         mLookupMode =M_HTREPORT_LOOKUP_UNIT; /* set lookup type as Status */
 }  
 else
 {
         /* invalid lookup type specified */
         eLogCrit(0,"No lookup specified");
         return(SYS_ERROR_FATAL);
 }



   
/* Program succesfully initialised */   
return(Status);
}


/*******************************************************************************
** Function Name: 
**    iHtiRPerformLookup
** 
** 
** Purpose:
**     To carry out the command line specifed lookup using Hti.
** 
** 
** 
** Description: 
**     Switch is done on the specified look up method. For each method different
**     Hti calls are made to get the Description and label of the value specifed.
**     If an Oid type look up is being made then tbe cilname specfied is usde as
**     well.
** 
** Return Type:
**     Status_t 
** 
** 
** 
** Arguements:
** 
** 
** 
** 
** Author(s):
**     Daniel W. Smith
** 
*******************************************************************************/
Status_t iHtiRPerformLookup(void)
{
   Status_t Status =  SYS_NOMINAL;
   char Description[M_HTREPORT_STRING_LENGTH];
   char Label[M_HTREPORT_STRING_LENGTH];
   
   
   switch(mLookupMode)
   {
      case M_HTREPORT_LOOKUP_UNIT:
         {
            /* Check length of returned description */
            if(strlen(eHtiGetDescriptionString(eHtiBinUnitTableName(), 
                            E_CIL_SDB, mHtiReportValue, Description))
                            >M_HTREPORT_STRING_LENGTH )
            {
                eLogCrit(0,"Description too long");
                return(SYS_ERROR_FATAL);
            }
            
            /* Copy returned description to local variable */
            strcpy(Description,eHtiGetDescriptionString(
                                                     eHtiBinUnitTableName(), 
                                                     E_CIL_SDB,
                                                     mHtiReportValue,
                                                     Description
                                                     ));
            /* check length of returned label */
            if(strlen(eHtiGetUnitString(mHtiReportValue,
                       Label)) >M_HTREPORT_STRING_LENGTH )
            {

                eLogCrit(0,"Description too long");
                return(SYS_ERROR_FATAL);
            }

            /* Copy returned label to local variable */
            strcpy(Label, eHtiGetUnitString(mHtiReportValue, Label));

         }
         break;
     case M_HTREPORT_LOOKUP_OID:
         {

            /* Check length of returned description */
            if(strlen(eHtiGetOidDescriptionString(
                                                  mHtiReportCilId,
                                                  mHtiReportValue,
                                                  Description
                                                  )) 
                                    >  M_HTREPORT_STRING_LENGTH )
            {
                eLogCrit(0,"Description too long");
                return(SYS_ERROR_FATAL);
            }
            
            /* Copy returned description to local variable */
            strcpy(Description,eHtiGetOidDescriptionString(
                                                  mHtiReportCilId,
                                                  mHtiReportValue,
                                                  Description ) );
            /* check length of returned label */
            if(strlen(eHtiGetDataString(mHtiReportCilId, mHtiReportValue,
                       Label)) >M_HTREPORT_STRING_LENGTH )
            {

                eLogCrit(0,"Description too long");
                return(SYS_ERROR_FATAL);
            }

            /* Copy returned label to local variable */
            strcpy(Label, eHtiGetDataString(mHtiReportCilId,
                                            mHtiReportValue, Label));

         }
         break;
     case M_HTREPORT_LOOKUP_STATUS:
         {  

            /* Check length of returned description */
            if(strlen(eHtiGetStatusCodeDescriptionString(mHtiReportValue,
                       Description)) >M_HTREPORT_STRING_LENGTH )
            {
                eLogCrit(0,"Description too long");
                return(SYS_ERROR_FATAL);
            }
            
            /* Copy returned description to local variable */
            strcpy(Description,
            eHtiGetStatusCodeDescriptionString(mHtiReportValue, Description));

            /* check length of returned label */
            if(strlen(eHtiGetStatusCodeString(mHtiReportValue,
                       Label)) >M_HTREPORT_STRING_LENGTH )
            {

                eLogCrit(0,"Description too long");
                return(SYS_ERROR_FATAL);
            }

            /* Copy returned label to local variable */
            strcpy(Label, eHtiGetStatusCodeString(mHtiReportValue, Label));

         }
         break;
    }
    
    /* Display Data */
    mHtiRDisplay(Label, Description);

return(Status);
}



/*******************************************************************************
** Function Name: 
**    mHtiRDisplay
** 
** 
** Purpose:
**    To displayed retrieved Data.
** 
** 
** 
** Description: 
**    Output the retrieved data to the screen and system log. 
** 
** 
** 
** Return Type:
**    Status_t 
** 
** 
** 
** Arguements:
**     char *Label
**         The label for the retrieved description
**     char *Description 
**         The Description retrieved from the value specified
** 
** 
** Author(s):
**    Daniel W. Smith (dws)
** 
*******************************************************************************/
Status_t  mHtiRDisplay(char *Label, char *Description)
{
   
  /* Display what was being resolved */
   switch(mLookupMode)
   {
      case M_HTREPORT_LOOKUP_UNIT:
         {
           eLogNotice(0, "Unit Lookup (%d, 0x%02x)", 
                      mHtiReportValue, mHtiReportValue);
         }
         break;
     case M_HTREPORT_LOOKUP_OID:
         {
            eLogNotice(0, "Datum Lookup (%d, 0x%06x)",
                       mHtiReportValue, mHtiReportValue);
         }
         break;
     case M_HTREPORT_LOOKUP_STATUS:
         {  
           eLogNotice(0, "Status Code Lookup (%d, 0x%06x)",
                      mHtiReportValue, mHtiReportValue);
         }
         break;
    }
 
  
   
   /* If a cilname wasnt set it mean that it wasnt required so dont display it*/
   if( mHtiReportCilname[0] != '\0' )
   {
      eLogNotice(0, "CIL ID      : %s (0x%x)", 
                 mHtiReportCilname, mHtiReportCilId);
   }
 
   /* 
   ** If there is No label and NO description then show that no details were 
   ** found.
   */
   if ( (*Label == '\0') && (*Description == '\0') ) 
   {
      eLogNotice(0, "No Details found");
   }
   else
   {
      eLogNotice(0, "Label       : '%s'", Label);
      eLogNotice(0, "Description : '%s'", Description);
   }

   return(SYS_NOMINAL);
}

/*******************************************************************************
** End of File Name: HtiReport.c
*******************************************************************************/
