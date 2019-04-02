/*******************************************************************************
** Module Name:
**    CmtLinux.c
**
** Purpose:
**    Linux specific functions.
**
** Description:
**    The module of the CMT which interfaces only to Linux to extract system 
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
**    $Id: CmtLinux.c,v 0.10 2007/10/16 08:33:09 mjf Exp $
**
** History:
**    $Log: CmtLinux.c,v $
**    Revision 0.10  2007/10/16 08:33:09  mjf
**    Linux functionality proven for CPU load, disk space, free memory and
**    NTP locking. Note NTP took ~8 hours to unlock following an 'ntpd stop'
**    but only several minutes to re-lock following an 'ntpd start'.
**
**    Revision 0.9  2007/10/04 10:27:15  mjf
**    Minor rework for Fedora 7 - no functional changes.
**
**    Revision 0.8  2006/08/23 09:49:12  mjf
**    Minor improvements for Linux.
**
**    Revision 0.7  2006/03/21 11:21:56  sxf
**    Added NTP lock functionaity for QNX6, Linux and VMS.
**
**    Revision 0.6  2006/01/31 11:56:05  sxf
**    Added check for NTP locking under linux.
**
**    Revision 0.5  2003/12/03 10:17:13  sxf
**    Debug - to compile on QNX.
**
**    Revision 0.4  2003/12/03 10:15:11  sxf
**    Simplify disk monitoring function.
**
**    Revision 0.1  2002/08/01 10:48:18  sxf
**    Not yet implemented.
**
**
*******************************************************************************/

#include "CmtPrivate.h"

#ifdef I_CMT_OS_LINUX

/*
** ANSI standard include files
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

/*
** OS specific include files
*/
#include <unistd.h>
#include <mntent.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <linux/kernel.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/timex.h>

/*
** TTL include files
*/
#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Log.h"
#include "Tim.h"
#include "Cmt.h"


#define M_CMT_MAXPACKETSIZE       1500
#define M_CMT_NTP_PORT            123    /* for ref only (see /etc/services) */
#define M_CMT_NTPVERSION_1        0x08
#define M_CMT_NTP_INFO_QUERY      62     /* **** THIS implementation dependent **** */
#define M_CMT_NTP_INFO_REPLY      63     /* **** THIS implementation dependent **** */
#define M_CMT_NTP_STIME           500000 /* usec to wait for another response */
#define M_CMT_NTP_WTIME           1     /* Time to wait for all responses */
#define M_CMT_NTPDC_VERSION       2
#define M_CMT_NTP_PEER_SHIFT      8
#define M_CMT_NTP_SHIFT_MASK      0xff   /* number of intervals to wait */
#define M_CMT_NTP_PEER_FL_CONFIG  1
#define M_CMT_NTP_PEER_FL_AUTHENABLE 2
#define M_CMT_NTP_PEER_FL_SANE    0x0100 /* sane peer */
#define M_CMT_NTP_PEER_FL_CANDIDATE 0x0200 /* candidate peer */
#define M_CMT_NTP_PEER_FL_SYNC      0x1000 /* peer can bet sync'd to */
#define M_CMT_NTP_PEER_FL_BCAST     0x2000 /* broadcast peer */
#define M_CMT_NTP_PEER_FL_REFCLOCK 0x4000 /* peer is a local reference clock */
#define M_CMT_NTP_PEER_FL_SELECTED 0x8000 /* actually used by query routine */

static char mCmtLinuxPacket[ M_CMT_MAXPACKETSIZE ];
int         mCmtLinuxSocket;
int         mCmtLinuxTimedOut;

void mCmtLinuxTimeOut( int );

