/*******************************************************************************
** Module Name:
**    CmtQnx.c
**
** Purpose:
**    QNX specific functions.
**
** Description:
**    The module of the CMT which interfaces only to QNX to extract system 
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
**    $Id: CmtQnx.c,v 0.3 2004/01/15 15:39:53 sxf Exp $
**
** History:
**    $Log: CmtQnx.c,v $
**    Revision 0.3  2004/01/15 15:39:53  sxf
**    Ported for Neutrino.
**
**    Revision 0.2  2002/08/01 11:08:40  sxf
**    Removed diagnostics etc.
**
**    Revision 0.1  2002/08/01 10:49:33  sxf
**     Initial version (taken from CmtInterface.c and CmtHardware.c).
**
**
*******************************************************************************/

#include "CmtPrivate.h"

#ifdef I_CMT_OS_QNX4

#include "Cmt.h"
#include "Log.h"
#include "Tim.h"

/*
** OS specific include files
*/
#include <conio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <i86.h>
#include <time.h>
#include <sys/kernel.h>
#include <sys/seginfo.h>
#include <sys/times.h>
#include <sys/osinfo.h>
#include <sys/osstat.h>
#include <sys/types.h>
#include <sys/disk.h>
#include <sys/psinfo.h>
#include <sys/sched.h>
#include <sys/pci.h>

void mCmtDelayNanosecs( Int32_t );
unsigned mCmtMAX1247ReadA2D( unsigned, char );
void mCmtMAX1247SetControl( unsigned, unsigned char );
void mCmtMAX1247ClearControl( unsigned, unsigned char );
void mCmtMAX1247SendByte( unsigned ,unsigned char );
unsigned char mCmtMAX1247GetByte( unsigned );

#define M_CMT_MAX1247_IOADDR            0xE4 /* Address for IO on MAX1247. */

#define M_CMT_TP400_A2D_CTL_START       0x80 /* Start bit - must be set.    */
#define M_CMT_TP400_A2D_CTL_SINGLE_END  0x04 /* Differential-zero this bit  */
#define M_CMT_TP400_A2D_CTL_UNIPOLAR    0x08 /* Bi-polar-zero this bit      */
#define M_CMT_TP400_A2D_CTL_EXT_CLK     0x03 /* Selects external clock      */
#define M_CMT_TP400_A2D_CTL_CH3_SEL     0x60 /* Channel 3 is temperature.   */

/*
** GPIO register 0 bits.
*/
#define M_CMT_GPIO0_A2DCSN              0x08 /* CS for MAX1247 */
#define M_CMT_GPIO0_SCLK                0x20 /* clock to MAX1247 */
#define M_CMT_GPIO0_DIN                 0x40 /* data in to MAX1247 */
#define M_CMT_GPIO0_DOUT                0x80 /* data out from MAX1247 */

/*
** Control byte to send to MAX1247.
*/
#define M_CMT_MAX1247_CTL_BYTE    M_CMT_TP400_A2D_CTL_START       |  \
                                  M_CMT_TP400_A2D_CTL_SINGLE_END  |  \
                                  M_CMT_TP400_A2D_CTL_UNIPOLAR    |  \
                                  M_CMT_TP400_A2D_CTL_EXT_CLK     |  \
                                  M_CMT_TP400_A2D_CTL_CH3_SEL   

/*
** Convert from volts to temperature in degrees celsius.
*/

#define M_CMT_MAX1247_VREF                   5.0  /* Reference voltage. */

/*
** Conversion formula that takes A2D raw reading and converts to volts and
** from volts to temperature in degrees C.
**
** Temp = ( Volts - 0.424 ) / 0.00625
** Volts = Reading * Ref_voltage / 4096.
**
*/ 
#define M_CMT_MAX1247_TEMP_FROM_READING( r ) \
          ((((double)r * M_CMT_MAX1247_VREF / 4096.0 )- 0.424) / 0.00625)

/*
** For Intel and ViA chipsets the address of the Smbus controller and the
** associated host config register are stored in these standard locations.
** The Smbus registers are then offset from the Smb base 
** These values are correct fo the Intel PIIX5 and VIA chipsets
*/
#define M_CMT_SMBBA           0x90    /* PCI Register holding Smb base address */
#define M_CMT_SMBHSTCFG       0XD2    /* PCI Register holding Smb host config. */
#define M_CMT_SMBHSTSTS       0       /* Smbus Host Status Register offset  */
#define M_CMT_SMBSLVSTS       1       /* Smbus Slave Status Register offset */
#define M_CMT_SMBHSTCNT       2       /* Smbus Host Control Register offset */
#define M_CMT_SMBHSTCMD       3       /* Smbus Host Command Register offset */
#define M_CMT_SMBHSTADD       4       /* Smbus Host Address Register offset */
#define M_CMT_SMBHSTDAT0      5       /* Smbus Host Data 0 Register offset */
#define M_CMT_SMBHSTDAT1      6       /* Smbus Host Data 1 Register offset */
#define M_CMT_SMBBLKDAT       7       /* Smbus Block Data Register offset  */

#define M_CMT_INVALID_FAN     0xFF    /* Invalid fan rotation count   */
#define M_CMT_ONE_ITEM        1       /* Obviously */
#define M_CMT_MS_DELAY_SHORT  750000  /* Short (0.75 ms) delay. */
#define M_CMT_MS_DELAY_LONG 10000000  /* Longer (10.0 ms) delay. */
#define M_CMT_MS_DELAY_10     10      /* Ten milli-second delay */

/*
** ASUSTECH & WINBOND conversion formulae
**
** The voltage A/D converter resolution is 16mV/bit
** Conversion values are based on recommended potential divider resistors. 
**
** ASUSTECH_NEG_5V factor should be -909.0 / 604.0 but -1000.0 gives
** correct result implying non-standard resistor values installed on board.
**
** Temperature1 is a 8-bit pos/neg value, resolution 1C/bit
** If bit 8 set then valueis negative.
** 
** Temperature2 is a 9-bit pos/neg value,  resolution 0.5C/bit 
** If the temperature is >512 then return 0
** If bit 9 set then temperature is negative then mask bit 9 and subtract 256
** 
*/
#define M_CMT_ASUSTECH_VOLT_CORE(v)    (v * 16.0) 
#define M_CMT_ASUSTECH_VOLT_REF(v)     (v * 16.0) 
#define M_CMT_ASUSTECH_VOLT_IO(v)      (v * 16.0) 
#define M_CMT_ASUSTECH_VOLT_POS_5V(v)  (v * 16.0 *  168.0 / 100.0) 
#define M_CMT_ASUSTECH_VOLT_POS_12V(v) (v * 16.0 *   38.0 /  10.0) 
#define M_CMT_ASUSTECH_VOLT_NEG_5V(v)  (v * 16.0 *-1000.0 / 604.0)  
#define M_CMT_ASUSTECH_VOLT_NEG_12V(v) (v * 16.0 *-2100.0 / 604.0)

#define M_CMT_ASUSTECH_TMP1(t)         (t & 128 ? t * -1000 : t * 1000 )
#define M_CMT_ASUSTECH_TMP2(t)         (t >= 511?0 : t&256?((t&255)-256)*500 : t*500)
#define M_CMT_ASUSTECH_TMP3(t)         (t >= 511?0 : t&256?((t&255)-256)*500 : t*500)

#define M_CMT_ASUSTECH_MAGIC           1000000 
                                     /* This gives give correct value */
                                     /* Compared with lm_sensors= 810000  */
                                     /* and Winbond = 135000              */    

#define M_CMT_WINBOND_VOLT_CORE(v)     (v * 16.0) 
#define M_CMT_WINBOND_VOLT_REF(v)      (v * 16.0) 
#define M_CMT_M_CMT_WINBOND_VOLT_IO(v)       (v * 16.0) 
#define M_CMT_WINBOND_VOLT_POS_5V(v)   (v * 16.0 * 168.0 / 100.0)  
#define M_CMT_WINBOND_VOLT_POS_12V(v)  (v * 16.0 *  38.0 /  10.0) 
#define M_CMT_WINBOND_VOLT_NEG_5V(v)   (v * 16.0 * 514.0 / 100.0 -  7710.0)
#define M_CMT_WINBOND_VOLT_NEG_12V(v)  (v * 16.0 * 314.0 / 100.0 - 14710.0)
#define M_CMT_WINBOND_TMP1(t)          (t & 128 ? t * -1000 : t * 1000 )
#define M_CMT_WINBOND_TMP2(t)          (t >= 511?0 : t&256?((t&255)-256)*500 : t*500)
#define M_CMT_WINBOND_TMP3(t)          (t >= 511?0 : t&256?((t&255)-256)*500 : t*500)

#define M_CMT_WINBOND_MAGIC            1350000

