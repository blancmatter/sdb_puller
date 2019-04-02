/*******************************************************************************
** Module Name:
**    CmtInterface.c
**
** Purpose:
**    Interface to read system usage statistics - CPU, disk and memory.
**
** Description:
**    The module of the CMT which interfaces to the OS to extract system 
**    usage statistics. 
**    QNX Linux and VMS operating systems are supported by this module.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.1 1-Nov-2000.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: CmtInterface.c,v 1.12 2006/10/17 13:57:45 sxf Exp $
**
** History:
**    $Log: CmtInterface.c,v $
**    Revision 1.12  2006/10/17 13:57:45  sxf
**    Report changes of NTP lock state at WARNING level.
**
**    Revision 1.11  2006/03/21 11:21:56  sxf
**    Added NTP lock functionaity for QNX6, Linux and VMS.
**
**    Revision 1.10  2005/07/14 09:20:46  sxf
**    Basic monitoring functionality now implemented. Monitoring of:
**       o CPU load
**       o Disk space
**       o Used memory
**
**    Revision 1.9  2004/01/15 15:36:44  sxf
**    Ported for Neutrino.
**
**    Revision 1.8  2003/12/03 10:13:33  sxf
**    Corrected reporting of disk 4.
**
**    Revision 1.7  2003/12/02 14:48:36  sxf
**    Change preprocessor directive so that old linux code is not compiled.
**    It remains in the file as reference.
**
**    Revision 1.6  2003/02/06 12:00:51  sxf
**    Check disk space and set warning flag if necessary.
**
**    Revision 1.5  2002/08/01 10:47:16  sxf
**    Now arranged so that each parameter monitored calls its own
**    system specific handling function.
**
**    Revision 1.4  2002/01/18 10:10:50  mjf
**    Checking in of development left by ASP.
**
**    Revision 1.3  2001/03/16 21:29:58  asp
**    Added hardware monitoring.
**    Unused data now identified by INVALID_UNITS.
**    Various bug-fixes applied.
**
**    Revision 1.2  2001/01/04 11:21:46  asp
**    Port to Linux and VMS added
**
**    Revision 1.1  2000/11/14 17:28:08  asp
**    Initial revision
**
**
*******************************************************************************/



/*
** ANSI standard include files
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef I_CMT_OS_QNX4

/*
** OS specific include files
*/
#include <unistd.h>
#include <fcntl.h>
#include <i86.h>
#include <sys/kernel.h>
#include <sys/seginfo.h>
#include <sys/times.h>
#include <sys/osinfo.h>
#include <sys/osstat.h>
#include <sys/types.h>
#include <sys/disk.h>
#include <sys/psinfo.h>
#include <sys/sched.h>

#endif

/*
** TTL include files
*/
#include "TtlSystem.h"
#include "Log.h"
#include "Tim.h"
#include "Cmt.h"


/*
** Local include files
*/
#include "CmtPrivate.h"