/*
 * Structure definitions for NTP fixed point values
 *
 *    0           1            2           3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                Integer Part              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                Fraction Part              |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *    0           1            2           3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |        Integer Part        |      Fraction Part        |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
struct l_fixedpt {
   u_long int_part;
   u_long fraction;
};

struct s_fixedpt {
   u_short int_part;
   u_short fraction;
};

/*  =================  Table 3.3. Packet Variables   ================= */
/*
 *    0           1            2           3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |LI | VN  | Mode|     Stratum    |       Poll     |    Precision   |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |            Synchronizing Distance           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |           Synchronizing Dispersion           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |         Reference Clock Identifier           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             |
 *   |             Reference Timestamp (64 bits)           |
 *   |                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             |
 *   |             Originate Timestamp (64 bits)           |
 *   |                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             |
 *   |         Receive Timestamp (64 bits)           |
 *   |                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                             |
 *   |         Transmit Timestamp (64 bits)           |
 *   |                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
struct ntpdata {
   u_char status;      /* status of local clock and leap info */
   u_char stratum;      /* Stratum level */
   u_char ppoll;      /* poll value */
   int precision:8;
   struct s_fixedpt distance;
   struct s_fixedpt dispersion;
   u_long refid;
   struct l_fixedpt reftime;
   struct l_fixedpt org;
   struct l_fixedpt rec;
   struct l_fixedpt xmt;
};

struct ntpinfo {
   u_char version;
   u_char type;      /* request type (stratum in ntp packets) */
   u_char count;      /* number of entries in this packet */
   u_char seq;      /* sequence number of this packet */

   u_char npkts;      /* total number of packets */
   u_char peers;
   u_char fill3;
   u_char fill4;
};

struct clockinfo {
   u_long net_address;
   u_long my_address;
   u_short port;
   u_short flags;
   u_long pkt_sent;
   u_long pkt_rcvd;
   u_long pkt_dropped;
   u_long timer;
   u_char leap;
   u_char stratum;
   u_char ppoll;
   int precision:8;

   u_char hpoll;
   u_char filler1;
   u_short reach;

   long   estdisp;         /* scaled by 1000 */
   long   estdelay;         /* in milliseconds */
   long   estoffset;         /* in milliseconds */
   u_long refid;
   struct l_fixedpt reftime;
   struct info_filter {
      short index;
      short filler;
      long offset[M_CMT_NTP_PEER_SHIFT];   /* in milliseconds */
      long delay[M_CMT_NTP_PEER_SHIFT];      /* in milliseconds */
   } info_filter;
}; 

Bool_t mCmtOutputNtpInfo( struct clockinfo *InfoPtr );

                  
/*******************************************************************************
** Function Name:
**    iCmtDetectHardware
**
** Purpose:
**    Dummy function - no hardware to detect on Linux system.
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
**    Steve Foale (sxf)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int iCmtDetectHardware( void )
{

   return FALSE;

}

/*******************************************************************************
** Function Name:
**    iCmtGetLinuxCpuUsed
**
** Purpose:
**    Get the CPU usage information from the Linux system.
**
** Description:
**    Not currently implemented.
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

Status_t iCmtGetLinuxCpuUsed( Int32_t *CpuUsedPtr, Int32_t *NumRet )
{
#if 0
#define PROC_STATE_FILE "/proc/stat" /* Location of proc state pseudo-file */
#else
#define PROC_STATE_FILE "/proc/uptime" /* Location of proc uptime pseudo-file */
#endif
#define CPU_100_PERCENT 100000       /* Because we report in milli-percent */
#define CPU_50_PERCENT   50000       /* Because we report in milli-percent */
#define JIFFIE_PERCENT  1000.0       /* Convert percent Linux jiffies      */

#if 0
   time_t     NowIdle;               /* Current CPU idle time */
#else
   double     NowIdle;               /* Current CPU idle time */
#endif
   eTtlTime_t NowTime;               /* Current time */
   eTtlTime_t DiffTime;              /* Time difference */
   double     Interval;              /* Time since last load check */
   FILE       *Fp;                   /* File pointer */

   static  int        CpuIdle = CPU_50_PERCENT; /* Cpu free time */
#if 0
   static  time_t     OldIdle = 0;              /* Previous CPU idle time */
#else
   static  double     OldIdle = 0.0;            /* Previous CPU idle time */
#endif
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

#if 0
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
#else
   /*
   ** Read in the CPU idle time from the 'uptime'.
   ** Skip all other fields
   */
   if ( 1 != fscanf( Fp, "%*lf %lf", &NowIdle ) )
   {
      /*
      ** Failed to read stat file, return exisint value
      */
      return CpuIdle;
   }