/*
** Genesys GL520sm chip conversion formulae
** The voltage A/D resolution is 19mV/bit except for +5 which is 23mV/bit
** Temperature is a 8-bit pos/neg value, resolution 1/bit 
*/
#define M_CMT_GL520SM_VOLT_CORE(v)     (v * 19.0) 
#define M_CMT_GL520SM_VOLT_REF(v)      (0.0) 
#define M_CMT_GL520SM_VOLT_IO(v)       (v * 19.0) 
#define M_CMT_GL520SM_VOLT_POS_5V(v)   (v * 23.0)  
#define M_CMT_GL520SM_VOLT_POS_12V(v)  (v * 19.0 * 197.0 / 47.0) 
#define M_CMT_GL520SM_VOLT_NEG_5V(v)   (0.0)
#define M_CMT_GL520SM_VOLT_NEG_12V(v)  (0.0)
#define M_CMT_GL520SM_TMP1(t)          (t >= 255 ? 0 : (t - 130) * 1000)
#define M_CMT_GL520SM_TMP2(t)          (t >= 255 ? 0 : (t - 130) * 1000)
#define M_CMT_GL520SM_TMP3(t)          (0)

#define M_CMT_GL520SM_MAGIC             960000 

/*
** Internally used chip ID's
*/
#define M_CMT_CHIP_INVALID  0     /* Not supported so don't even try */
#define M_CMT_CHIP_UNKNOWN  1     /* Unkown but looks useable        */
#define M_CMT_CHIP_ASUSTECH 2     /* Asus technologies               */
#define M_CMT_CHIP_WINBOND  3     /* Winbond                         */
#define M_CMT_CHIP_GL520SM  4     /* Genesys                         */ 


/*
** Recognised chip monitor Vendor ID's and chip ID's
*/
#define M_CMT_VENDID_NONE      0x0000
#define M_CMT_VENDID_WINBOND   0x5CA3 
#define M_CMT_VENDID_ASUSTECH  0x12C3
#define M_CMT_VENDID_GENESYS   0x05E3 

#define M_CMT_CHIPID_W83781D   0x10
#define M_CMT_CHIPID_W83627HF  0x20
#define M_CMT_CHIPID_W83782D   0x30
#define M_CMT_CHIPID_W83783S   0x40
#define M_CMT_CHIPID_AS99127F  0x30
#define M_CMT_CHIPID_GL520SM   0x20


/*
** Location of sensor chip control registers
** Refer to Winbond W83781D & ASUSTECH AS99127F reference document 
*/
#define M_CMT_REG_BANK_SELECT  0x4E   /* Bank select for switching between inputs */
#define M_CMT_REG_VENDOR_ID    0x4F   /* Manufacturer identity */
#define M_CMT_REG_CHIP_ID      0x58   /* Monitor chip identity */
        
#define M_CMT_REG_VID_LO       0x47   /* CPU core voltage identifier low-byte  */
#define M_CMT_REG_VID_HI       0x49   /* CPU core voltage identifier high-byte */

#define M_CMT_REG_FAN1         0x28   /* Fan pulse values */
#define M_CMT_REG_FAN2         0x29
#define M_CMT_REG_FAN3         0x2A

#define M_CMT_REG_DIV1         0x47   /* Fan rotation divisors */
#define M_CMT_REG_DIV2         0x47
#define M_CMT_REG_DIV3         0x4B

#define M_CMT_REG_TMP1         0x27   /* 8-bit and 9-bit temperature sensors  */   
#define M_CMT_REG_TMP2_HI      0x50   
#define M_CMT_REG_TMP2_LO      0x51
#define M_CMT_REG_TMP3_HI      0x50
#define M_CMT_REG_TMP3_LO      0x51

#define M_CMT_REG_VOLT_CORE    0x20   /* Voltage input sensors */
#define M_CMT_REG_VOLT_REF     0x21
#define M_CMT_REG_VOLT_IO      0x22
#define M_CMT_REG_VOLT_POS_5V  0x23
#define M_CMT_REG_VOLT_POS_12V 0x24
#define M_CMT_REG_VOLT_NEG_5V  0x25
#define M_CMT_REG_VOLT_NEG_12V 0x26

#define M_CMT_REG_SMB_FIRST_ADDRESS 0x2C   /* Regular chips can have one of  */
#define M_CMT_REG_SMB_FINAL_ADDRESS 0x2D   /* these possible Smbus addresses */

/*
** Location of GL5xx series registers
** Refer to Genesys GL520SM reference document 
*/                   
#define M_CMT_GL5_CHIP_ID      0x00
#define M_CMT_GL5_VID          0x01

#define M_CMT_GL5_PULSES_PER_REV  2        /* Kludge to get the correct GL5 rpm */
#define M_CMT_GL5_FAN1_2       0x07
#define M_CMT_GL5_DIV1         0x0F
#define M_CMT_GL5_DIV2         0x0F
#define M_CMT_GL5_TMP1         0x04
#define M_CMT_GL5_TMP2         0x0E
#define M_CMT_GL5_VOLT_CORE    0x0D
#define M_CMT_GL5_VOLT_IO      0x14
#define M_CMT_GL5_VOLT_POS_5V  0x15
#define M_CMT_GL5_VOLT_POS_12V 0x13

#define M_CMT_GL5_SMB_FIRST_ADDRESS 0x2C   /* The GL5xx chip can have one of */   
#define M_CMT_GL5_SMB_FINAL_ADDRESS 0x2D   /* these possible Smbus addresses */

/*
** Macros to calculate fan divisor
*/
#define M_CMT_GL5_CALC_DIV1( x )  ( 1 << ( ( x & 0xC0 ) >> 6 ) )
#define M_CMT_GL5_CALC_DIV2( x )  ( 1 << ( ( x & 0x30 ) >> 4 ) )


/*
** Bit masks which need to be applied to some register fields on the
** Smbus controller.
*/
#define M_CMT_MSK_QUICK_RW     0x40  
#define M_CMT_MSK_BYTE_RW      0x44      
#define M_CMT_MSK_BYTE_DATA_RW 0x48
#define M_CMT_MSK_WORD_DATA_RW 0x4C
   
#define M_CMT_MSK_READ         0x01     
#define M_CMT_MSK_WRITE        0x00
#define M_CMT_MSK_HOST_BUSY    0x01

#define M_CMT_MSK_VID_LO       0x0f   /* Voltage ID masks */     
#define M_CMT_MSK_VID_HI       0x01

#define M_CMT_MSK_DIV1         0x30   /* Fan divisors bit masks */
#define M_CMT_MSK_DIV2         0xC0
#define M_CMT_MSK_DIV3         0xC0

#define M_CMT_MSK_HBACS        0x80   /* Bank access bit masks */
#define M_CMT_MSK_BANK0        0x00
#define M_CMT_MSK_BANK1        0x01
#define M_CMT_MSK_BANK2        0x02

#define M_CMT_MSK_CHIP_ID      0xFE

/*
** Left and right shifts which need to be applied to some register fields
*/ 
#define M_CMT_RSH_DIV1            4
#define M_CMT_RSH_DIV2            6
#define M_CMT_RSH_DIV3            6

#define M_CMT_LSH_TMP2_HI         1
#define M_CMT_RSH_TMP2_LO         7
#define M_CMT_LSH_TMP3_HI         1
#define M_CMT_RSH_TMP3_LO         7

#define M_CMT_LSH_VID_LO          0
#define M_CMT_LSH_VID_HI          4

/* 
** Function prototypes with local scope only.
*/
static int mCmtSmbWriteByte(int addr, int value);
static int mCmtSmbReadByte (int addr);
static int mCmtSmbReadWord (int addr);
static int mCmtIOWriteByte (int addr, int value);
static int mCmtIOReadByte  (int addr);
static int mCmtIOReadWord  (int addr);
static int mCmtProbeSmbPort( void );
static int mCmtProbeIOPort ( void );
static int mCmtReadReg     ( int reg );
static void mCmtDelayNanosecs( Int32_t );
static unsigned mCmtMAX1247ReadA2D( unsigned, char );
static void mCmtMAX1247SetControl( unsigned, unsigned char );
static void mCmtMAX1247ClearControl( unsigned, unsigned char );
static void mCmtMAX1247SendByte( unsigned ,unsigned char );
static unsigned char mCmtMAX1247GetByte( unsigned );

/*
** Module specific variables
*/
static int  mCmtSmbHstSts;                 /* Host status register */       
static int  mCmtSmbSlvSts;                 /* Host slave status  register */
static int  mCmtSmbHstCnt;                 /* Host control register */
static int  mCmtSmbHstCmd;                 /* Host command register */
static int  mCmtSmbHstAdd;                 /* Host address register */
static int  mCmtSmbHstDat0;                /* Host data register - byte 0 */
static int  mCmtSmbHstDat1;                /* Host data register - byte 1 */
static int  mCmtSmbBlkDat;                 /* Block data register */
static int  mCmtSmbSlaveWrite;             /* Slave write register */
static int  mCmtSmbSlaveRead;              /* Slave read register */
static int  mCmtSmbPort    = 0;            /* Smb port base address on chipset */ 
static int  mCmtSmbEnabled = 0;            /* Smb port is enabled on chipset */
static int  mCmtIOAddr     = 0x295;        /* Monitor I/O address, address port */
static int  mCmtIOData     = 0x296;        /* Monitor I/O address, data port */
#if 0
static int  mCmtChip       = I_CMT_HWMON_AUTO; /* Chip ID internal to this process */  
#endif
static int  mCmtVendorID   = M_CMT_VENDID_NONE;  /* Manufacturer ID number */
static int  mCmtChipID     = 0;            /* Chip ID number         */
static char mCmtVendorName[32];            /* Manufacturer name      */
static char mCmtChipName  [32];            /* Monitor chip name      */
static char *mCmtMethodPtr;                /* Text description of access method */ 

