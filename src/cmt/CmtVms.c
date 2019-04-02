/*******************************************************************************
** Module Name:
**    CmtVms.c
**
** Purpose:
**    Vms specific functions.
**
** Description:
**    The module of the CMT which interfaces only to Vms to extract system 
**    usage statistics. 
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.1 1-Nov-2000.
**
** Author(s):
**    Steve Foale (sxf)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2002
**
** Version:
**    $Id: CmtVms.c,v 0.4 2006/03/21 11:21:56 sxf Exp $
**
** History:
**    $Log: CmtVms.c,v $
**    Revision 0.4  2006/03/21 11:21:56  sxf
**    Added NTP lock functionaity for QNX6, Linux and VMS.
**
**    Revision 0.5  2006/03/15 16:32:53  sxf
**    Check for NTP lock once per minute rather than once per second
**    to prevent excessive disk accesses.
**
**    Revision 0.4  2006/01/27 11:17:57  sxf
**    Implement NTP lock  chack for VMS.
**
**    Revision 0.3  2004/01/13 14:22:08  mkb
**    Remove potential memory leak by replacing malloc with compile time
**    assignment.
**
**    Revision 0.2  2002/08/01 15:45:19  sxf
**    Added dummy iCmtDetectHardware function.
**
**    Revision 0.1  2002/08/01 10:48:51  sxf
**    Initial version (taken from CmtInterface.c and CmtHardware.c).
**
**
*******************************************************************************/

#ifdef I_CMT_OS_VMS

/*
** Use pre-defined structures for VMS system service calls (about time too!)
*/
#define __NEW_STARLET 1

/*
** Standard non-specific header files
*/
#include <stdio.h>                   
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <lnmdef.h>

/*
** VMS specific header files
*/
#include <descrip.h>	/* String descriptors */
#include <starlet.h>	/* System service routines */
#include <dcdef.h>      /* Disk storage definitions */
#include <ssdef.h>      /* System service definitions */
#include <dvidef.h>     /* Device information definitions */
#include <stsdef.h>     /* Status returned definitions */
#include <syidef.h>     /* Systsem information definitions */
#include <dvsdef.h>     /* Device class definitions */
#include <gen64def.h>   /* 64 bit data types */
#include <iosbdef.h>    /* IO status block definitions */
#include <iledef.h>     /* Item list definitions */


/*
** TTL sepcific includes
*/
#include "TtlSystem.h"
#include "Mcp.h"
#include "Sdb.h"
#include "Tim.h"
#include "Log.h"
#include "Cmt.h"      

/*
** Local includes
*/
#include "CmtPrivate.h"
#include "CmtCpuVms.h"



/*
** References to global system variables
*/
#ifdef __DECC
#  pragma message save
#  pragma message disable (GLOBALEXT)
#endif
globalref long  SMP$GL_ACTIVE_CPUS;
globalref $CPU *SMP$GL_CPU_DATA[1];
#ifdef __DECC
#  pragma message restore
#endif

/*
** Declare module specific variables
*/
static unsigned long int mCmtCpuCount = 1; /* There should be at least 1 CPU */
static unsigned long int mCmtBusyTime;
static unsigned long int mCmtIdleTime;

/*
** Module specific function prototypes
*/
int  mCmtCpuLoadVMS( void );