#endif

   /*
   ** Close the file
   */
   fclose( Fp );

#if 0
   /*
   ** Idle count is usually 1 too much so adjust accordingly
   */
   if ( NowIdle != OldIdle ) OldIdle++;
#else
   /*
   ** Convert into milli-percent.
   */
   NowIdle *= I_CMT_MILLIPERCENT;
#endif

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
#if 0
      CpuIdle = ( NowIdle - OldIdle ) * JIFFIE_PERCENT / Interval;
#else
      CpuIdle = (Int32_t) ( ( NowIdle - OldIdle ) / Interval );
#endif
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
   *CpuUsedPtr = ( CPU_100_PERCENT - CpuIdle );
   *NumRet     = 1;

   eLogInfo( "CPU load %.3f percent", 
             (double) *CpuUsedPtr / E_TTL_MILLI_PER_UNIT );

   return SYS_NOMINAL;

} /* End of iCmtGetLinuxCpuUsed */

/*******************************************************************************
** Function Name:
**    iCmtGetLinuxDiskFree
**
** Purpose:
**    Get the disk free information from the Linux system.
**
** Description:
**    Not currently implemented.
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

Status_t iCmtGetLinuxDiskFree( Int32_t *Disk1FreePtr,
                               Int32_t *Disk2FreePtr, 
                               Int32_t *Disk3FreePtr,
                               Int32_t *Disk4FreePtr,
                               Int32_t *NumRet )
{

#define VALID_FILETYPE_1   "ext"  /* Currently only the three normal Linux */
#define VALID_FILETYPE_2   "ext2" /* file-systems are supported */
#define VALID_FILETYPE_3   "ext3" /* file-systems are supported */

   Status_t   Status;           /* Returned status of functions called */

   eTtlTime_t CurrentTime;      /* Time at which status was gained */

   int i;                       /* Loop counter */
   int Disk;
   static int DiskFreePercent   = 0;

   /* 
   ** Structures for the extraction of system information
   */
   struct sysinfo  SysInfo;       /* Linux System information */ 
   struct statfs   StatFs;        /* File system statistics */
   struct mntent  *DiskMountPtr;  /* Disk mount entity info */
 
   char  *DiskTablePtr = MOUNTED; /* Look for mounted disks */

   FILE  *DiskPtr;                /* File pointer but used here for disks */

   eCmtDataId_t DataIdDisk;       /* Data ID for disks */

   double f_bfree;                /* From statfs structure */
   double f_blocks;
   double f_bsize;

   /* Start of code */

   *NumRet = 0;

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
              !strcmp( DiskMountPtr->mnt_type, VALID_FILETYPE_3 )||
              !strcmp( DiskMountPtr->mnt_type, VALID_FILETYPE_2 )  )
         {
            /*
            ** Find index of disk (if we're monitoring it)
            */
            if ( strcmp( DiskMountPtr -> mnt_fsname, 
                         iCmtData.DisksToMonitor[ 0 ] ) == 0 )
            {
               Disk = 1;
            }
            else if ( strcmp( DiskMountPtr -> mnt_fsname, 
                         iCmtData.DisksToMonitor[ 1 ] ) == 0 )
            {
               Disk = 2;
            }
            else if ( strcmp( DiskMountPtr -> mnt_fsname, 
                         iCmtData.DisksToMonitor[ 2 ] ) == 0 )
            {
               Disk = 3;
            }
            else if ( strcmp( DiskMountPtr -> mnt_fsname, 
                         iCmtData.DisksToMonitor[ 3 ] ) == 0 )
            {
               Disk = 4;
            }
            else
            {
               Disk = 0;
            }

            /* 
            ** If disk is one we're monitoring 
            */
            if ( Disk > 0 )
            {
               /* 
               ** Read the disk stats for the mounted disk 
               */
               if ( ( statfs( DiskMountPtr->mnt_dir, &StatFs ) )||
                    ( StatFs.f_blocks == 0                     )  )
               {
                  Status = SYS_WARNING;
                  eLogWarning( Status, "Failed to get disk statistics, errno = %d",errno );
               }
               else
               {
                  f_bfree  = (double) StatFs.f_bfree;
                  f_blocks = (double) StatFs.f_blocks;
                  f_bsize  = (double) StatFs.f_bsize;

                  DiskFreePercent = (Int32_t) ( ( f_bfree * I_CMT_MILLIPERCENT ) / f_blocks );

                  eLogInfo( "Disk '%s' %.3f percent free (%.0f of %.0f MB)", 
                            DiskMountPtr->mnt_fsname, 
                            (double) DiskFreePercent / E_TTL_MILLI_PER_UNIT,
                            ( f_bfree * f_bsize ) / (double) ( I_CMT_KILOBYTE * I_CMT_KILOBYTE ),
                            ( f_blocks * f_bsize ) / (double) ( I_CMT_KILOBYTE * I_CMT_KILOBYTE ) );

                  /* 
                  ** Load the value and timestamp into the data area 
                  */
                  if ( Disk == 1 )
                  {
                     *Disk1FreePtr = DiskFreePercent;
                  }

                  if ( Disk == 2 )
                  {
                     *Disk2FreePtr = DiskFreePercent;
                  }

                  if ( Disk == 3 )
                  {
                     *Disk3FreePtr = DiskFreePercent;
                  }

                  if ( Disk == 4 )
                  {
                     *Disk4FreePtr = DiskFreePercent;
                  }

                  (*NumRet)++;


               }
            } /* Endif - is disk one we're monitoring */
         } /* Endif - valid file type */
      } /* End-while */

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

   return SYS_NOMINAL;
}