static int ( *mCmtReadBytePtr  )( int Addr ) = NULL;
static int ( *mCmtReadWordPtr  )( int Addr ) = NULL;
static int ( *mCmtWriteBytePtr )( int Cmd, int Value ) = NULL;

/* 
** Define the supported PCI chipsets as defined by the Vendor ID
** and the PCI device. Also define the number of supported chips
*/
struct OKChip_s
{
   unsigned VendId;
   unsigned DevId;
} mCmtOKChips[] =
{
   { 0x1106, 0x3057 },   /* VIA chipset with ACPI   */
   { 0x8086, 0x7113 },   /* Intel chipset wih ACPI  */
};
#define M_CMT_CHIP_COUNT  (sizeof( mCmtOKChips ) / sizeof( struct OKChip_s ) ) 



/*******************************************************************************
** Function Name:
**    iCmtGetQnxCpuUsed
**
** Purpose:
**    Get the CPU usage information from the QNX system.
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

Status_t iCmtGetQnxCpuUsed( Int32_t *CpuUsedPtr, Int32_t *NumRet )
{
#define CPU_100_PERCENT 100000   /* Because we report in milli-percent */
#define CPU_50_PERCENT   50000   /* Because we report in milli-percent */
#define JIFFIE_PERCENT   100.0   /* Used to convert QNX jiffies to percent */
#define PSINFO_START         1   /* First process ID */
#define PSINFO_END          -1   /* No more processes */ 

   struct _psinfo Data;          /* Process information */
   pid_t  Id;                    /* Process identity    */

   time_t     NowIdle;           /* Current CPU idle time */ 
   eTtlTime_t NowTime;           /* Current time */
   eTtlTime_t DiffTime;          /* Time difference */
   double     Interval;          /* Time since last load check */      

   static  int        CpuFree = CPU_50_PERCENT;   /* Cpu free time */ 
   static  time_t     OldIdle = 0;                /* Previous CPU idle time */
   static  eTtlTime_t OldTime = {0,0};            /* Previous time */

   /*
   ** Search process list
   */  
   NowIdle = 0;
   for(Id = PSINFO_START; 
      (Id = qnx_psinfo( 0, Id, &Data, 0, 0 )) != PSINFO_END;
       Id++ )
   {
      /*
      ** If the process priority is 0 (null process) then sum CPU time
      */
      if ( !Data.priority )
      {
         NowIdle +=  Data.un.proc.times.tms_utime 
                   + Data.un.proc.times.tms_stime;         
      }
   }

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
      CpuFree = ( NowIdle - OldIdle ) * JIFFIE_PERCENT / Interval;
   }

   /*
   ** Save the current usage stats
   */
   OldTime = NowTime;
   OldIdle = NowIdle;
   
   /*
   ** If null processes are ever stopped and restarted then the usage numbers
   ** will become huge so clip to sensible limits
   */
   if ( CpuFree < 0 )
   {
      CpuFree = 0;
   }
   else if ( CpuFree > CPU_100_PERCENT )
   {
      CpuFree = CPU_100_PERCENT;
   }
   
   /*
   ** Return the CPU used load
   */
   
   *CpuUsedPtr = ( CPU_100_PERCENT - CpuFree );
   *NumRet     = 1;

   return SYS_NOMINAL;

} /* End of iCmtGetQnxCpuUsed */

/*******************************************************************************
** Function Name:
**    iCmtGetQnxDiskFree
**
** Purpose:
**    Get the disk free information from the QNX system.
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

Status_t iCmtGetQnxDiskFree( Int32_t *Disk1FreePtr,
                             Int32_t *Disk2FreePtr, 
                             Int32_t *Disk3FreePtr,
                             Int32_t *Disk4FreePtr,
                             Int32_t *NumRet )
{

   int       FileDesc;               /* File descriptor. */
   Status_t  Status = SYS_NOMINAL;   /* Return status - initially OK. */
   int       QnxRet;                 /* Return value from Qnx function. */
   long   FreeBlocks;                /* Free disk blocks */
   long   TotBlocks;                 /* Total disk block */      


   *NumRet = 0;  /* Initially set no of returned parameters to zero. */

   /*
   ** Determine the disk 1 space by opening the root directory
   */

   if ( iCmtData.NumDisksToMonitor >= 1 )
   {
      FileDesc = open( iCmtData.DisksToMonitor[ 0 ] , O_RDONLY );
      if ( FileDesc == I_CMT_OSCALL_FAILED )
      {
         Status = E_CMT_FILE_ERR;
         eLogWarning( Status,
                      "Failed to open disk1, \"%s\"",
                      iCmtData.DisksToMonitor[ 0 ] ); 
      }
      else
      {
         QnxRet = disk_space( FileDesc, &FreeBlocks, &TotBlocks );
         if (( QnxRet == I_CMT_OSCALL_FAILED )||    /* Function call failed or */
             ( TotBlocks == 0   )  )   /* total disk space zero */
         {
            Status = E_CMT_FILE_ERR;
            eLogWarning( Status,
                         "Failed reading disk1 space for disk, \"%s\"",
                         iCmtData.DisksToMonitor[ 0 ] ); 
         }
         else
         {
            *Disk1FreePtr = ( FreeBlocks * I_CMT_MILLIPERCENT ) 
                           / TotBlocks;
            (*NumRet)++;
              
         }
         close( FileDesc );
      }
   }
   
   /*
   ** Determine the disk 2 space by opening the root directory
   */
   if ( iCmtData.NumDisksToMonitor >= 2 )
   {
      FileDesc = open( iCmtData.DisksToMonitor[ 1 ] , O_RDONLY );
      if ( FileDesc == I_CMT_OSCALL_FAILED )
      {
         Status = E_CMT_FILE_ERR;
         eLogWarning( Status,
                      "Failed to open disk2, \"%s\"",
                      iCmtData.DisksToMonitor[ 1 ] ); 
      }
      else
      {
         QnxRet = disk_space( FileDesc, &FreeBlocks, &TotBlocks );
         if (( QnxRet == I_CMT_OSCALL_FAILED )||    /* Function call failed or */
             ( TotBlocks == 0   )  )   /* total disk space zero */
         {
            Status = E_CMT_FILE_ERR;
            eLogWarning( Status,
                         "Failed reading disk2 space for disk, \"%s\"",
                         iCmtData.DisksToMonitor[ 1 ] ); 
         }
         else
         {
            *Disk2FreePtr = ( FreeBlocks * I_CMT_MILLIPERCENT ) 
                              / TotBlocks;
            (*NumRet)++;
              
         }
         close( FileDesc );
      }
   }

   /*
   ** Determine the disk 3 space by opening the root directory
   */
   if ( iCmtData.NumDisksToMonitor >= 3 )
   {
      FileDesc = open( iCmtData.DisksToMonitor[ 2 ] , O_RDONLY );
      if ( FileDesc == I_CMT_OSCALL_FAILED )
      {
         Status = E_CMT_FILE_ERR;
         eLogWarning( Status,
                      "Failed to open disk3, \"%s\"",
                      iCmtData.DisksToMonitor[ 2 ] ); 
      }
      else
      {
         QnxRet = disk_space( FileDesc, &FreeBlocks, &TotBlocks );
         if (( QnxRet == I_CMT_OSCALL_FAILED )||    /* Function call failed or */
             ( TotBlocks == 0   )  )   /* total disk space zero */
         {
            Status = E_CMT_FILE_ERR;
            eLogWarning( Status,
                         "Failed reading disk3 space for disk, \"%s\"",
                         iCmtData.DisksToMonitor[ 2 ] ); 
         }
         else
         {
            *Disk3FreePtr = ( FreeBlocks * I_CMT_MILLIPERCENT ) 
                              / TotBlocks;
            (*NumRet)++;
         }     
      }
      close( FileDesc );
   }

   /*
   ** Determine the disk 3 space by opening the root directory
   */
   if ( iCmtData.NumDisksToMonitor >= 4 )
   {
      FileDesc = open( iCmtData.DisksToMonitor[ 3 ] , O_RDONLY );
      if ( FileDesc == I_CMT_OSCALL_FAILED )
      {
         Status = E_CMT_FILE_ERR;
         eLogWarning( Status,
                      "Failed to open disk4, \"%s\"",
                      iCmtData.DisksToMonitor[ 3 ] ); 
      }
      else
      {
         QnxRet = disk_space( FileDesc, &FreeBlocks, &TotBlocks );
         if (( QnxRet == I_CMT_OSCALL_FAILED )||    /* Function call failed or */
             ( TotBlocks == 0   )  )   /* total disk space zero */
         {
            Status = E_CMT_FILE_ERR;
            eLogWarning( Status,
                         "Failed reading disk4 space for disk, \"%s\"",
                         iCmtData.DisksToMonitor[ 3 ] ); 
         }
         else
         {
            *Disk4FreePtr = ( FreeBlocks * I_CMT_MILLIPERCENT ) 
                              / TotBlocks;
            (*NumRet)++;
         }     
      }
      close( FileDesc );
   }

   return Status;

}