/*******************************************************************************
** Function Name:
**    iCmtGetExtStatus
**
** Purpose:
**    Get the computer status information.
**
** Description:
**    This function uses system specific calls to extract system information
**    and statistics.
**
** Return Type:
**    Status_t  (out)  
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iCmtGetExtStatus ( void )
{

   Status_t Status;

   Status = iCmtGetCpuUsed(  );

   if ( Status != SYS_NOMINAL )
   { 
      eLogWarning( Status, "Unable to determine CPU usage." );
   }

   Status = iCmtGetDiskFree( );

   if ( Status != SYS_NOMINAL )
   { 
      eLogWarning( Status, "Unable to determine disk usage." );
   }

   Status = iCmtGetMemoryFree(  );

   if ( Status != SYS_NOMINAL )    
   { 
      eLogWarning( Status, "Unable to determine memory usage." );
   }

   Status = iCmtGetNtpLocked(  );              

   if ( Status != SYS_NOMINAL )
   { 
      eLogWarning( Status, "Unable to determine if ntp locked." );
   }


#if 0
   Status = iCmtGetTemperatures( );

   if ( Status != SYS_NOMINAL )
   { 
      eLogWarning( Status, "Unable to determine motherboard temperatures." );
   }
                    
   Status = iCmtGetFanSpeed( );

   if ( Status != SYS_NOMINAL )
   { 
      eLogWarning( Status, "Unable to determine motherboard fan speeds." );
   }

   Status = iCmtGetVoltages(  );              

   if ( Status != SYS_NOMINAL )
   { 
      eLogWarning( Status, "Unable to determine motherboard voltages." );
   }
#endif
   return Status;

}

/*******************************************************************************
** Function Name:
**    iCmtGetCpuUsed
**
** Purpose:
**    Get the current percentage CPU usage.
**
** Description:
**    This function uses system specific calls to extract the required
**    system information and statistics.
**
** Return Type:
**    Status_t  (out)  
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

Status_t iCmtGetCpuUsed ( void )
{

   Status_t Status;   /* Returned status */
   Int32_t  CpuUsed;  /* Amount of Cpu used (milli-percent) */
   Int32_t  NumRet;   /* Number of parameters returned. */
   
#ifdef I_CMT_OS_QNX6 

   Status = iCmtGetQnx6CpuUsed( &CpuUsed, &NumRet );

#endif

#ifdef I_CMT_OS_QNX4 

   Status = iCmtGetQnxCpuUsed( &CpuUsed, &NumRet );

#endif

#ifdef I_CMT_OS_LINUX

   Status = iCmtGetLinuxCpuUsed( &CpuUsed, &NumRet );

#endif

#ifdef I_CMT_OS_VMS

   Status = iCmtGetVmsCpuUsed( &CpuUsed, &NumRet );

#endif

   if ( ( Status == SYS_NOMINAL) && ( NumRet == 1 ) )
   {

      iCmtData.SdbParams.Datum[ D_CMT_CPU_USED_PERCENT ].Msrment.Value =
               CpuUsed;

      iCmtData.SdbParams.Datum[ D_CMT_CPU_USED_PERCENT ].Units =
               E_SDB_MPERCENT_UNITS;

      iCmtData.SdbDatumChanged[ D_CMT_CPU_USED_PERCENT ] = TRUE;
   }

   return Status;
}               

/*******************************************************************************
** Function Name:
**    iCmtGetDiskFree
**
** Purpose:
**    Get the current disk usage.
**
** Description:
**    This function uses system specific calls to extract the required
**    system information and statistics.
**
** Return Type:
**    Status_t  (out)  
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

Status_t iCmtGetDiskFree ( void )
{

   Status_t Status;     /* Returned status. */

   Int32_t  Disk1Free,  /* Free disk space. */ 
            Disk2Free,
            Disk3Free,
            Disk4Free;
   Int32_t  NumRet;     /* Number of valid parameters returned. */

   /*
   ** Initially assume no disks are too full.
   */
   iCmtData.DiskWarn = FALSE;
   
#ifdef I_CMT_OS_QNX6

   Status = iCmtGetQnx6DiskFree( &Disk1Free,
                                &Disk2Free,
                                &Disk3Free,
                                &Disk4Free,
                                &NumRet );
#endif

#ifdef I_CMT_OS_QNX4

   Status = iCmtGetQnxDiskFree( &Disk1Free,
                                &Disk2Free,
                                &Disk3Free,
                                &Disk4Free,
                                &NumRet );
#endif

#ifdef I_CMT_OS_LINUX

   Status = iCmtGetLinuxDiskFree( &Disk1Free,
                                  &Disk2Free,
                                  &Disk3Free,
                                  &Disk4Free,
                                  &NumRet );
#endif

#ifdef I_CMT_OS_VMS

   Status = iCmtGetVmsDiskFree( &Disk1Free,
                                &Disk2Free,
                                &Disk3Free,
                                &Disk4Free,
                                &NumRet );