/*******************************************************************************
** Function Name:
**    iCmtDetectHardware
**
** Purpose:
**    Dummy function - no hardware to detect on VMS system.
**
** Description:
**    See above.
**
** Return Type:
**    int
**        Returns TRUE if supported hardware found else returns FALSE.  
**
** Arguments:
**    None
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int iCmtDetectHardware( void )
{

   return FALSE;

}

/*******************************************************************************
** Function Name:
**    iCmtGetVmsCpuUsed
**
** Purpose:
**    Get the CPU usage information from the VMS system.
**
** Description:
**    This function is determines CPU load by summing Null processes
**    CPU usage times.
**    The inital returned value is always 50%. 
**
** Return Type:
**    Status_t     
**       Return status (SYS_NOMINAL).
**
** Arguments:
**    Int32_t *CpuUsedPtr
**       Pointer to variable to return percentage of CPU use (millipercent).
**    Int32_t *NumRet
**       Number of valid parameters returned.
**
** Author(s):
**    Steve Foale (sxf)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iCmtGetVmsCpuUsed( Int32_t *CpuUsedPtr, Int32_t *NumRet )
{

   unsigned long int Busyness;           /* CPU busy ticks */     
   unsigned long int Idleness;           /* CPU idle ticks */ 
   unsigned long int Completeness;       /* CPU total ticks */

   unsigned long int BusyTime;           /* Time spent busy */
   unsigned long int IdleTime;           /* Time spent idle */

   Status_t Status = SYS_NOMINAL;        /* Returned status */

   /*
   ** Get the CPU load - NB the first reading will be average since startup
   */
   BusyTime = mCmtBusyTime;
   IdleTime = mCmtIdleTime;
   if ( $VMS_STATUS_SUCCESS( sys$cmkrnl( &mCmtCpuLoadVMS, NULL ) ) )
   {
      /*
      ** Calculate the CPU load
      */
      Busyness = ( mCmtBusyTime - BusyTime ) / mCmtCpuCount;
      Idleness = ( mCmtIdleTime - IdleTime ) / mCmtCpuCount;
      Completeness = Busyness + Idleness;

      if ( Completeness )
      {
         *CpuUsedPtr = (Busyness * I_CMT_MILLIPERCENT) / Completeness;
      }
      else
      {
         *CpuUsedPtr = 0;
      }

      *NumRet = 1;

   }
   else
   {
      Status = E_CMT_NO_OSINFO;
      eLogWarning( Status, "Unable to determine CPU usage." );

      NumRet = 0;
   }

   return Status;

} /* End of iCmtGetVmsCpuUsed */

/*******************************************************************************
** Function Name:
**    mCmtCpuLoadVMS
**
** Purpose:
**    Get the computer status information from the VMS system.
**
** Description:
**    This function is called in kernel mode to access the VMS SMP 
**    variables that hold the CPU usages statistics.
**
** Return Type:
**    int    
**       Returns the VMS completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/


int mCmtCpuLoadVMS( void )
{
#define CPU_ACTIVE       1     /* Shows CPU states is active */
#define BIT_SHIFT_RIGHT  1     /* Single bit right shift     */

/* Indices into CPU state information */
#define KERN_TIME     0 
#define EXEC_TIME     2
#define SUPR_TIME     4
#define USER_TIME     6
#define INTR_TIME     8
#define SPIN_TIME    10
#define IDLE_TIME    12

   register i;            /* Index into CPU data array    */
   register Cpu;          /* Bit mask of active CPU's     */
 
   long *Smp$GlKernelPtr; /* Pointer to system space holding SMP data */

   mCmtCpuCount = 0;      /* Active CPU's found */ 

   /* 
   ** For every active CPU on the system gather the CPU stats 
   */
   for( Cpu = SMP$GL_ACTIVE_CPUS, i = 0; Cpu; i++, Cpu >>= BIT_SHIFT_RIGHT )
   {
      /* If CPU active then get stats */
      if ( Cpu & CPU_ACTIVE )
      {
         /* 
         ** Get the pointer to the CPU kernel data
         */
         Smp$GlKernelPtr = (SMP$GL_CPU_DATA[ i ])->cpu$q_kernel;

         /* 
         ** Count the active CPU's 
         */
         mCmtCpuCount++;

         /* 
         ** Busy time is everything that isn't idle time 
         */
         mCmtBusyTime =   Smp$GlKernelPtr[ KERN_TIME ]
                        + Smp$GlKernelPtr[ EXEC_TIME ]
                        + Smp$GlKernelPtr[ SUPR_TIME ]
                        + Smp$GlKernelPtr[ USER_TIME ]
        /*                Exclude interrupt mode time  */
        /*              + Smp$GlKernelPtr[ INTR_TIME ] */  
                        + Smp$GlKernelPtr[ SPIN_TIME ];

         mCmtIdleTime = Smp$GlKernelPtr[ IDLE_TIME ];
      }
   }

   /* 
   ** Kernel mode call must return a VMS status code so return success
   */  
   return( SS$_NORMAL );

} /* End of mGetCpuLoadVMS */