/*******************************************************************************
** Function Name:
**    iCmtGetLinuxMemoryFree
**
** Purpose:
**    Get the memory free information from the Linux system.
**
** Description:
**    Not currently implented.
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

Status_t iCmtGetLinuxMemoryFree( Int32_t *MemFreePtr,
                                 Int32_t *NumRet )
{
   static int MemoryFreePercent = 0;
   Status_t   Status;
   struct sysinfo  SysInfo;       /* Linux System information */
   double freeram;                /* From the 'sysinfo' structure */
   double totalram;               /* From the 'sysinfo' structure */
   double mem_unit;               /* From the 'sysinfo' structure */

   /* 
   ** Retrieve system information 
   */
   if ( sysinfo( &SysInfo ) )
   {
      Status = SYS_WARNING;
      eLogWarning( Status, "Failure calling sysinfo()" );
      *NumRet = 0;

   }
   else
   {
      freeram  = (double) SysInfo.freeram;
      totalram = (double) SysInfo.totalram;
      mem_unit = (double) SysInfo.mem_unit;

      *MemFreePtr = (Int32_t) ( ( freeram * I_CMT_MILLIPERCENT ) / totalram );

      *NumRet = 1;

      eLogInfo( "Free memory %.3f percent (%lu MB of %lu MB)", 
                (double) *MemFreePtr / E_TTL_MILLI_PER_UNIT,
                (unsigned long) ( freeram * mem_unit ) / ( I_CMT_KILOBYTE * I_CMT_KILOBYTE ),
                (unsigned long) ( totalram * mem_unit ) / ( I_CMT_KILOBYTE * I_CMT_KILOBYTE ) );
   }

   return SYS_NOMINAL;

}