#endif

   if ( ( Status == SYS_NOMINAL) && ( NumRet >= 1 ) && ( NumRet <= 4 ) )
   {

      if (NumRet >= 1 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_DISK1_FREE_PERCENT ].Msrment.Value
            = Disk1Free;
         iCmtData.SdbParams.Datum[ D_CMT_DISK1_FREE_PERCENT ].Units
            = E_SDB_MPERCENT_UNITS;

         if ( Disk1Free < iCmtData.DiskFreeWarn[ 0 ] )
         {
            iCmtData.DiskWarn = TRUE;
         }

         iCmtData.SdbDatumChanged[ D_CMT_DISK1_FREE_PERCENT ]               
            = TRUE;
      }

      if (NumRet >= 2 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_DISK2_FREE_PERCENT ].Msrment.Value
            = Disk2Free;
         iCmtData.SdbParams.Datum[ D_CMT_DISK2_FREE_PERCENT ].Units         
            = E_SDB_MPERCENT_UNITS;

         if ( Disk2Free < iCmtData.DiskFreeWarn[ 1 ] )
         {
            iCmtData.DiskWarn = TRUE;
         }

         iCmtData.SdbDatumChanged[ D_CMT_DISK2_FREE_PERCENT ]               
            = TRUE;
      }

      if (NumRet >= 3 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_DISK3_FREE_PERCENT ].Msrment.Value 
            = Disk3Free;
         iCmtData.SdbParams.Datum[ D_CMT_DISK3_FREE_PERCENT ].Units
            = E_SDB_MPERCENT_UNITS;

         if ( Disk3Free < iCmtData.DiskFreeWarn[ 2 ] )
         {
            iCmtData.DiskWarn = TRUE;
         }

         iCmtData.SdbDatumChanged[ D_CMT_DISK3_FREE_PERCENT ]
            = TRUE;
      }

      if (NumRet >= 4 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_DISK4_FREE_PERCENT ].Msrment.Value 
            = Disk4Free;
         iCmtData.SdbParams.Datum[ D_CMT_DISK4_FREE_PERCENT ].Units
            = E_SDB_MPERCENT_UNITS;

         if ( Disk4Free < iCmtData.DiskFreeWarn[ 3 ] )
         {
            iCmtData.DiskWarn = TRUE;
         }

         iCmtData.SdbDatumChanged[ D_CMT_DISK4_FREE_PERCENT ]
            = TRUE;
      }
   }

   
   return Status;
}

/*******************************************************************************
** Function Name:
**    iCmtGetMemoryFree
**
** Purpose:
**    Get the current free memory.
**
** Description:
**    This function uses system specific calls to extract the required
**    system information and statistics.
**
** Return Type:
**    Status_t  (out)  
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

Status_t iCmtGetMemoryFree ( void )
{

   Status_t Status;   /* Return status. */
   Int32_t  MemFree;  /* Free memory determined from system call. */
   Int32_t  NumRet;   /* Number of parameters returned. */

#ifdef I_CMT_OS_QNX6

   Status = iCmtGetQnx6MemoryFree( &MemFree, &NumRet ); 

#endif

#ifdef I_CMT_OS_QNX4

   Status = iCmtGetQnxMemoryFree( &MemFree, &NumRet ); 

#endif

#ifdef I_CMT_OS_LINUX
            
   Status = iCmtGetLinuxMemoryFree( &MemFree, &NumRet ); 

#endif

#ifdef I_CMT_OS_VMS

   Status = iCmtGetVmsMemoryFree( &MemFree, &NumRet ); 