/*******************************************************************************
** Function Name:
**    iCmtGetVmsDiskFree
**
** Purpose:
**    Get the disk free information from the VMS system.
**
** Description:
**    This function determines disk usage of up to three disks.
**
** Return Type:
**    Status_t     
**       Return status (SYS_NOMINAL).
**
** Arguments:
**    Int32_t *Disk1FreePtr
**       Pointer to variable to return percentage of disk 1 free (millipercent).
**    Int32_t *Disk2FreePtr
**       Pointer to variable to return percentage of disk 2 free (millipercent).
**    Int32_t *Disk3FreePtr
**       Pointer to variable to return percentage of disk 3 free (millipercent).
**    Int32_t *Disk4FreePtr
**       Pointer to variable to return percentage of disk 4 free (millipercent).
**    Int32_t *NumRet
**       Number of valid parameters returned.
**
** Author(s):
**    Steve Foale (sxf)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iCmtGetVmsDiskFree( Int32_t *Disk1FreePtr,
                             Int32_t *Disk2FreePtr, 
                             Int32_t *Disk3FreePtr,
                             Int32_t *Disk4FreePtr,
                             Int32_t *NumRet )
{

#define   M_CMT_DEV_NAME_LEN 16  /* Max length of a VMS disk device */
                     
   unsigned long  int FreeBlk;   /* Free block on disk */
   unsigned long  int BlkSize;   /* Size of a single block */
   unsigned long  int TotBlk;    /* Total number of blocks */
   unsigned long  int MntCnt;    /* Volume mount count */
   unsigned long  int FreePage;  /* Free pagefile pages */
   unsigned short int RetLen;    /* Length of returned data item */

   long int TotPage;                     /* Total pagefile pages */
   long int PageSize;                    /* Size of a pagefile page */
   long int DevClass = DC$_DISK;         /* VMS Device class */
   long int DevType  = DT$_GENERIC_DK;   /* VMS device type */ 

   /* 
   ** Variable holding extracted system information
   ** NB Variables wil percent suffix are actually milliPercent
   */
   unsigned int DiskFreePercent;        
   unsigned int DiskUsedPercent;         
   unsigned int DiskFreeKByte;
   unsigned int DiskUsedKByte;

   char   DevNameStr[M_CMT_DEV_NAME_LEN];

   unsigned short DevNameLen;    /* Length of disk device name */

   Status_t Status;              /* Returned status */
   
   struct _generic_64 Context;   /* Device search context */

   eCmtDataId_t DataIdDisk;      /* Data ID for disks */

   IOSB Iosb;                    /* IO operation completion status block */

   int i;                        /* Loop counter */

   Status = SYS_NOMINAL;         /* Initialise status to OK. */

   /* 
   ** Disk device name VMS descriptor 
   */
   struct dsc$descriptor DevName = { M_CMT_DEV_NAME_LEN, 
                                     DSC$K_DTYPE_T,
                                     DSC$K_CLASS_S,
                                     DevNameStr };

   /* 
   ** Disk device type VMS descriptor
   */
   $DESCRIPTOR( DevNameSearch, I_CMT_VMS_DISKTYPE );

   /* 
   ** Declare and initialise the VMS system service item lists 
   */
   ILE3 ItmBlkSize[] = {
                          {sizeof(BlkSize), DVI$_DEVBUFSIZ, &BlkSize, &RetLen},
                          {0, 0, NULL, NULL}
                       };

   ILE3 ItmFreeBlk[] = {
                          {sizeof(FreeBlk), DVI$_FREEBLOCKS,&FreeBlk, &RetLen},
                          {0, 0, NULL, NULL}
                       };

   ILE3 ItmTotBlk[]  = {
                          {sizeof(TotBlk), DVI$_MAXBLOCK, &TotBlk, &RetLen},
                          {0, 0, NULL, NULL}
                       };

   ILE3 ItmMntCnt[]  = {
                          {sizeof(TotBlk), DVI$_MOUNTCNT, &MntCnt, &RetLen},
                          {0, 0, NULL, NULL}
                       };
                            
   /* 
   ** Code starts here
   */

   /* 
   ** Initial status for this function is SYS_NOMINAL
   */
   Status = SYS_NOMINAL;

   /* 
   ** Zero the context information 
   */
   memset( &Context, 0, sizeof( Context ) );

   /* 
   ** Start at first disk
   */
   DataIdDisk = D_CMT_DISK1_FREE_PERCENT;

   /* 
   ** For each disk device found get the size/usage statistics
   */
   while( SS$_NORMAL == sys$device_scan( &DevName,
                                         &DevName.dsc$w_length,  
                                         &DevNameSearch,
                                          0,
                                         &Context ) )   
   {
      /*
      ** Zero the disk information
      */
      FreeBlk = TotBlk = BlkSize = MntCnt = 0;

      /* 
      ** Get the disk device statistics and calculate the disk usage
      ** All the system service calls to get disk stats must succeed 
      ** and the returned MntCnt and TotBlk counts must be non-zero  
      */
      if( $VMS_STATUS_SUCCESS( sys$getdviw( 0, 0, &DevName,
                                           &ItmFreeBlk, &Iosb, 0, 0, 0)) &&
          $VMS_STATUS_SUCCESS( sys$getdviw( 0, 0, &DevName,
                                           &ItmTotBlk,  &Iosb, 0, 0, 0)) &&
          $VMS_STATUS_SUCCESS( sys$getdviw( 0, 0, &DevName,
                                           &ItmBlkSize, &Iosb, 0, 0, 0)) &&
          $VMS_STATUS_SUCCESS( sys$getdviw( 0, 0, &DevName,
                                           &ItmMntCnt,  &Iosb, 0, 0, 0)) &&
          ( MntCnt > 0) && (TotBlk > 0)                                     ) 
      {
         /*
         ** Calculate the disk usage in kilo-bytes
         */
         DiskFreeKByte = ( FreeBlk * BlkSize ) / I_CMT_KILOBYTE;
         DiskUsedKByte = ( TotBlk  * BlkSize ) / I_CMT_KILOBYTE - DiskFreeKByte;

         /*
         ** Calculate the disk usage in milli-percentage 
         */
         DiskFreePercent = ( FreeBlk  * I_CMT_MILLIPERCENT) / TotBlk;
         DiskUsedPercent = ((TotBlk - FreeBlk) * I_CMT_MILLIPERCENT ) / TotBlk;

         /*
         ** Save the amount of free disk space in the appropriate return variable.
         */

         switch ( DataIdDisk )
         {
            case D_CMT_DISK1_FREE_PERCENT:
         
               *Disk1FreePtr = DiskFreePercent;
               *NumRet = 1;
               break;

            case D_CMT_DISK2_FREE_PERCENT:
         
               *Disk2FreePtr = DiskFreePercent;
               *NumRet = 2;
               break;

            case D_CMT_DISK3_FREE_PERCENT:
          
               *Disk3FreePtr = DiskFreePercent;
               *NumRet = 3;
               break;

            case D_CMT_DISK4_FREE_PERCENT:
          
               *Disk4FreePtr = DiskFreePercent;
               *NumRet = 4;
               break;

            default:

               Status = E_CMT_NO_OSINFO;
               eLogWarning( Status, "Found more than 3 disk drives" );
               break;
         }
                  
         /*
         ** Increment the DataId and the disk count
         */ 
         if ( DataIdDisk++ > I_CMT_DISK_LAST ) break;        
      }
   }
 
   return Status;

}