/*******************************************************************************
** Function Name:
**    iCmtGetQnxMemoryFree
**
** Purpose:
**    Get the memory free information from the QNX system.
**
** Description:
**    This function determines memory usage of the host QNX system.
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

Status_t iCmtGetQnxMemoryFree( Int32_t *MemFreePtr,
                               Int32_t *NumRet )
{

   int       QnxStat;     /* Status returned from QNX function. */
   Status_t  Status;      /* TTL status code. */
   struct _osinfo OsInfo; /* QNX operating system information */

   /*
   ** Get the QNX operating system information
   */
   QnxStat = qnx_osinfo( 0, &OsInfo );

   if (( QnxStat != 0 ) ||          /* Function call failed or */
       ( OsInfo.totpmem == 0)  )    /* total memory is zero    */
   {
      Status = E_CMT_NO_OSINFO;
      eLogWarning( Status, "Failed to get OS information." ); 
      *NumRet = 0;
   }
   else
   {
      /*
      ** Calculate the free memory in milli-percent
      */  

      *MemFreePtr = ( OsInfo.freepmem * I_CMT_MILLIPERCENT )
                          / OsInfo.totpmem;

      *NumRet     = 1;
      Status      = SYS_NOMINAL;
   }

   return SYS_NOMINAL;

}

/*******************************************************************************
** Function Name:
**    iCmtGetQnxTemperature
**
** Purpose:
**    Get the temperature information from the QNX system.
**
** Description:
**    This function determines up to three measured temperatures.
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

Status_t iCmtGetQnxTemperature( Int32_t *Temp1Ptr,
                                Int32_t *Temp2Ptr,
                                Int32_t *Temp3Ptr,
                                Int32_t *NumRet )
{

   int  Tmp1       = 0;       /* Temperature readings in milli-degrees C */
   int  Tmp2       = 0;
   int  Tmp3       = 0;

    /*
   ** Perform the read and data conversions appropriate for the chip
   */

   switch ( iCmtData.HwMonitorChip )
   {
      case I_CMT_HWMON_AS99127F:
      {
         /*
         ** Read temperatures
         */
         mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK2 );

         Tmp3 = ( mCmtReadBytePtr( M_CMT_REG_TMP3_HI ) << M_CMT_LSH_TMP3_HI ) +
                ( mCmtReadBytePtr( M_CMT_REG_TMP3_LO ) >> M_CMT_RSH_TMP3_LO );

         *Temp3Ptr = M_CMT_ASUSTECH_TMP3( Tmp3 );
      
         mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK1 );

         Tmp2 = ( mCmtReadBytePtr( M_CMT_REG_TMP2_HI ) << M_CMT_LSH_TMP2_HI ) +
                ( mCmtReadBytePtr( M_CMT_REG_TMP2_LO ) >> M_CMT_RSH_TMP2_LO );

         *Temp2Ptr = M_CMT_ASUSTECH_TMP2( Tmp2 );
      
         mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK0 );
         
         Tmp1 = mCmtReadBytePtr( M_CMT_REG_TMP1 );

         *Temp1Ptr = M_CMT_ASUSTECH_TMP1( Tmp1 );

         *NumRet = 3;
         
         break;
      
      }
      case I_CMT_HWMON_W83XXXXX:
      {

         /*
         ** Read temperatures
         */
         mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK2 );

         Tmp3 = ( mCmtReadBytePtr( M_CMT_REG_TMP3_HI ) << M_CMT_LSH_TMP3_HI ) +
                ( mCmtReadBytePtr( M_CMT_REG_TMP3_LO ) >> M_CMT_RSH_TMP3_LO );

         *Temp3Ptr = M_CMT_WINBOND_TMP3( Tmp3 );

         mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK1 );

         Tmp2 = ( mCmtReadBytePtr( M_CMT_REG_TMP2_HI ) << M_CMT_LSH_TMP2_HI ) +
                ( mCmtReadBytePtr( M_CMT_REG_TMP2_LO ) >> M_CMT_RSH_TMP2_LO );
         *Temp2Ptr = M_CMT_WINBOND_TMP2( Tmp2 );

         mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK0 );
         Tmp1 = mCmtReadBytePtr( M_CMT_REG_TMP1 );

         *Temp1Ptr = M_CMT_WINBOND_TMP1( Tmp1 );

         *NumRet = 3;

         break;
      }
      case I_CMT_HWMON_GL520SM:
      {
         /* 
         ** Get temperatures - Tmp3 not supported
         */
         mCmtReadBytePtr( M_CMT_GL5_TMP1 );
         mCmtReadBytePtr( 0 );
         
         Tmp1 = mCmtReadBytePtr( 0 );

         *Temp1Ptr = M_CMT_GL520SM_TMP1( Tmp1 );

         mCmtReadBytePtr( M_CMT_GL5_TMP2 );
         mCmtReadBytePtr( 0 );

         Tmp2 = mCmtReadBytePtr( 0 );

         *Temp2Ptr = M_CMT_GL520SM_TMP2( Tmp2 );

         *NumRet = 2;

         break;

      }
      case I_CMT_HWMON_MAX1247:
      {
         /* 
         ** Get temperatures - Tmp1 only supported
         */
         Tmp1 = mCmtMAX1247ReadA2D( M_CMT_MAX1247_IOADDR, 
                                    M_CMT_MAX1247_CTL_BYTE );

         *Temp1Ptr = (int)( 1000.0 * M_CMT_MAX1247_TEMP_FROM_READING ( Tmp1 ) );

         *NumRet = 1;

         break;
      }
      default:
      {
         *NumRet = 0;

         break;
      }

   }

   return SYS_NOMINAL;

}

/*******************************************************************************
** Function Name:
**    iCmtGetQnxFanSpeed
**
** Purpose:
**    Get the fan speed information from the QNX system.
**
** Description:
**    This function determines up to three measured fan speeds.
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

Status_t iCmtGetQnxFanSpeed( Int32_t *Fan1Ptr,
                             Int32_t *Fan2Ptr,
                             Int32_t *Fan3Ptr,
                             Int32_t *NumRet )
{

   int RpmFan1     = 0;
   int RpmFan2     = 0;

   unsigned char  DivFan1 = 0; /* Fan divisor values */
   unsigned char  DivFan2 = 0;
   unsigned char  DivFan3 = 0;
   unsigned short Word    = 0; /* General read word */

   /*
   ** Perform the read and data conversions appropriate for the chip
   */
   switch ( iCmtData.HwMonitorChip )
   {
      case I_CMT_HWMON_AS99127F:
      {
         /*
         ** Ensure BANK0 is intially selected
         */
         mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK0 );

         /*
         ** Read the fan divisors
         */
         DivFan1 = ( ( mCmtReadBytePtr( M_CMT_REG_DIV1 ) & M_CMT_MSK_DIV1 )
                      >> M_CMT_RSH_DIV1 );
         DivFan2 = ( ( mCmtReadBytePtr( M_CMT_REG_DIV2 ) & M_CMT_MSK_DIV2)
                      >> M_CMT_RSH_DIV2 );
         DivFan3 = ( ( mCmtReadBytePtr( M_CMT_REG_DIV3 ) & M_CMT_MSK_DIV3 )
                      >> M_CMT_RSH_DIV3 );

         /*
         ** Read raw fan and convert to RPM
         */
         *Fan1Ptr = mCmtRpmAsustech( mCmtReadBytePtr( M_CMT_REG_FAN1 ), DivFan1 );

         *Fan2Ptr = mCmtRpmAsustech( mCmtReadBytePtr( M_CMT_REG_FAN2 ), DivFan2 );

         *Fan3Ptr = mCmtRpmAsustech( mCmtReadBytePtr( M_CMT_REG_FAN3 ), DivFan3 );

         *NumRet = 3;

        break;
      }
      case I_CMT_HWMON_W83XXXXX:
      {
         /* 
         ** Ensure BANK0 is intially selected
         */
         mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK0 );

         /*
         ** Get fan divisor values
         */
         DivFan1 = ( ( mCmtReadBytePtr( M_CMT_REG_DIV1 ) & M_CMT_MSK_DIV1 )
                      >> M_CMT_RSH_DIV1 );
         DivFan2 = ( ( mCmtReadBytePtr( M_CMT_REG_DIV2 ) & M_CMT_MSK_DIV2 ) 
                      >> M_CMT_RSH_DIV2 );
         DivFan3 = ( ( mCmtReadBytePtr( M_CMT_REG_DIV3 ) & M_CMT_MSK_DIV3 ) 
                      >> M_CMT_RSH_DIV3 );

         /*
         ** Get fan rpm and convert to rpm
         */
         *Fan1Ptr = mCmtRpmWinbond( mCmtReadBytePtr( M_CMT_REG_FAN1 ), DivFan1 );

         *Fan2Ptr = mCmtRpmWinbond( mCmtReadBytePtr( M_CMT_REG_FAN2 ), DivFan2 );

         *Fan3Ptr = mCmtRpmWinbond( mCmtReadBytePtr( M_CMT_REG_FAN3 ), DivFan3 );

        *NumRet = 3;

        break;
      }
      case I_CMT_HWMON_GL520SM:
      {
         /*
         ** Get fan divisor values
         */ 
         mCmtReadBytePtr( M_CMT_GL5_DIV1 );
         mCmtReadBytePtr( 0 );
         DivFan1 = M_CMT_GL5_CALC_DIV1( mCmtReadBytePtr( 0 ) );

         mCmtReadBytePtr( M_CMT_GL5_DIV2 );
         mCmtReadBytePtr( 0 );
         DivFan2 = M_CMT_GL5_CALC_DIV2( mCmtReadBytePtr( 0 ) );

         /*
         ** Get fan rpm - No Fan3, Fan1 & Fan2 stored in same word
         */
         mCmtReadBytePtr( M_CMT_GL5_FAN1_2 );
         mCmtReadBytePtr( 0 );
         Word    = mCmtReadWordPtr( 0 );
         RpmFan1 = UCHAR_MAX & (Word >> CHAR_BIT);
         RpmFan2 = UCHAR_MAX &  Word;
 
         *Fan1Ptr = mCmtRpmGl520sm( RpmFan1, DivFan1 );

         *Fan2Ptr = mCmtRpmGl520sm( RpmFan2, DivFan2 );

         *NumRet = 2;

         break;
      }
      case M_CMT_CHIP_INVALID:
      default:
      {

         *NumRet = 0;

         break;

      }

   }

   return SYS_NOMINAL;

}