#endif
                                
   if ( ( Status == SYS_NOMINAL) && ( NumRet == 1 ) )
   {

      iCmtData.SdbParams.Datum[ D_CMT_MEMORY_FREE_PERCENT ].Msrment.Value =
               MemFree;

      iCmtData.SdbParams.Datum[ D_CMT_MEMORY_FREE_PERCENT ].Units =
               E_SDB_MPERCENT_UNITS;

      iCmtData.SdbDatumChanged[ D_CMT_MEMORY_FREE_PERCENT ] = TRUE;
   }


   return Status;
}

/*******************************************************************************
** Function Name:
**    iCmtGetNtpLocked
**
** Purpose:
**    Get the ntp locked status
**
** Description:
**    This function uses system specific calls to extract the required
**    system information and statistics.
**
** Return Type:
**    Status_t  (out)  
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

Status_t iCmtGetNtpLocked ( void )
{

  Status_t Status;
  Bool_t   Locked;
  static Bool_t LockedReported   = FALSE;
  static Bool_t UnlockedReported = FALSE;

#ifdef I_CMT_OS_LINUX

  Status = iCmtGetLinuxNtpLocked( &Locked );

#endif

#ifdef I_CMT_OS_QNX4

  Status = iCmtGetQnx4NtpLocked( &Locked );

#endif

#ifdef I_CMT_OS_QNX6

  Status = iCmtGetQnx6NtpLocked( &Locked );

#endif


#ifdef I_CMT_OS_VMS

  Status = iCmtGetVmsNtpLocked( &Locked );

#endif

  if ( Status == SYS_NOMINAL )
  {
     iCmtData.SdbParams.Datum[ D_CMT_NTP_LOCKED ].Msrment.Value 
        = Locked;
     iCmtData.SdbParams.Datum[ D_CMT_NTP_LOCKED ].Units
        = E_SDB_TRUEFALSE_UNITS;
     iCmtData.SdbDatumChanged[ D_CMT_NTP_LOCKED ]
        = TRUE;
     
     if ( Locked )
     {
        iCmtData.NtpWarn = FALSE;
        if ( !LockedReported )
        {
           eLogWarning( 0, "Ntp has become locked" );
           LockedReported   = TRUE;
           UnlockedReported = FALSE;
        }
     }
     else
     {
        iCmtData.NtpWarn = TRUE;
        if ( !UnlockedReported )
        {
           eLogWarning( 0, "Ntp has become unlocked" );
           UnlockedReported = TRUE;
           LockedReported   = FALSE;
        }
     }
  }


  return Status;

}



/*******************************************************************************
** Function Name:
**    iCmtGetTemperatures
**
** Purpose:
**    Get the current motherboard temperatures.
**
** Description:
**    This function uses system specific calls to extract the required
**    system information and statistics.
**
** Return Type:
**    Status_t  (out)  
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

Status_t iCmtGetTemperatures ( void )
{

   Int32_t  Temp1,   /* Temperatures determined from hardware. */
            Temp2,
            Temp3;   
   Int32_t  NumRet;  /* Number of returned parameters. */
   Status_t Status;  /* Return status. */

#ifdef I_CMT_OS_QNX6

   Status = iCmtGetQnx6Temperature( &Temp1, &Temp2, &Temp3, &NumRet );

#endif

#ifdef I_CMT_OS_QNX4

   Status = iCmtGetQnxTemperature( &Temp1, &Temp2, &Temp3, &NumRet );

#endif

#ifdef I_CMT_OS_LINUX

   Status = iCmtGetLinuxTemperature( &Temp1, &Temp2, &Temp3, &NumRet ); 

#endif

#ifdef I_CMT_OS_VMS

   Status = iCmtGetVmsTemperature( &Temp1, &Temp2, &Temp3, &NumRet ); 