/*******************************************************************************
** Function Name:
**    iCmtGetVmsMemoryFree
**
** Purpose:
**    Get the memory free information from the VMS system.
**
** Description:
**    This function determines memory usage of the host VMS system.
**
** Return Type:
**    Status_t     
**       Return status (SYS_NOMINAL).
**
** Arguments:
**    Int32_t *MemFreePtr
**       Pointer to variable to return percentage of disk 1 free (millipercent).
**    Int32_t *NumRet
**       Number of valid parameters returned.
**
** Author(s):
**    Steve Foale (sxf)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iCmtGetVmsMemoryFree( Int32_t *MemFreePtr,
                               Int32_t *NumRet )
{
   unsigned long  int FreePage;  /* Free pagefile pages */
   long int TotPage;             /* Total pagefile pages */
   long int PageSize;            /* Size of a pagefile page */
   unsigned int MemFreePercent;  
   unsigned int MemUsedPercent;   
   unsigned int MemFreeKByte;     
   unsigned int MemUsedKByte;
   unsigned short int RetLen;    /* Length of returned data item */
   IOSB Iosb;                    /* IO operation completion status block */

   Status_t Status;              /* Returned status */

   /* 
   ** Declare and initialise the VMS system service item lists 
   */
   ILE3 ItmPageSize[]= {
                           {sizeof(PageSize),SYI$_PAGE_SIZE,&PageSize,&RetLen},
                           {0, 0, NULL, NULL}
                        };
                           
   ILE3 ItmFreePage[]= {
                        {sizeof(FreePage),SYI$_PAGEFILE_FREE,&FreePage,&RetLen},
                        {0, 0, NULL, NULL}
                       };

   ILE3 ItmTotPage[] = {
                          {sizeof(TotPage),SYI$_PAGEFILE_PAGE,&TotPage,&RetLen},
                          {0, 0, NULL, NULL }
                      };

   /* 
   ** Code starts here
   */

   /* 
   ** Initial status for this function is SYS_NOMINAL
   */
   Status = SYS_NOMINAL;

   /* 
   ** Get the memory statistics and ensure returned Total Pages is not zero 
   ** NB TotPage is set by the system service call        
   */
   if ($VMS_STATUS_SUCCESS(sys$getsyiw( 0, 0, 0, &ItmFreePage,&Iosb, 0, 0 ))&&
       $VMS_STATUS_SUCCESS(sys$getsyiw( 0, 0, 0, &ItmTotPage, &Iosb, 0, 0 ))&&
       $VMS_STATUS_SUCCESS(sys$getsyiw( 0, 0, 0, &ItmPageSize,&Iosb, 0, 0 ))&& 
       TotPage > 0                                                            )
   {
      /* 
      ** Calculate the memory usage
      */
      MemFreeKByte   = ( FreePage * PageSize / I_CMT_KILOBYTE );
 
      MemUsedKByte   = ( TotPage  * PageSize / I_CMT_KILOBYTE ) - MemFreeKByte;
      MemFreePercent = ( FreePage * I_CMT_MILLIPERCENT ) / TotPage;
      MemUsedPercent = ((TotPage  - FreePage) * I_CMT_MILLIPERCENT ) / TotPage;

      /* 
      ** Load the value and timestamp into the data area, SDB paramters
      */
      *MemFreePtr = MemFreePercent;
      *NumRet     = 1;

   }
   else
   {
      Status = E_CMT_NO_OSINFO;
      eLogWarning( Status, "Failed to get memory stats." );
      *MemFreePtr = 0;
      *NumRet     = 0;

   }

   return Status;

}