/*******************************************************************************
** Function Name:
**    iCmtDetectHardware
**
** Purpose:
**    Detects the presence of a supported hardware monitoring chip
**
** Description:
**    IO is tried first then if an active controller is present on the
**    PCI chipset the Smbus is probed.
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
   int i;

   unsigned char  LoByte;  /* General purpose least significant byte */
   unsigned char  HiByte;  /* General purpose most significant byte  */

   if ( iCmtData.HwMonitorChip == I_CMT_HWMON_AUTO )
   {
      /*
      ** Hardware monitor may be on either an IO port or Smbus
      ** So probe the IO port first for Asustech or Winbond chip
      ** Then try the Smbus for Asustech, Winbond or Genesys
      **  If no hardware monitor found then give up
      */
      if ( mCmtProbeIOPort() )
      {
         /*
         ** Use IO port method
         */
         eLogDebug("Using IO port method");
         mCmtReadBytePtr  = mCmtIOReadByte;
         mCmtWriteBytePtr = mCmtIOWriteByte;
         mCmtReadWordPtr  = mCmtIOReadWord;
         mCmtMethodPtr = "IOport";
      }
      else if ( mCmtProbeSmbPort() )
      {
         /*
         ** Use Smbus method
         */
         eLogDebug("Using Smbus method");
         mCmtReadBytePtr  = mCmtSmbReadByte;
         mCmtWriteBytePtr = mCmtSmbWriteByte;
         mCmtReadWordPtr  = mCmtSmbReadWord;
         mCmtMethodPtr = "Smbus";
      }
      else
      {
         /*
         ** No suitable chip found so disable HW monitoring
         */
         iCmtData.HwMonitorChip      = M_CMT_CHIP_INVALID; 
         mCmtMethodPtr = "None";
         eLogInfo( "Method=None - no hardware monitoring" );
         return FALSE;
      }

      /*
      ** Initially no valid vendor has been found
      */
      mCmtVendorID = M_CMT_VENDID_NONE;
    
      /*
      ** Look for a Genesys GL520SM chip first because it's least dangerous
      ** Check both the possible addresses 0x2D and 0x2D
      */
      if ( mCmtVendorID == M_CMT_VENDID_NONE )
      {
         for ( i  = M_CMT_GL5_SMB_FIRST_ADDRESS;
               i <= M_CMT_GL5_SMB_FINAL_ADDRESS; i++ )
         {
            mCmtInitSmbus( i );

            /* 
            ** The GL5xx chip are addressed using an unusual, kludgey, method.
            ** The first read sets the address.
            ** The second read gets the value of the address just written.
            ** The third gets the actual data.
            */
            mCmtReadBytePtr( M_CMT_GL5_CHIP_ID );
            mCmtReadBytePtr( 0 );
            mCmtChipID = mCmtReadBytePtr( 0 );
            if ( mCmtChipID == M_CMT_CHIPID_GL520SM )
            {
               /*
               ** This chip does not supply a vendor ID so assign it
               */ 
               mCmtVendorID = M_CMT_VENDID_GENESYS;
               break;
            }
         }
      }

      /*
      ** If still not found a supported chip then check for ASUS/WINBOND
      */
      if ( mCmtVendorID == M_CMT_VENDID_NONE )  
      {
         for ( i  = M_CMT_REG_SMB_FIRST_ADDRESS;
               i <= M_CMT_REG_SMB_FINAL_ADDRESS; i++ ) 
         {
            mCmtInitSmbus( i );  
            /*
            ** Look for Asustech & Winbond chips, First the manufacturer identity
            */
            mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_HBACS );
            HiByte = mCmtReadBytePtr( M_CMT_REG_VENDOR_ID );
            mCmtWriteBytePtr( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK0 ); 
            LoByte = mCmtReadBytePtr( M_CMT_REG_VENDOR_ID );
            mCmtVendorID = ( HiByte << CHAR_BIT ) + LoByte;

            /*
            ** Then get the chip identity
            */
            mCmtChipID = mCmtReadBytePtr( M_CMT_REG_CHIP_ID ) & M_CMT_MSK_CHIP_ID;
       
            /*
            ** If known vendor ID then probably OK so look no further
            */  
            if ( ( mCmtVendorID == M_CMT_VENDID_WINBOND  )|| 
                 ( mCmtVendorID == M_CMT_VENDID_ASUSTECH )  )
               break;
         }
      }

      /*
      ** Convert Vendor ID to text name
      */
      if       ( mCmtVendorID ==  M_CMT_VENDID_WINBOND  )
         strcpy( mCmtVendorName, "WINBOND" );
      else if  ( mCmtVendorID ==  M_CMT_VENDID_ASUSTECH )
         strcpy( mCmtVendorName, "ASUSTECH");
      else if  ( mCmtVendorID ==  M_CMT_VENDID_GENESYS  )
         strcpy( mCmtVendorName, "GENESYS" );    
      else
         sprintf(mCmtVendorName, "0x%4.4x", mCmtVendorID);

      /*
      ** Convert the Chip ID to a text name 
      */
      if ( (mCmtVendorID == M_CMT_VENDID_WINBOND) &&
           (mCmtChipID  ==  M_CMT_CHIPID_W83781D ) )
      {
         iCmtData.HwMonitorChip = I_CMT_HWMON_W83XXXXX;
         strcpy( mCmtChipName,  "W83781D" );
      }   
      else if ( ( mCmtVendorID == M_CMT_VENDID_WINBOND ) &&
                ( mCmtChipID  ==  M_CMT_CHIPID_W83627HF ) )
      {
         iCmtData.HwMonitorChip = I_CMT_HWMON_W83XXXXX;
         strcpy( mCmtChipName,  "W83627HF");
      }
      else if ( (mCmtVendorID == M_CMT_VENDID_WINBOND) &&
                (mCmtChipID  == M_CMT_CHIPID_W83782D ) )
      {
         iCmtData.HwMonitorChip = I_CMT_HWMON_W83XXXXX;
         strcpy( mCmtChipName,  "W83782D" );
      }
      else if  ( (mCmtVendorID == M_CMT_VENDID_WINBOND) &&
                 (mCmtChipID  == M_CMT_CHIPID_W83783S ) )
      {
         iCmtData.HwMonitorChip = I_CMT_HWMON_W83XXXXX;
         strcpy( mCmtChipName,  "W83783S" );
      }
      else if  ( (mCmtVendorID == M_CMT_VENDID_ASUSTECH) &&
                 (mCmtChipID == M_CMT_CHIPID_AS99127F) )
      {
         iCmtData.HwMonitorChip = I_CMT_HWMON_AS99127F;
         strcpy( mCmtChipName,  "AS99127F");
      }
      else if ( (mCmtVendorID == M_CMT_VENDID_GENESYS) &&
                (mCmtChipID == M_CMT_CHIPID_GL520SM ) )
      {
         iCmtData.HwMonitorChip = I_CMT_HWMON_GL520SM;
         sprintf( mCmtChipName, "GL520SM" );
      }
      else
      {
         iCmtData.HwMonitorChip = I_CMT_HWMON_NONE;
         sprintf( mCmtChipName, "0x%2.2x", mCmtChipID );
      }
   
      eLogInfo("Access method=%s Vendor=%s Chip=%s",
                mCmtMethodPtr, mCmtVendorName, mCmtChipName );
   }
   else
   {

#if __SXF__
      /* 
      ** If method for hardware monitoring is "NONE" then
      ** make sure none is done by setting chip to I_CMT_HWMON_NONE.
      */

      if ( iCmtData.HwMonitorMethod == I_CMT_HWMETHOD_NONE )
      {
         iCmtData.HwMonitorChip = I_CMT_HWMON_NONE;
      }
#endif

      if ( iCmtData.HwMonitorMethod == I_CMT_HWMETHOD_IOPORT )
      {
         /*
         ** Use IO port method
         */
         if ( mCmtProbeIOPort() )
         {
            eLogDebug("Using IO port method");
            mCmtReadBytePtr  = mCmtIOReadByte;
            mCmtWriteBytePtr = mCmtIOWriteByte;
            mCmtReadWordPtr  = mCmtIOReadWord;
            mCmtMethodPtr = "IOport";
         }
         else
         {
            eLogInfo( "Failed to find IO port. No hardware monitoring" );
            /*
            ** Set method and chip to NONE so that no attemt is made to talk
            ** to unknown hardware.
            */
            iCmtData.HwMonitorMethod = I_CMT_HWMETHOD_NONE;
            iCmtData.HwMonitorChip = I_CMT_HWMON_NONE;
         }
      }
      else if ( iCmtData.HwMonitorMethod == I_CMT_HWMETHOD_SMBUS )
      {
         /*
         ** Use Smbus method
         */

         if ( mCmtProbeSmbPort() )
         {
            eLogDebug("Using Smbus method");
            mCmtReadBytePtr  = mCmtSmbReadByte;  
            mCmtWriteBytePtr = mCmtSmbWriteByte;
            mCmtReadWordPtr  = mCmtSmbReadWord;
            mCmtInitSmbus( M_CMT_REG_SMB_FIRST_ADDRESS );
            mCmtInitSmbus( M_CMT_REG_SMB_FINAL_ADDRESS );
            mCmtMethodPtr = "Smbus";
         }
         else
         {
            eLogInfo( "Failed to find SMB port. No hardware monitoring" );
            /*
            ** Set method and chip to NONE so that no attemt is made to talk
            ** to unknown hardware.
            */
            iCmtData.HwMonitorMethod = I_CMT_HWMETHOD_NONE;
            iCmtData.HwMonitorChip = I_CMT_HWMON_NONE;

         }
      }
      /*
      ** MAX1247 uses a non-generic method for talking to hardware so
      ** we treat it as a special case here.
      */
      else if ( iCmtData.HwMonitorChip != I_CMT_HWMON_MAX1247 )
      {
         /*
         ** No suitable chip found so disable HW monitoring
         */
         mCmtMethodPtr = "None";
         eLogInfo( "Method=None - no hardware monitoring" );
         return FALSE;
      }

   }

   return TRUE;

}  /* End of iCmtDetectHardware() */