#endif

   if ( ( Status == SYS_NOMINAL) && ( NumRet >= 1 ) && ( NumRet <= 3 ) )
   {

      if (NumRet >= 1 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_TEMPERATURE1 ].Msrment.Value
            = Temp1;
         iCmtData.SdbParams.Datum[ D_CMT_TEMPERATURE1 ].Units
            = E_SDB_MCELSIUS_UNITS;
         iCmtData.SdbDatumChanged[ D_CMT_TEMPERATURE1 ]               
            = TRUE;
      }

      if (NumRet >= 2 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_TEMPERATURE2 ].Msrment.Value
            = Temp2;
         iCmtData.SdbParams.Datum[ D_CMT_TEMPERATURE2 ].Units         
            = E_SDB_MCELSIUS_UNITS;
         iCmtData.SdbDatumChanged[ D_CMT_TEMPERATURE2 ]               
            = TRUE;
      }

      if (NumRet >= 3 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_TEMPERATURE3 ].Msrment.Value 
            = Temp3;
         iCmtData.SdbParams.Datum[ D_CMT_TEMPERATURE3 ].Units
            = E_SDB_MCELSIUS_UNITS;
         iCmtData.SdbDatumChanged[ D_CMT_TEMPERATURE3 ]
            = TRUE;
      }

   }

   return Status;
}

/*******************************************************************************
** Function Name:
**    iCmtGetFanSpeed
**
** Purpose:
**    Get the current motherboard fan speeds.
**
** Description:
**    This function uses system specific calls to extract the required
**    system information and statistics.
**
** Return Type:
**    Status_t  (out)  
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

Status_t iCmtGetFanSpeed ( void )
{

   Int32_t  Fan1,    /* Fan speeds determined from hardware. */
            Fan2,
            Fan3;    
   Int32_t  NumRet;  /* Number of parameters returned. */
   Status_t Status;

#ifdef I_CMT_OS_QNX6

   Status = iCmtGetQnx6FanSpeed( &Fan1, &Fan2, &Fan3, &NumRet );

#endif

#ifdef I_CMT_OS_QNX4

   Status = iCmtGetQnxFanSpeed( &Fan1, &Fan2, &Fan3, &NumRet );

#endif

#ifdef I_CMT_OS_LINUX

   Status = iCmtGetLinuxFanSpeed( &Fan1, &Fan2, &Fan3, &NumRet ); 

#endif

#ifdef I_CMT_OS_VMS

   Status = iCmtGetVmsFanSpeed( &Fan1, &Fan2, &Fan3, &NumRet ); 

#endif


   if ( ( Status == SYS_NOMINAL) && ( NumRet >= 1 ) && ( NumRet <= 3 ) )
   {

      if (NumRet >= 1 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_FAN1_RPM ].Msrment.Value
            = Fan1;
         iCmtData.SdbParams.Datum[ D_CMT_FAN1_RPM ].Units
            = E_SDB_RPM_UNITS;
         iCmtData.SdbDatumChanged[ D_CMT_FAN1_RPM ]               
            = TRUE;
      }

      if (NumRet >= 2 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_FAN2_RPM ].Msrment.Value
            = Fan2;
         iCmtData.SdbParams.Datum[ D_CMT_FAN2_RPM ].Units         
            = E_SDB_RPM_UNITS;
         iCmtData.SdbDatumChanged[ D_CMT_FAN2_RPM ]               
            = TRUE;
      }

      if (NumRet >= 3 )
      {
         iCmtData.SdbParams.Datum[ D_CMT_FAN3_RPM ].Msrment.Value 
            = Fan3;
         iCmtData.SdbParams.Datum[ D_CMT_FAN3_RPM ].Units
            = E_SDB_RPM_UNITS;
         iCmtData.SdbDatumChanged[ D_CMT_FAN3_RPM ]
            = TRUE;
      }
   }

   return Status;

}

/*******************************************************************************
** Function Name:
**    iCmtGetVoltages
**
** Purpose:
**    Get the current motherboard voltages.
**
** Description:
**    This function uses system specific calls to extract the required
**    system information and statistics.
**
** Return Type:
**    Status_t  (out)  
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

Status_t iCmtGetVoltages ( void )
{

   Status_t Status;

#ifdef I_CMT_OS_QNX6

     Status = SYS_NOMINAL;
/*
**   Status = iCmtGetQnx6Voltages( 
**             &iCmtData.SdbParams.Datum[ D_CMT_VOLT_CORE ],
**             &iCmtData.SdbParams.Datum[ D_CMT_VOLT_REF ] );
*/
#endif