/*******************************************************************************
** Function Name:
**    iCmtGetLinuxTemperature
**
** Purpose:
**    Get the temperature information from the Linux system.
**
** Description:
**    Not currently implemented.
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

Status_t iCmtGetLinuxTemperature( Int32_t *Temp1Ptr,
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
**    iCmtGetLinuxFanSpeed
**
** Purpose:
**    Get the fan speed information from the Linux system.
**
** Description:
**    Not currently implemented.
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

Status_t iCmtGetLinuxFanSpeed( Int32_t *Fan1Ptr,
                               Int32_t *Fan2Ptr,
                               Int32_t *Fan3Ptr,
                               Int32_t *NumRet )
{

   *Fan1Ptr = 0;
   *Fan2Ptr = 0;
   *Fan3Ptr = 0;
   *NumRet   = 0;

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCmtGetLinuxNtpLocked
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

Status_t iCmtGetLinuxNtpLocked( Bool_t *Locked )
{

#if 0
   struct sockaddr_in       Watcher;
   register struct ntpinfo *MsgInfoPtr = (struct ntpinfo *) mCmtLinuxPacket;
   register struct ntpdata *MsgDataPtr = (struct ntpdata *) mCmtLinuxPacket;
   struct hostent          *HostEntPtr;
   static struct servent   *ServEntPtr = NULL;
   long                     HostAddr;
   static char              MyHost[ 128 ];
   struct clockinfo        *InfoPtr;
   struct sockaddr_in       From;
   socklen_t                FromLen = sizeof(From);
   int                      Count, Cc;
   fd_set                   Bits;
   struct timeval           ShortTime;
   int                      First = 1;
   long                     Replies = 0;
   static Bool_t            FirstTime = TRUE;
   struct sigaction Action; 

   if ( FirstTime )
   {
      mCmtLinuxSocket = 0;
      /* Get my own hostname */
      gethostname( MyHost, 128 );
      eLogDebug( "Host is %s", MyHost );
      FirstTime = FALSE;
   }

   /* Query */
   memset( (char *) &Watcher, 0, sizeof( Watcher ) );
   Watcher.sin_family = AF_INET;
   HostAddr = inet_addr( MyHost ); /* htonl (INADDR_ANY); */
   Watcher.sin_addr.s_addr = (u_long) HostAddr;

   if (HostAddr == -1)
   {
      HostEntPtr = gethostbyname(MyHost);
      if (HostEntPtr == 0)
      {
         eLogCrit( 0,"%s: unknown\n", MyHost);
        return E_CMT_NO_NTPINFO;
      }
      memmove( (char *) &Watcher.sin_addr, HostEntPtr->h_addr, HostEntPtr->h_length );
   }

   ServEntPtr = getservbyname("ntp", "udp");

   if (ServEntPtr == 0)
   {
      Watcher.sin_port = htons(M_CMT_NTP_PORT);
   }
   else
   {
      Watcher.sin_port = ServEntPtr -> s_port;
   }

   MsgDataPtr -> status = M_CMT_NTPVERSION_1;
   MsgDataPtr -> stratum = M_CMT_NTP_INFO_QUERY;

   if ( mCmtLinuxSocket == 0 )
   {
      mCmtLinuxSocket = socket(AF_INET, SOCK_DGRAM, 0);
   }

   if ( mCmtLinuxSocket < 0 )
   {
      eLogNotice( E_CMT_NO_NTPINFO, "Unable to create socket, errno = %d", errno );
      return E_CMT_NO_NTPINFO;
   }


   if (connect( mCmtLinuxSocket, (struct sockaddr *) &Watcher, sizeof(Watcher)))
   {
         eLogNotice( E_CMT_NO_NTPINFO, "Unable to connect to socket, errno = %d", errno );
         return E_CMT_NO_NTPINFO;
   }

   if (send( mCmtLinuxSocket, mCmtLinuxPacket, sizeof(struct ntpdata), 0) < 0)
   {
         eLogNotice( E_CMT_NO_NTPINFO, "Unable to send message to socket, errno = %d", errno );
         return E_CMT_NO_NTPINFO;
   }

   /*
    * Listen for returning packets; may be more than one packet per
    * host. 
    */
   FD_ZERO( &Bits );
   FD_SET( mCmtLinuxSocket, &Bits );
   ShortTime.tv_sec = 0;
   ShortTime.tv_usec = M_CMT_NTP_STIME;
   Action.sa_handler = mCmtLinuxTimeOut;
   (void) sigaction(SIGALRM, &Action, (struct sigaction *)NULL);
   (void) alarm(M_CMT_NTP_WTIME);
   mCmtLinuxTimedOut = 0; 

   while ( 
            ( First || Replies ) && 
            ( !mCmtLinuxTimedOut || select(FD_SETSIZE, &Bits, (fd_set *) 0,
                (fd_set *) 0, &ShortTime) > 0 ) 
         )
   {

      if ( ( Cc = recvfrom(mCmtLinuxSocket, mCmtLinuxPacket, sizeof(mCmtLinuxPacket), MSG_TRUNC,
           (struct sockaddr *)&From, &FromLen)) <= 0 )
      {
         if (Cc == 0 || errno == EINTR)
         {
            continue;
         }

         eLogCrit( E_CMT_NO_NTPINFO, "Error receiving from NTP socket" );
         (void) close( mCmtLinuxSocket );
         return E_CMT_NO_NTPINFO;
      }

      FD_SET( mCmtLinuxSocket, &Bits );

      if ( MsgInfoPtr -> type != M_CMT_NTP_INFO_REPLY )
      {
         return E_CMT_NO_NTPINFO;
      }

      if ( MsgInfoPtr -> version != M_CMT_NTPDC_VERSION)
      {
         return E_CMT_NO_NTPINFO;
      }

      if ( First )
      {
         First = 0;
         Replies = (1L << MsgInfoPtr -> npkts) - 1;
         eLogDebug( "   (rem)  Address   (lcl)      Strat Poll Reach    Delay   Offset    Disp" );
         eLogDebug( "=========================================================================" );
      }

      Replies &= ~(1L << MsgInfoPtr->seq);
      InfoPtr = (struct clockinfo *)&MsgInfoPtr[1];  
      for ( Count = MsgInfoPtr -> count; Count > 0; Count--)
      {
         *Locked = mCmtOutputNtpInfo( InfoPtr );
         InfoPtr++;
      }
   }