/*******************************************************************************
** Function Name:
**    mCmtProbeIOPort
**
** Purpose:
**    Determine if a supported hardware monitor chip is mapped to the
**    standard I/O port
**
** Description:
**    Attempts to read the Vendor ID and Chip using the I/O port most
**    often mapped the hardware monitor chip.
**
** Return Type:
**    int
**         Indicates whether a supported chip was found, TRUE/FALSE
**          
** Arguments:
**    None
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtProbeIOPort( void )
{ 
   unsigned char LoByte;
   unsigned char HiByte;
   int           VendorId;

   /*
   ** Read Vendor ID from the standard IO port 0x290 (+offsets) 
   */
   eLogDebug( "Probing IO port" );

   mCmtIOWriteByte( M_CMT_REG_BANK_SELECT, M_CMT_MSK_HBACS ); 
   eLogDebug( "Write byte 1" );
   HiByte = mCmtIOReadByte( M_CMT_REG_VENDOR_ID );
   eLogDebug( "Read byte 1" );

   mCmtIOWriteByte( M_CMT_REG_BANK_SELECT, M_CMT_MSK_BANK0 );
   eLogDebug( "Write byte 2" );
   LoByte = mCmtIOReadByte( M_CMT_REG_VENDOR_ID );
   eLogDebug( "Read byte 2" );

   VendorId = ( HiByte << CHAR_BIT ) + LoByte;
 
   /*
   ** Check if the Vendor is supported
   */
   switch( VendorId )
   {
      case M_CMT_VENDID_WINBOND:
      case M_CMT_VENDID_ASUSTECH:
         mCmtReadBytePtr  = mCmtIOReadByte;
         mCmtWriteBytePtr = mCmtIOWriteByte;
         return TRUE;
         break;

      default:
         return FALSE;
         break;
   }
}   /* End of mCmtProbeIOPort. */

/*******************************************************************************
** Function Name:
**    mCmtProbeSmbPort
**
** Purpose:
**    Determine if a supported PCI chipset is present.
**
** Description:
**    Verify the the BIOS and OS support PCI devices.
**    Search for a supported southbridge PCI chipset and determine the
**    address of the Smbus controller and check if it is enabled.
**
** Return Type:
**    int
**         Indicates whether an enabled Smbus was found TRUE/FALSE
**          
** Arguments:
**    None
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtProbeSmbPort( void )
{
   unsigned BusNum;     /* PCI bus number */
   unsigned DevFn;      /* PCI Device=bits 5:3  PCI Function=bits 2:0 */
   unsigned LastBus;    /* Last bus found */
   unsigned Version;    /* PCI version number */
   unsigned Hardware;   /* PCI Hardware ID */
      
   int Chip;
   int Status;

   struct _osinfo OsInfo;

   /*
   ** Read OS specific information
   */
   if( qnx_osinfo(0, &OsInfo) == -1 )
      return FALSE;

   eLogDebug("OS info read.");

   /*
   ** Check if PCI BIOS is supported 
   */
   if( !( OsInfo.sflags & _PSF_PCI_BIOS ))
      return FALSE;

   eLogDebug("PCI BIOS is supported");

   /*
   ** Check if a PCI BIOS/bus is present
   */
   if( _CA_PCI_BIOS_Present( &LastBus, &Version, &Hardware) != PCI_SUCCESS )
      return FALSE;

   eLogDebug("PCI BIOS/bus is present" );

   /*
   ** Look for valid chipsets
   */
   for( Chip = M_CMT_CHIP_COUNT; Chip--; )
   {
      /*
      ** Look for a supported Device & Vendor
      */
      Status = _CA_PCI_Find_Device( mCmtOKChips[Chip].DevId, 
                                    mCmtOKChips[Chip].VendId, 
                                    0, &BusNum, &DevFn);
      /*
      ** Found one so look no further 
      */
      if ( Status == PCI_SUCCESS )
      {
         eLogDebug("Found PCI device with DevId %Xh = , VendId = %Xh",
                                      mCmtOKChips[Chip].DevId,
                                      mCmtOKChips[Chip].VendId );
         /*
         ** Read the SMB base address
         */
         _CA_PCI_Read_Config_Word( BusNum, DevFn, 
                   M_CMT_SMBBA, M_CMT_ONE_ITEM, (char*)&mCmtSmbPort);
   
         /* 
         ** Read the SMB host configuration 
         */
         _CA_PCI_Read_Config_Byte( BusNum, DevFn, 
                   M_CMT_SMBHSTCFG, M_CMT_ONE_ITEM, (char*)&mCmtSmbEnabled);

         /*
         ** Mask off the relevant bits
         */
         mCmtSmbPort    &= 0xffff0;
         mCmtSmbEnabled &= 0x01;

         eLogInfo("PCI Ver=%d.%02x ", (Version >> 8) & 0xff, Version & 0xff);
         eLogInfo("BusNum=%x DevFn=%x SmbBase=%x SmbEnabled=%i", 
                   BusNum, DevFn, mCmtSmbPort, mCmtSmbEnabled );
         break;
      }
   }

   if ( mCmtSmbEnabled )
   {
      return TRUE;      
   }
   else
   {
      /*
      ** Failed to find Smb Port
      */
      return FALSE;
   }
} /* End of mCmtProbeSmbPort. */

/*******************************************************************************
** Function Name:
**    mCmtRpmAsustech
**
** Purpose:
**    Calculate the RPM value of a fan connected to a Winbond chip.
**
** Description:
**    If the supplied pulse width is not in range return 0
**    else calculate the equivalent RPM.
**    Refer to chip manufacturer doco. for details on calculation. 
**
** Return Type:
**    int
**        The fan RPM value  
**
** Arguments:
**    int   (in)
**        A pulse-width proportion to the fan rotation speed.
**
**    int   (in)
**        A divisor value supplied the by monitor chip 
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtRpmAsustech( int Fan, int Divisor )
{
   /*
   ** The Fan RPM is calculated using a magic formula supplied by the chip
   ** manufacturer. 
   ** RPM = MAGIC / ( FanCount * (1 + Divisor) )   
   **
   ** Also a fan count of 0xFF indicates invalid fan speed
   */
   if (( Fan      >= M_CMT_INVALID_FAN )||   /* Fan not moving */
       ( Fan      <= 0           )||   /* Failed to read fan register */
       (++Divisor <= 0           )  )  /* Divisor overflow */ 
   {
      return 0;
   }
   else
   {
      return( M_CMT_ASUSTECH_MAGIC / ( Divisor * Fan ) );
   }
}