#ifdef I_CMT_OS_QNX4

     Status = SYS_NOMINAL;
/*
**   Status = iCmtGetQnxVoltages( 
**             &iCmtData.SdbParams.Datum[ D_CMT_VOLT_CORE ],
**             &iCmtData.SdbParams.Datum[ D_CMT_VOLT_REF ] );
*/

#endif

#ifdef I_CMT_OS_LINUX

  Status = SYS_NOMINAL;
/*
**   Status = iCmtGetLinuxVoltages( 
**             &iCmtData.SdbParams.Datum[ D_CMT_VOLT_CORE ],
**             &iCmtData.SdbParams.Datum[ D_CMT_VOLT_REF ] );
*/

#endif

#ifdef I_CMT_OS_VMS

  Status = SYS_NOMINAL;
/*
   Status = iCmtGetVmsVoltages( 
             &iCmtData.SdbParams.Datum[ D_CMT_VOLT_CORE ],
            &iCmtData.SdbParams.Datum[ D_CMT_VOLT_REF ] );
*/
#endif

   return Status;

}



#ifdef DONT_INCLUDE /*LINUX */

/*******************************************************************************
**
** LINUX version of module
**
*******************************************************************************/


/*
** ANSI standard include files
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


/*
** OS specific include files
*/
#include <unistd.h>
#include <mntent.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <linux/kernel.h>
#include <linux/sys.h>


/*
** TTL specific include files
*/
#include "TtlSystem.h"
#include "Log.h"
#include "Tim.h"
#include "Cmt.h"


/*
** Application specific include files
*/
#include "CmtPrivate.h"

/*
** Module specific function prototypes
*/
int  mCmtCpuLoadLinux( void );