/*******************************************************************************
** Function Name:
**    iCmtGetVmsTemperature
**
** Purpose:
**    Get the temperature information from the VMS system.
**
** Description:
**    Not currently implemented. Return zeros for temperatures and number
**    of valid parameters.
**
** Return Type:
**    Status_t     
**       Return status (SYS_NOMINAL).
**
** Arguments:
**    Int32_t *Temp1Ptr
**       Pointer to variable to return temperature 1 (millicelcius).
**    Int32_t *Temp2Ptr
**       Pointer to variable to return temperature 2 (millicelcius).
**    Int32_t *Temp3Ptr
**       Pointer to variable to return temperature 3 (millicelcius).
**    Int32_t *NumRet
**       Number of valid parameters returned.
**
** Author(s):
**    Steve Foale (sxf)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iCmtGetVmsTemperature( Int32_t *Temp1Ptr,
                                Int32_t *Temp2Ptr,
                                Int32_t *Temp3Ptr,
                                Int32_t *NumRet )
{

  
   *Temp1Ptr = 0;
   *Temp2Ptr = 0;
   *Temp3Ptr = 0;
   *NumRet   = 0;

   return SYS_NOMINAL;

}

/*******************************************************************************
** Function Name:
**    iCmtGetVmsFanSpeed
**
** Purpose:
**    Get the fan speed information from the VMS system.
**
** Description:
**    Not currently implemented. Return zeros for fan speeds and number
**    of valid parameters.
**
** Return Type:
**    Status_t     
**       Return status (SYS_NOMINAL).
**
** Arguments:
**    Int32_t *Fan1Ptr
**       Pointer to variable to return fan speed 1 (RPM).
**    Int32_t *Fan2Ptr
**       Pointer to variable to return fan speed 2 (RPM).
**    Int32_t *Fan3Ptr
**       Pointer to variable to return fan speed 3 (RPM).
**    Int32_t *NumRet
**       Number of valid parameters returned.
**
** Author(s):
**    Steve Foale (sxf)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iCmtGetVmsFanSpeed( Int32_t *Fan1Ptr,
                             Int32_t *Fan2Ptr,
                             Int32_t *Fan3Ptr,
                             Int32_t *NumRet )
{

   *Fan1Ptr = 0;
   *Fan2Ptr = 0;
   *Fan3Ptr = 0;
   *NumRet  = 0;

   return SYS_NOMINAL;

}

/*******************************************************************************
** Function Name:
**    iCmtGetVmsNtpLocked
**
** Purpose:
**    Get the ntp lock state
**
** Description:
**
** Return Type:
**    Status_t     
**       Return status (SYS_NOMINAL).
**
** Arguments:
**    Bool_t *Locked
**       Pointer to variable to return TRUE/FALSE for locked/unlocked
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

Status_t iCmtGetVmsNtpLocked( Bool_t *Locked )
{

   Status_t      Status;          /* TCS return status.          */
   int           SS_Status;       /* VMS return status.          */
   char          SymText[LNM$C_NAMLENGTH];
   static eTtlTime_t TimeStopped; /* Time of failed lock.    */
   eTtlTime_t    TimeNow;
   unsigned int  NtpLockedLen;
   static int    Count = 60;
   static Bool_t LockedMemory = FALSE;

   /* Define constant descriptors */
   $DESCRIPTOR(CheckNtp,         "UTILS_DIR:CHECK_NTP.COM");
   $DESCRIPTOR(NtpResultLogical, "NTP$LOCKED");
   $DESCRIPTOR(NtpResultTable,   "LNM$GROUP" );
   $DESCRIPTOR(NtpResultText,    SymText     );

   Count ++;

   /* 
   ** Only determine NTP lock status every minute.
   */
   if ( Count >= 60 )
   {
      /*
      ** Reset counter.
      */
      Count = 0;

      /* Spawn script to check NTP lock. */
      SS_Status = LIB$SPAWN((struct dsc$descriptor *) NULL, &CheckNtp );

      if (SS_Status != SS$_NORMAL)
      {
         eLogNotice( 0, "Failed to spawn NTP check script." ); 
         return E_CMT_NO_NTPINFO;
      }                                            

      /* Decode logical reported by NTP lock script. */
      SS_Status = LIB$GET_LOGICAL 
        (&NtpResultLogical, 
         &NtpResultText, 
         &NtpLockedLen, 
         &NtpResultTable);
      if (SS_Status != SS$_NORMAL)
      {
         eLogNotice( 0, "Unable to decode NTP lock logical." ); 
         return E_CMT_NO_NTPINFO;
      }                                            

      if (strstr(NtpResultText.dsc$a_pointer, "TRUE") != NULL)
      {
         LockedMemory = TRUE;
      }
      else 
      {
         LockedMemory = FALSE;
      }

   }

   *Locked = LockedMemory;

   return SYS_NOMINAL;

}
#endif  /* #ifdef __VMS */