/*******************************************************************************
** Function Name:
**    mCmtRpmGl502sm
**
** Purpose:
**    Calculate the RPM value of a fan connected to a Winbond chip.
**
** Description:
**    If the supplied pulse width is not in range return 0
**    else calculate the equivalent RPM.
**    Refer to chip manufacturer doco. for details on calculation. 
**
** Return Type:
**    int
**        The fan RPM value  
**
** Arguments:
**    int   (in)
**        A pulse-width proportion to the fan rotation speed.
**
**    int   (in)
**        A divisor value supplied the by monitor chip 
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtRpmGl520sm( int Fan, int Divisor )
{

   /*
   ** The Fan RPM is calculated using a magic formula supplied by the chip
   ** manufacturer. 
   ** RPM = MAGIC / ( FanCount * Divisor * PULSES_PER_REV )   
   **
   ** Also a fan count of 0xFF indicates invalid fan speed
   */
   if (( Fan      >= M_CMT_INVALID_FAN )||   /* Fan not moving */
       ( Fan      <= 0           )||   /* Failed to read fan register */
       ( Divisor  <= 0           )  )  /* Divisor overflow */ 
   {
      return 0;
   }
   else
   {
      return( M_CMT_GL520SM_MAGIC / ( Divisor * Fan * M_CMT_GL5_PULSES_PER_REV ) );
   }
}


/*******************************************************************************
** Function Name:
**    mCmtRpmWinbond
**
** Purpose:
**    Calculate the RPM value of a fan connected to a Winbond chip.
**
** Description:
**    If the supplied pulse width is not in range return 0
**    else calculate the equivalent RPM.
**    Refer to chip manufacturer doco. for details on calculation. 
**
** Return Type:
**    int
**        The fan RPM value  
**
** Arguments:
**    int   (in)
**        A pulse-width proportion to the fan rotation speed.
**
**    int   (in)
**        A divisor value supplied the by monitor chip 
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtRpmWinbond( int Fan, int Divisor )
{
 
   /*
   ** The Fan RPM is calculated using a magic formula supplied by the chip
   ** manufacturer. 
   ** RPM = MAGIC / ( FanCount * (1 + Divisor) )   
   **
   ** Also a fan count of 0xFF indicates invalid fan speed
   */
   if (( Fan      >= M_CMT_INVALID_FAN )||   /* Fan not moving */
       ( Fan      <= 0           )||   /* Failed to read fan register */
       (++Divisor <= 0           )  )  /* Divisor underflow */ 
   {
      return 0;
   }
   else
   {
      return( M_CMT_WINBOND_MAGIC / ( Divisor * Fan ) );
   }
}


/*******************************************************************************
** Function Name:
**    InitSmbus
**
** Purpose:
**    Initialise the variables usd to communicate to the Smbus controller.
**
** Description:
**    Initialises variables that define the address of the hardware monitor 
**    the Smbus and those that define the location of the Smb controller on
**    in I/O space.
**    NB the slave address is 7-bit and for hardware monitor chips is 
**    in the range 0x20-0x2f ( usually it either 0x2C or 2x2D ) 
**
** Return Type:
**    None
**
** Arguments:
**    int   (in)
**        The address of the slave device i.e. the hardware monitor on the
**        Smbus.
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

void mCmtInitSmbus( int Slave )
{
   /*
   ** Create the slave read & write addresses
   */
   mCmtSmbSlaveRead  = (Slave << 1) | 1; 
   mCmtSmbSlaveWrite = (Slave << 1) | 0;

   /* Initialise the global variables used to talk to the Smbus */
   mCmtSmbHstSts  = mCmtSmbPort + M_CMT_SMBHSTSTS;
   mCmtSmbSlvSts  = mCmtSmbPort + M_CMT_SMBSLVSTS;
   mCmtSmbHstCnt  = mCmtSmbPort + M_CMT_SMBHSTCNT;
   mCmtSmbHstCmd  = mCmtSmbPort + M_CMT_SMBHSTCMD;
   mCmtSmbHstAdd  = mCmtSmbPort + M_CMT_SMBHSTADD;
   mCmtSmbHstDat0 = mCmtSmbPort + M_CMT_SMBHSTDAT0;
   mCmtSmbHstDat1 = mCmtSmbPort + M_CMT_SMBHSTDAT1;
   mCmtSmbBlkDat  = mCmtSmbPort + M_CMT_SMBBLKDAT;
}


/*******************************************************************************
** Function Name:
**    mCmtSmbWriteByte
**
** Purpose:
**    Smbus write byte of data to a register on the hardware monitor chip.
**
** Description:
**    The Smbus controller registers are configured in the correct order.
**
** Return Type:
**    int
**        Value written to hardware monitor register
**
** Arguments:
**    int   (in)
**        The register to be written to.
**
**    int   (in)
**        The value to be written to the register.
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtSmbWriteByte( int Cmd, int Value)
{
   outp( mCmtSmbHstCmd,  Cmd );               /* The register to be read */
   outp( mCmtSmbHstAdd,  mCmtSmbSlaveWrite );     /* Slave device write register */
   outp( mCmtSmbHstDat0, Value );             /* Zero receive buffer */
   outp( mCmtSmbHstDat1, 0);                  /* Zero receive buffer */
   outp( mCmtSmbHstSts,  inp( mCmtSmbHstSts ) );  /* Clear flags */
   outp( mCmtSmbHstCnt,  M_CMT_MSK_BYTE_DATA_RW );  /* Action write byte operation */

   /*
   ** The Smbus controller is slow so wait before checking completion
   */
   mCmtDelayNanosecs( M_CMT_MS_DELAY_SHORT );

   /*
   ** Wait for completion bit be set
   */
   while ( inp( mCmtSmbHstCnt ) & M_CMT_MSK_HOST_BUSY ) 
      mCmtDelayNanosecs( M_CMT_MS_DELAY_LONG ); 

   return inp( mCmtSmbHstDat0 );
}


/*******************************************************************************
** Function Name:
**    SmbReadByte
**
** Purpose:
**    Smbus read byte of data from a register on the hardware monitor chip.
**
** Description:
**    The Smbus controller registers are configured in the correct order.
**
** Return Type:
**    int
**        Value read from hardware monitor register
**
** Arguments:
**    int   (in)
**        The register to be read.
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtSmbReadByte( int Cmd )
{
   outp( mCmtSmbHstCmd,  Cmd);                /* The register to be read */
   outp( mCmtSmbHstAdd,  mCmtSmbSlaveRead );      /* Slave device read register */
   outp( mCmtSmbHstDat0, 0);                  /* Zero receive buffer */
   outp( mCmtSmbHstDat1, 0);                  /* Zero receive buffer */
   outp( mCmtSmbHstSts,  inp( mCmtSmbHstSts ) );  /* Clear flags */
   outp( mCmtSmbHstCnt,  M_CMT_MSK_BYTE_DATA_RW );  /* Action read byte operation */
  
   /*
   ** The Smbus controller is slow so wait before checking completion
   */
   mCmtDelayNanosecs( M_CMT_MS_DELAY_SHORT );
      
   /* Wait for completion bit be set */
   while ( inp( mCmtSmbHstCnt ) & M_CMT_MSK_HOST_BUSY )
      mCmtDelayNanosecs( M_CMT_MS_DELAY_LONG ); 

   return inp( mCmtSmbHstDat0 );
}


/*******************************************************************************
** Function Name:
**    SmbReadWord
**
** Purpose:
**    Smbus read word of data from a register on the hardware monitor chip.
**
** Description:
**    The Smbus controller registers are configured in the correct order.
**
** Return Type:
**    int
**        Value read from hardware monitor register
**
** Arguments:
**    int   (in)
**        The register to be read.
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtSmbReadWord( int Cmd )
{
   /*
   ** Set-up the Smbus controller registers in the correct order
   */
   outp( mCmtSmbHstCmd,  Cmd);                /* The register to be read */ 
   outp( mCmtSmbHstAdd,  mCmtSmbSlaveRead );      /* Slave device read register */
   outp( mCmtSmbHstDat0, 0);                  /* Zero receive buffer */
   outp( mCmtSmbHstDat1, 0);                  /* Zero receive buffer */
   outp( mCmtSmbHstSts,  inp( mCmtSmbHstSts ) );  /* Clear flags */ 
   outp( mCmtSmbHstCnt,  M_CMT_MSK_WORD_DATA_RW );  /* Action read word operation */
  
   /*
   ** The Smbus controller is slow so wait before checking completion
   */

   mCmtDelayNanosecs( M_CMT_MS_DELAY_SHORT );
      
   /*
   ** Wait for completion bit be set
   */
   while ( inp( mCmtSmbHstCnt ) & M_CMT_MSK_HOST_BUSY )
      mCmtDelayNanosecs( M_CMT_MS_DELAY_LONG ); 


   return( ( inp( mCmtSmbHstDat0 ) + inp( mCmtSmbHstDat1 ) ) << CHAR_BIT );
}