/*******************************************************************************
** Function Name:
**    iCmtGetExtStatus
**
** Purpose:
**    Get the computer status information.
**
** Description:
**    This function uses system specific calls to extract system information
**    and statistics.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/


Status_t iCmtGetExtStatus( void )
{
#define VALID_FILETYPE_1   "ext"  /* Currently only the two normal Linux */
#define VALID_FILETYPE_2   "ext2" /* file-systems are supported */
#define INVALID_FILETYPE_1 "proc" /* Reported as a file system */

   Status_t   Status;           /* Returned status of functions called */

   eTtlTime_t CurrentTime;      /* Time at which status was gained */

   int i;                       /* Loop counter */

   static int CpuUsedPercent    = 0;
   static int DiskFreePercent   = 0;
   static int MemoryFreePercent = 0;

   /* 
   ** Structures for the extraction of system information
   */
   struct sysinfo  SysInfo;       /* Linux System information */ 
   struct statfs   StatFs;        /* File system statistics */
   struct mntent  *DiskMountPtr;  /* Disk mount entity info */
 
   char  *DiskTablePtr = MOUNTED; /* Look for mounted disks */

   FILE  *DiskPtr;                /* File pointer but used here for disks */

   eCmtDataId_t DataIdDisk;       /* Data ID for disks */

   /* Start of code */

   /* 
   ** Get the time and set the timestamp for each datum
   */
   Status = eTimGetTime( &CurrentTime );
   for ( i = D_CMT_PROC_STATE; i < D_CMT_DATAID_EOL; i++ )
   {
      eTimCopy( CurrentTime, &iCmtData.SdbParams.Datum[ i ].Msrment.TimeStamp);
   }

   /*
   ** Initially assume failure to read disks
   */
   for ( i = D_CMT_DISK1_FREE_PERCENT; i <= I_CMT_DISK_LAST; i++ )
   {
      /*
      ** Mark units as invalid
      */
      iCmtData.SdbParams.Datum[ i ].Units = I_CMT_BAD_UNITS;      
   }
   
   /* 
   ** There may be several file systems so start at first one
   */
   DataIdDisk = D_CMT_DISK1_FREE_PERCENT;

   /* 
   ** Look at the first mounted disk device - uses /etc/fstab 
   */
   DiskPtr = setmntent( DiskTablePtr, "r" );
   if ( DiskPtr )
   {
      /* 
      ** While there are disks to look at (ie DiskMountPtr > 0)
      */
      while ( DiskMountPtr = getmntent( DiskPtr ) )
      {
         /*
         ** Check if mounted disk has a valid recognised file type
         */
         if ( !strcmp( DiskMountPtr->mnt_type, VALID_FILETYPE_1 )|| 
              !strcmp( DiskMountPtr->mnt_type, VALID_FILETYPE_2 )  )
         {
            /* 
            ** Read the disk stats for the mounted disk 
            */
            if ( ( statfs( DiskMountPtr->mnt_dir, &StatFs ) )||
                 ( StatFs.f_blocks == 0                     )  )
            {
               Status = SYS_WARNING;
               eLogWarning( Status, "Failed to get disk statistics" );
            }
            else
            {
               DiskFreePercent = ( StatFs.f_bfree * I_CMT_MILLIPERCENT )
                                 / StatFs.f_blocks;

               /* 
               ** Load the value and timestamp into the data area 
               */
               iCmtData.SdbParams.Datum[ DataIdDisk ].Msrment.Value 
                  = DiskFreePercent;

               /*
               ** Use correct units
               */
               iCmtData.SdbParams.Datum[ DataIdDisk ].Units
                  = iCmtSdbDatumType[ DataIdDisk ];     
       
               /* 
               ** Increment the DataId & check if space for more disk info 
               */ 
               if ( DataIdDisk++ >= I_CMT_DISK_LAST ) break;
            }
         }
         else if ( !strcmp( DiskMountPtr->mnt_type, INVALID_FILETYPE_1 ) )
         {
            ; /* An ignoreable file system type so skip */
         } 
         else
         {
            Status = SYS_WARNING;
            eLogWarning( Status, "File system \"%s\" unrecognised", 
                         DiskMountPtr->mnt_type );
         }
      }

      /* 
      ** Close the disk file 
      */
      endmntent( DiskPtr );
   }
   else
   {
      Status = SYS_WARNING;
      eLogWarning( Status, "Failure calling statfs()" );
   }

   /* 
   ** Retrieve system information 
   */
   if ( sysinfo( &SysInfo ) )
   {
      Status = SYS_WARNING;
      eLogWarning( Status, "Failure calling sysinfo()" );

      iCmtData.SdbParams.Datum[ D_CMT_MEMORY_FREE_PERCENT ].Units
         = I_CMT_BAD_UNITS;     
   }
   else
   {

      MemoryFreePercent = ( SysInfo.freeram * I_CMT_MILLIPERCENT )  
                           / SysInfo.totalram;
      /* 
      ** Write values into data area for submitting SDB parameters 
      */
      iCmtData.SdbParams.Datum[ D_CMT_MEMORY_FREE_PERCENT].Msrment.Value
         = MemoryFreePercent;

      /*
      ** Use correct units
      */
      iCmtData.SdbParams.Datum[ D_CMT_MEMORY_FREE_PERCENT ].Units
         = iCmtSdbDatumType[ D_CMT_MEMORY_FREE_PERCENT ];     
   }
   
   /*
   ** Get the CPU loading
   */
   CpuUsedPercent = mCmtCpuLoadLinux();
   iCmtData.SdbParams.Datum[ D_CMT_CPU_USED_PERCENT].Msrment.Value
      = CpuUsedPercent;
   
   /*
   ** Use correct units
   */
   iCmtData.SdbParams.Datum[ D_CMT_CPU_USED_PERCENT ].Units
      = iCmtSdbDatumType[ D_CMT_CPU_USED_PERCENT ];        

   /*
   ** Get hardware monitor parameters. This function writes fan-speeds, 
   ** temperatures & voltages into iCmtData.SdbParams.Datum. 
   */
   iCmtReadHardware();

   return Status;

}  /* End of iCmtGetExtStatus() */