#else
   struct ntptimeval NtpTimeval;

   /* Initially assume NTP isn't locked */
   *Locked = FALSE;

   /*
   ** If call to ntp_gettime returns zero, then we're locked.
   */
   if ( ntp_gettime( &NtpTimeval ) == 0 )
   {
      *Locked = TRUE;
   }

#endif

   return SYS_NOMINAL;

}

void mCmtLinuxTimeOut( int i )
{

   i = i;
   mCmtLinuxTimedOut = 1;

}

Bool_t mCmtOutputNtpInfo( struct clockinfo *InfoPtr )
{
   int i;
   double Offset[M_CMT_NTP_PEER_SHIFT], Delay[M_CMT_NTP_PEER_SHIFT], Dsp,Del,Off;
   char C;
   int Flags;
   struct sockaddr_in Sin = {AF_INET};
   Bool_t RetVal = FALSE;

   Sin.sin_addr.s_addr = InfoPtr->net_address;
   for (i = 0; i < M_CMT_NTP_PEER_SHIFT; i++) {
      Delay[i]  = (double) ((long) (ntohl(InfoPtr->info_filter.delay[i])/1000.0));
      Offset[i] = (double) ((long) (ntohl(InfoPtr->info_filter.offset[i])/1000.0));
   }
   Dsp = (long) ntohl(InfoPtr->estdisp);      /* leave in milliseconds */
   Del = (long) ntohl(InfoPtr->estdelay);   /* leave in milliseconds */
   Off = (long) ntohl(InfoPtr->estoffset);   /* leave in milliseconds */
   C = ' ';
   Flags = ntohs(InfoPtr->flags);
   if (Flags & M_CMT_NTP_PEER_FL_CONFIG)
   {
      C = '-';      /* mark pre-configured */
   }
   if (Flags & M_CMT_NTP_PEER_FL_SANE)
   {
      C = '.';      /* passed sanity check */
   }
   if (Flags & M_CMT_NTP_PEER_FL_CANDIDATE)
   {
      C = '+';      /* made candidate list */
   }
   if (Flags & M_CMT_NTP_PEER_FL_SELECTED)
   {
      C = '*';      /* mark peer selection */
      RetVal = TRUE;
   }
   Sin.sin_addr.s_addr = InfoPtr->net_address;
   eLogDebug( "%c-%-16.16s %2d %4d  %03o  %8.1f %8.1f %8.1f",
          C, Sin.sin_addr.s_addr ? inet_ntoa(Sin.sin_addr) : "wildcard", 
          InfoPtr -> stratum, (int)ntohl((u_long)InfoPtr->timer), 
          ntohs(InfoPtr->reach) & M_CMT_NTP_SHIFT_MASK, Del, Off, Dsp);

   return RetVal;

}  

#endif /* #ifdef __GNUC__ */