/*******************************************************************************
** Function Name:
**    mCmtIOWriteByte
**
** Purpose:
**    I/O write a byte of data to a register on the hardware monitor chip.
**
** Description:
**    Select the register to be written then write the new value to 
**    the register. 
**
** Return Type:
**    int
**        The value writine to the register  
**
** Arguments:
**    int   (in)
**        The address of the register to be written to.
**
**    int   (in)
**        The value to be written to the register.
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtIOWriteByte( int Addr, int Value )
{
  outp( mCmtIOAddr, Addr );
  return outp( mCmtIOData, Value );
}


/*******************************************************************************
** Function Name:
**    mCmtIOReadByte
**
** Purpose:
**    I/O read a byte of data from a register on the hardware monitor chip.
**
** Description:
**    Read a single byte of data from a particular register on the
**    hardware monitor chip.
**
** Return Type:
**    int
**        The data byte read from the register
**         
** Arguments:
**    int
**        The address of the register to be retrieved.
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtIOReadByte( int Addr )
{
  outp( mCmtIOAddr, Addr );
  return inp( mCmtIOData ); 
}


/*******************************************************************************
** Function Name:
**    mCmtIOReadWord
**
** Purpose:
**    I/O read a word of data from a register on the hardware monitor chip.
**
** Description:
**    Reads the word by doing two successive calls of a single read and
**    incrementing the address between calls. 
**    NB This is a word read from the chip and is not equivalent to an inpw() becaus
**
** Return Type:
**    int
**        The data word read from two consecutive registers.
**
** Arguments:
**    int   (in)
**        The address of the register to be retrieved.  
**
** Author(s):
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

int mCmtIOReadWord( int Addr )
{
  return( ( mCmtIOReadByte( Addr    ) << CHAR_BIT ) + 
          ( mCmtIOReadByte( Addr + 1) &  CHAR_MAX )  ); 
}

/*******************************************************************************
** Function Name:
**    mCmtDelayNanosecs
**
** Purpose:
**    Delay a given number of nanoseconds.
**
** Description:
**    Use the nanosleep function to delay the process for the 
**    given number of nanoseconds.
**
** Return Type:
**    void
**        None.
**
** Arguments:
**    Int32_t
**        Nanoseconds to delay the process for.
**
** Author(s):
**    Setve Foale (sxf).
**
*******************************************************************************/

void mCmtDelayNanosecs( Int32_t NsDelay )
{
   struct timespec ReqDelay;
   struct timespec RetDelay;

   ReqDelay.tv_sec  = 0;
   ReqDelay.tv_nsec = NsDelay;
   nanosleep( &ReqDelay, &RetDelay );

}

/*******************************************************************************
** Function Name:
**    mCmtMAX1247ReadA2D
**
** Purpose:
**    Read a channel of MAX1247 ADC.
**
** Description:
**    See above.
**    Method as in example program TP3A2D.CPP on TP400B Utilities Disk.
**
** Return Type:
**    unsigned
**        The 12 bit data word constructed from the two bytes read.
**
** Arguments:
**    unsigned   (in)
**        The address of the register to be retrieved.  
**    unsigned char 
**        Control character. 
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

unsigned mCmtMAX1247ReadA2D( unsigned      GpIOaddr,
                             unsigned char Control)
{

   unsigned Reading1,    /* Two bytes read */
            Reading2;    /* from A2D.      */

    /*
    ** Make sure CS is high.
    */
    mCmtMAX1247SetControl( GpIOaddr, M_CMT_GPIO0_A2DCSN ); 

    /*
    ** Make sure CLK is low
    */
    mCmtMAX1247ClearControl( GpIOaddr, M_CMT_GPIO0_SCLK );

    /* 
    ** DOUT is data out of the A2D chip. Set to 1 to avoid driving it.
    */
    mCmtMAX1247SetControl( GpIOaddr, M_CMT_GPIO0_DOUT );
                           
    /*
    ** Assert chip select.
    */
    mCmtMAX1247ClearControl( GpIOaddr, M_CMT_GPIO0_A2DCSN );
    /*
    ** Start the conversion
    */
    mCmtMAX1247SendByte( GpIOaddr, Control );  

    /* 
    ** Get 1st valid result byte.
    */
    Reading1 = mCmtMAX1247GetByte( GpIOaddr ); 
    /* 
    ** Get 2nd valid result byte.
    */
    Reading2 = mCmtMAX1247GetByte( GpIOaddr );

    /* 
    ** Make sure CS is high.
    */
    mCmtMAX1247SetControl( GpIOaddr, M_CMT_GPIO0_A2DCSN );
    mCmtMAX1247SetControl( GpIOaddr, M_CMT_GPIO0_SCLK |
                                     M_CMT_GPIO0_DOUT |
                                     M_CMT_GPIO0_DIN );

    /*
    ** Construct and return the 12-bit number - 
    ** hosted in a 16-bit unsigned int.
    */
    return ( ((Reading2 >> 4) & 0x0f) | ((Reading1 << 4) & 0xff0 ));
}

/*******************************************************************************
** Function Name:
**    mCmtMAX1247SetControl
**
** Purpose:
**    Set control signals for MAX1247 ADC.
**
** Description:
**   Set the control signals specified for the MAX1247 to logic one.
**   All other bits in the control register will be unchanged. DOUT must
**   be written high to ensure that it does not drive against the A2D chip.
**
** Return Type:
**    void
**        None.
**
** Arguments:
**    unsigned      (in)
**        The address of the register to be retrieved.  
**    unsigned char (in)
**        Control character. 
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

void mCmtMAX1247SetControl( unsigned GpIOaddr,
                            unsigned char Ctl )
{
    unsigned GPIO0;
    GPIO0 = inp( GpIOaddr ); /* Read current status. */
    GPIO0 |= Ctl;            /* Set control bits. */

    outp( GpIOaddr, GPIO0 | M_CMT_GPIO0_DOUT );  /* Write new value. */

}

/*******************************************************************************
** Function Name:
**    mCmtMAX1247ClearControl
**
** Purpose:
**    Clear control signals for MAX1247 ADC.
**
** Description:
**    Clear the control signals specified for the MAX1247 to logic zero.
**    All other bits in the control register will be unchanged. DOUT must
**    be written high to ensure that it does not drive against the A2D chip.
**
**
** Return Type:
**    void
**        None.
**
** Arguments:
**    unsigned      (in)
**        The address of the register to be retrieved.  
**    unsigned char (in) 
**        Control character. 
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

void mCmtMAX1247ClearControl( unsigned      GpIOaddr,
                              unsigned char Ctl )
{
    unsigned GPIO0 = inp( GpIOaddr );           /* Read current status. */
    GPIO0 &= ~Ctl;                              /* Set control bits. */
    outp( GpIOaddr, GPIO0 | M_CMT_GPIO0_DOUT ); /* Write new value. */
}

/*******************************************************************************
** Function Name:
**    mCmtMAX1247SendByte
**
** Purpose:
**    Send a command byte to ADC.
**
** Description:
**    Send a command byte to the MAX1247. Do not read the returned data.
**
** Return Type:
**    void
**        None.
**
** Arguments:
**    unsigned      (in)
**        The address of the register to be retrieved.  
**    unsigned char (in)
**        Control character. 
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

void mCmtMAX1247SendByte( unsigned      GpIOa,
                          unsigned char Control )
{
    int i;   /* Loop counter for control bits. */

    /*
    ** Loop over 8 control bits.
    */
    for (i = 0; i < 8; i++)    
    {                                               
       if ((Control & 0x80) == 0)                          /* If bit is 0      */
         mCmtMAX1247ClearControl( GpIOa, M_CMT_GPIO0_DIN); /* Set data to 0    */
       else                                                /* else             */
        mCmtMAX1247SetControl( GpIOa, M_CMT_GPIO0_DIN  );  /* Set data to 1    */
                                                           /* End if           */
      mCmtMAX1247SetControl( GpIOa, M_CMT_GPIO0_SCLK );    /* Clock cycle to   */
      mCmtMAX1247ClearControl(GpIOa, M_CMT_GPIO0_SCLK);    /* clock in data    */
      Control = Control << 1;                              /* Move to next bit */
    }                                                

    /*
    ** Set data to 0 or next clk will be a start bit.
    */
    mCmtMAX1247ClearControl( GpIOa, M_CMT_GPIO0_DIN);   

}

/*******************************************************************************
** Function Name:
**    mCmtMAX1247GetByte
**
** Purpose:
**    Read byte from ADC.
**
** Description:
**    Read data byte returned from MAX1247. Data is clocked in MSB first.
**
** Return Type:
**    unsigned char
**        Byte read from ADC.
**
** Arguments:
**    unsigned      (in)
**        The address of the register to be retrieved.  
**
** Author(s):
**    Steve Foale (sxf)
**
*******************************************************************************/

unsigned char mCmtMAX1247GetByte( unsigned GpIOa )
{
    unsigned char Ip = 0; /* Store for read byte, contructed bit by bit. */
    int           i;      /* Loop counter. */

    for (i = 0; i < 8; i++)                             /* For 8 bits data */
    {
      Ip = Ip << 1;                                     /* Move to next bit */

      mCmtMAX1247SetControl( GpIOa, M_CMT_GPIO0_SCLK ); /* Clock cycle to  */
      mCmtMAX1247ClearControl(GpIOa, M_CMT_GPIO0_SCLK); /* clock in data   */

      if ((inp( GpIOa ) & M_CMT_GPIO0_DOUT) != 0)       /* If data is 1  */
         Ip |= 1;                                       /* store in result byte */
    }                                                   /* Endfor */

    return Ip;
}

#endif /* #ifdef __QNX__ */