/*******************************************************************************
** Function Name:
**    mCmtCpuLoadLinux
**
** Purpose:
**    Get the CPU usage information from the Linux system.
**
** Description:
**    This function is determines CPU load by summing Null processes
**    CPU usage times.
**    The inital returned value is always 50%. 
**
** Return Type:
**    int    
**       Returns the CPU load in milli-percent.
**
** Arguments:
**    none
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/


int mCmtCpuLoadLinux( void )
{
#define PROC_STATE_FILE "/proc/stat" /* Location of proc state pseudo-file */ 
#define CPU_100_PERCENT 100000       /* Because we report in milli-percent */
#define CPU_50_PERCENT   50000       /* Because we report in milli-percent */
#define JIFFIE_PERCENT  1000.0       /* Convert percent Linux jiffies      */

   time_t     NowIdle;               /* Current CPU idle time */ 
   eTtlTime_t NowTime;               /* Current time */
   eTtlTime_t DiffTime;              /* Time difference */
   double     Interval;              /* Time since last load check */      
   FILE       *Fp;                   /* File pointer */

   static  int        CpuIdle = CPU_50_PERCENT; /* Cpu free time */ 
   static  time_t     OldIdle = 0;              /* Previous CPU idle time */
   static  eTtlTime_t OldTime = {0,0};          /* Previous time */

   /*
   ** Open the pseudo file /proc/stat 
   */
   Fp = fopen( PROC_STATE_FILE, "r" );
   if ( Fp == NULL )
   {
      /*
      ** Failed to open stat file, return existing value
      */
      return CpuIdle;
   }

   /*
   ** Read in the CPU idle time in Linux "jiffies"
   ** Skip all other fields
   */
   if ( 1 != fscanf( Fp, "%*s %*lu %*lu %*lu %lu", &NowIdle ) )
   {
      /*
      ** Failed to read stat file, return exisint value
      */
      return CpuIdle;
   }

   /*
   ** Close the file
   */
   fclose( Fp );

   /*
   ** Idle count is usually 1 too much so adjust accordingly
   */
   if ( NowIdle != OldIdle ) OldIdle++;

   /*
   ** Get the current time
   */
   eTimGetTime( &NowTime );
   
   /*
   ** If not the first time and time has passed calculate the null process
   ** CPU load else use the previous value.
   */
   eTimDifference( &OldTime, &NowTime, &DiffTime );
   eTimToDouble( &DiffTime, &Interval );    
   if ( ( Interval      >  0.0)&&  /* Some time has passed and */ 
        ( OldTime.t_sec != 0  )  ) /* not the first time thru' */ 
   {
      CpuIdle = ( NowIdle - OldIdle ) * JIFFIE_PERCENT / Interval;
   }
   
   /*
   ** Save the current usage stats
   */
   OldTime = NowTime;
   OldIdle = NowIdle;
   
   /*
   ** Sanity check so that CPU load is within range 
   */
   if ( CpuIdle < 0 )
   {
      CpuIdle = 0;
   }
   else if ( CpuIdle > CPU_100_PERCENT )
   {
      CpuIdle = CPU_100_PERCENT;
   }
         
   /*
   ** Return the CPU used load
   */
   return( CPU_100_PERCENT - CpuIdle );

} /* End of mGetCpuLoadLinux */

#endif /* __GNUC__ LINUX */



/*******************************************************************************
** End of File Name: CmtInterface.c
*******************************************************************************/
