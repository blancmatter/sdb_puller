/*******************************************************************************
** Module Name:
**    CmtQnx6.c
**
** Purpose:
**    QNX 6 (Neutrino) specific functions.
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
**    $Id: CmtQnx6.c,v 0.8 2006/10/17 13:57:45 sxf Exp $
**
** History:
**    $Log: CmtQnx6.c,v $
**    Revision 0.8  2006/10/17 13:57:45  sxf
**    Determine ntp lock state, memory free and cpu uses less frequently.
**
**    Revision 0.7  2006/09/28 12:47:04  sxf
**    Updated for QNX6.3SP2.
**
**    Revision 0.6  2006/08/23 09:49:12  mjf
**    Minor improvements for Linux.
**
**    Revision 0.5  2006/03/21 11:21:56  sxf
**    Added NTP lock functionaity for QNX6, Linux and VMS.
**
**    Revision 0.4  2005/07/14 09:20:46  sxf
**    Basic monitoring functionality now implemented. Monitoring of:
**       o CPU load
**       o Disk space
**       o Used memory
**
**    Revision 0.3  2004/06/15 17:22:56  mjf
**    Don't attempt any hardware detection until proven.
**
**    Revision 0.2  2004/06/15 16:59:24  mjf
**    QNX v4 operating system calls previously compiled under QNX v6 but
**    crashed, so bypassed at the temporary expense of any monitoring.
**
**    Revision 0.1  2004/01/15 15:46:25  sxf
**    Initial version.
**
**
**
*******************************************************************************/

#include "CmtPrivate.h"
#ifdef I_CMT_OS_QNX6

#include "CmtQnx6.h"

#include "Cmt.h"
#include "Log.h"
#include "Tim.h"

/*
** OS specific include files
*/
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/sockmsg.h>
#include <netdb.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/disk.h>
#include <sys/sched.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <errno.h>
#include <sys/select.h>
#include <resolv.h>

static int disk_space(int , long *, long *);
static void mCmtDelayNanosecs( Int32_t );
static unsigned mCmtMAX1247ReadA2D( unsigned, unsigned char );
static void mCmtMAX1247SetControl( unsigned, unsigned char );
static void mCmtMAX1247ClearControl( unsigned, unsigned char );
static void mCmtMAX1247SendByte( unsigned ,unsigned char );
static unsigned char mCmtMAX1247GetByte( unsigned );
static int mCmtRpmAsustech( int , int );
static int mCmtRpmGl520sm( int, int );
static int mCmtRpmWinbond( int , int );
static void mCmtInitSmbus( int );

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
#define M_CMT_REG_BANK_SELECT  0x4E   /* Bank select for switching between in8uts */
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

#define M_CMT_REG_VOLT_CORE    0x20   /* Voltage in8ut sensors */
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

#define M_CMT_MAXPACKETSIZE       1500
#define M_CMT_NTP_PORT            123    /* for ref only (see /etc/services) */
#define M_CMT_NTPVERSION_1        0x08
#define M_CMT_NTP_INFO_QUERY      62     /* **** THIS implementation dependent **** */
#define M_CMT_NTP_INFO_REPLY      63     /* **** THIS implementation dependent **** */
#define M_CMT_NTP_STIME           500000 /* usec to wait for another response */
#define M_CMT_NTP_WTIME           10     /* Time to wait for all responses */
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
static int  mCmtPciHandle;                 /* PCI handle to attach to */
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

static char mCmtQnxPacket[ M_CMT_MAXPACKETSIZE ];
int         mCmtQnxSocket;
int         mCmtQnxTimedOut;

void mCmtQnxTimeOut( int );


/*
 * Structure definitions for NTP fixed point values
 *
 *    0			  1		      2			  3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			       Integer Part			     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			       Fraction Part			     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *    0			  1		      2			  3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |		  Integer Part	     |	   Fraction Part	     |
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
 *    0			  1		      2			  3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |LI | VN  | Mode|	  Stratum    |	    Poll     |	 Precision   |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			   Synchronizing Distance		     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			  Synchronizing Dispersion		     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			Reference Clock Identifier		     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |								     |
 *   |		       Reference Timestamp (64 bits)		     |
 *   |								     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |								     |
 *   |		       Originate Timestamp (64 bits)		     |
 *   |								     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |								     |
 *   |			Receive Timestamp (64 bits)		     |
 *   |								     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |								     |
 *   |			Transmit Timestamp (64 bits)		     |
 *   |								     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
struct ntpdata {
	u_char status;		/* status of local clock and leap info */
	u_char stratum;		/* Stratum level */
	u_char ppoll;		/* poll value */
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
	u_char type;		/* request type (stratum in ntp packets) */
	u_char count;		/* number of entries in this packet */
	u_char seq;		/* sequence number of this packet */

	u_char npkts;		/* total number of packets */
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

	long	estdisp;			/* scaled by 1000 */
	long	estdelay;			/* in milliseconds */
	long	estoffset;			/* in milliseconds */
	u_long refid;
	struct l_fixedpt reftime;
	struct info_filter {
		short index;
		short filler;
		long offset[M_CMT_NTP_PEER_SHIFT];	/* in milliseconds */
		long delay[M_CMT_NTP_PEER_SHIFT];		/* in milliseconds */
	} info_filter;
};

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

Bool_t mCmtOutputNtpInfo( struct clockinfo *InfoPtr );


/*******************************************************************************
** Function Name:
**    iCmtGetQnx6CpuUsed
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

Status_t iCmtGetQnx6CpuUsed( Int32_t *CpuUsedPtr, Int32_t *NumRet )
{
#define CPU_100_PERCENT 100000   /* Because we report in milli-percent */
#define CPU_50_PERCENT   50000   /* Because we report in milli-percent */
#define JIFFIE_PERCENT   100000.0   /* Used to convert QNX jiffies to percent */
#define PSINFO_START         1   /* First process ID */
#define PSINFO_END          -1   /* No more processes */ 

   struct _psinfo Data;          /* Process information */
   pid_t  Id;                    /* Process identity    */

   time_t     NowIdle;           /* Current CPU idle time */ 
   eTtlTime_t NowTime;           /* Current time */
   eTtlTime_t DiffTime;          /* Time difference */
   double     Interval;          /* Time since last load check */      

   static  int        CpuUsed;                    /* Cpu free time */ 
   static  time_t     OldIdle = 0;                /* Previous CPU idle time */
   static  eTtlTime_t OldTime = {0,0};            /* Previous time */
   static  int        Count = 0;

   Count ++;

   /*
   ** Only calculate CPU load every so often.
   */
   if ( ( Count % I_CMT_CPU_LOAD_PERIOD ) != 1 )
   {
      /* Return last calculated value */
      *CpuUsedPtr = CpuUsed;
      *NumRet     = 1;

      return SYS_NOMINAL;
   }

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
      CpuUsed = ( NowIdle - OldIdle ) * JIFFIE_PERCENT / Interval;
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
   if ( CpuUsed < 0 )
   {
      CpuUsed = 0;
   }
   else if ( CpuUsed > CPU_100_PERCENT )
   {
      CpuUsed = CPU_100_PERCENT;
   }
   
   /*
   ** Return the CPU used load
   */
   
   *CpuUsedPtr = CpuUsed;
   *NumRet     = 1;

   return SYS_NOMINAL;

} /* End of iCmtGetQnxCpuUsed */

/*******************************************************************************
** Function Name:
**    iCmtGetQnx6DiskFree
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

Status_t iCmtGetQnx6DiskFree( Int32_t *Disk1FreePtr,
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
                      "Failed to open disk1, \"%s\", errno %d",
                      iCmtData.DisksToMonitor[ 0 ], errno ); 
         close( FileDesc );
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
         close( FileDesc );
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
         close( FileDesc );
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
         close( FileDesc );
      }
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
         close( FileDesc );
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
         close( FileDesc );
      }
   }


   return Status;

}

/*******************************************************************************
** Function Name:
**    iCmtGetQnx6MemoryFree
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

Status_t iCmtGetQnx6MemoryFree( Int32_t *MemFreePtr,
                               Int32_t *NumRet )
{

   int       QnxStat;     /* Status returned from QNX function. */
   Status_t  Status;      /* TTL status code. */
   static struct _osinfo OsInfo; /* QNX operating system information */
   static int Count = 0;

   Count ++;

   /*
   ** Only calculate memory free every so often.
   */
   if ( ( Count % I_CMT_MEM_FREE_PERIOD ) != 1 )
   {
      /* Return last calculated value */
      *MemFreePtr = ( OsInfo.freepmem * I_CMT_MILLIPERCENT )
                          / OsInfo.totpmem;

      *NumRet     = 1;
      Status      = SYS_NOMINAL;
      return Status;
   }

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
**    iCmtGetQnx6Temperature
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

Status_t iCmtGetQnx6Temperature( Int32_t *Temp1Ptr,
                                Int32_t *Temp2Ptr,
                                Int32_t *Temp3Ptr,
                                Int32_t *NumRet )
{

   int  Tmp1       = 0;       /* Temperature readings in milli-degrees C */
   int  Tmp2       = 0;
   int  Tmp3       = 0;

   /*
   ** Until QNX v6 version fully ported, return SYS_NOMINAL and 0 valid parameters.
   */
#if 1
   *NumRet = 0;
   return SYS_NOMINAL;
#endif

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
**    iCmtGetQnx6FanSpeed
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

Status_t iCmtGetQnx6FanSpeed( Int32_t *Fan1Ptr,
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
   ** Until QNX v6 version fully ported, return SYS_NOMINAL and 0 valid parameters.
   */
#if 1
   *NumRet = 0;
   return SYS_NOMINAL;
#endif

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
**    iCmtGetQnx6NtpLocked
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
#if 1

Status_t iCmtGetQnx6NtpLocked( Bool_t *Locked )
{

   static char Command[] = "ntpdc -p | tail -n1 | cut -c1 > " I_CMT_NTP_LOCK_FILE "`hostname`";
   int         Ret;
   static char Result;
   FILE       *NtpLockFile;
   static Bool_t NtpLockFileReport = FALSE;
   static Bool_t NtpLockCmdReport  = FALSE;
   Status_t    Status = SYS_NOMINAL;
   static int  Count = 0;
   static char HostLockFile[ 256 ];
   static char HostName[ 128 ];
   static Bool_t FirstTime = TRUE;

   if ( FirstTime )
   {
      gethostname( HostName, 128 );

      sprintf( HostLockFile, "%s%s", I_CMT_NTP_LOCK_FILE, HostName );
      eLogCrit( 0, "Hostlockfile is %s", HostLockFile );
      FirstTime = FALSE;
   }

   Count ++;


   /*
   ** Only do ntpdc once per minute
   */
   if (( Count % 60 ) != 1 )
   {
      *Locked = FALSE;

      if ( Result != EOF )
      {
         if ( Result == '*' )
         {
            *Locked = TRUE;
         }
      }

      return Status;
   }

   *Locked = FALSE;

   Ret = system( Command );

   if ( Ret == 0 )
   {
      NtpLockFile = fopen( HostLockFile , "r" );

      if ( NtpLockFile != NULL )
      {
         Result = fgetc( NtpLockFile );

         if ( Result != EOF )
         {
            if ( Result == '*' )
            {
               *Locked = TRUE;
            }
         }
         
         fclose( NtpLockFile );
         NtpLockFileReport = FALSE;
         NtpLockCmdReport  = FALSE;

      }
      else
      {
         if ( NtpLockFileReport == FALSE )
         {
            Status = E_CMT_NO_NTPINFO;
            eLogErr( E_CMT_NO_NTPINFO, "Unable to write NTP lock file %s, errno = %d",
                  I_CMT_NTP_LOCK_FILE,
                  errno );
            NtpLockFileReport = TRUE;
         }
      }
   }
   else
   {
      if ( NtpLockCmdReport == FALSE )
      {
         Status = E_CMT_NO_NTPINFO;
         eLogErr( E_CMT_NO_NTPINFO, "Unable to determine ntp lock state, errno = %d", errno );
         NtpLockCmdReport = TRUE;
      }
   }

   return Status;

}

#else

Status_t iCmtGetQnx6NtpLocked( Bool_t *Locked )
{

   struct sockaddr_in       Watcher;
   register struct ntpinfo *MsgInfoPtr = (struct ntpinfo *) mCmtQnxPacket;
   register struct ntpdata *MsgDataPtr = (struct ntpdata *) mCmtQnxPacket;
   struct hostent          *HostEntPtr;
   static struct servent   *ServEntPtr = NULL;
   long                     HostAddr;
   static char              MyHost[ 128 ];
   struct clockinfo        *InfoPtr;
   struct sockaddr_in       From;
   int                      FromLen = sizeof(From);
   int                      Count, Cc;
   fd_set                   Bits;
   struct timeval           ShortTime;
   int                      First = 1;
   long                     Replies = 0;
   static Bool_t            FirstTime = TRUE;


   if ( FirstTime )
   {
      mCmtQnxSocket = 0;
      /* Get my own hostname */
      gethostname( MyHost, 128 );
      eLogDebug( "Host is %s", MyHost );
      FirstTime = FALSE;
   }

   /* Query */
   bzero((char *) &Watcher, sizeof(Watcher));
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
      bcopy(HostEntPtr->h_addr, (char *) &Watcher.sin_addr, HostEntPtr->h_length);
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

   if ( mCmtQnxSocket == 0 )
   {
      mCmtQnxSocket = socket(AF_INET, SOCK_DGRAM, 0);
   }

   if ( mCmtQnxSocket < 0 )
   {
      eLogNotice( E_CMT_NO_NTPINFO, "Unable to create socket, errno = %d", errno );
      return E_CMT_NO_NTPINFO;
   }


   if (connect( mCmtQnxSocket, (struct sockaddr *) &Watcher, sizeof(Watcher)))
   {
         eLogNotice( E_CMT_NO_NTPINFO, "Unable to connect to socket, errno = %d", errno );
         return E_CMT_NO_NTPINFO;
   }

   if (send( mCmtQnxSocket, mCmtQnxPacket, sizeof(struct ntpdata), 0) < 0)
   {
         eLogNotice( E_CMT_NO_NTPINFO, "Unable to send message to socket, errno = %d", errno );
         return E_CMT_NO_NTPINFO;
   }

   /*
    * Listen for returning packets; may be more than one packet per
    * host. 
    */
   FD_ZERO( &Bits );
   FD_SET( mCmtQnxSocket, &Bits );
   ShortTime.tv_sec = 0;
   ShortTime.tv_usec = M_CMT_NTP_STIME;
   (void) signal(SIGALRM, mCmtQnxTimeOut);
   (void) alarm(M_CMT_NTP_WTIME);
   mCmtQnxTimedOut = 0; 

   while ( 
            ( First || Replies ) && 
            ( !mCmtQnxTimedOut || select(FD_SETSIZE, &Bits, (fd_set *) 0,
                (fd_set *) 0, &ShortTime) > 0 ) 
         )
   {
      if ( ( Cc = recvfrom(mCmtQnxSocket, mCmtQnxPacket, sizeof(mCmtQnxPacket), 0,
           (struct sockaddr *)&From, &FromLen)) <= 0 )
      {
         if (Cc == 0 || errno == EINTR)
         {
            continue;
         }

         eLogCrit( E_CMT_NO_NTPINFO, "Error receiving from NTP socket" );
         (void) close( mCmtQnxSocket );
         return E_CMT_NO_NTPINFO;
      }

      FD_SET( mCmtQnxSocket, &Bits );

#if 1     

      eLogCrit( 0, "Version :    %d", MsgInfoPtr -> version );
      eLogCrit( 0, "Type    :    %d", MsgInfoPtr -> type );
      eLogCrit( 0, "Count   :    %d", MsgInfoPtr -> count );
      eLogCrit( 0, "Seq     :    %d", MsgInfoPtr -> seq );
      eLogCrit( 0, "Npkts   :    %d", MsgInfoPtr -> npkts );
      eLogCrit( 0, "Peers   :    %d", MsgInfoPtr -> peers );

#endif

#if 0
      if ( MsgInfoPtr -> type != M_CMT_NTP_INFO_REPLY )
      {
         eLogCrit( E_CMT_NO_NTPINFO, "ntp reply type not an info repl, type = %d",
                   MsgInfoPtr -> type );

         return E_CMT_NO_NTPINFO;
      }

      if ( MsgInfoPtr -> version != M_CMT_NTPDC_VERSION)
      {
         eLogCrit( E_CMT_NO_NTPINFO, "ntp version not %d, version = %d",
                   M_CMT_NTPDC_VERSION, MsgInfoPtr -> version );

         return E_CMT_NO_NTPINFO;
      }
#endif

      *Locked = FALSE;

      if ( First )
      {
         First = 0;
         Replies = (1L << MsgInfoPtr -> npkts) - 1;
         eLogDebug( "   (rem)  Address   (lcl)      Strat Poll Reach    Delay   Offset    Disp" );
         eLogDebug( "=========================================================================" );
      }

      Replies &= ~(1L << MsgInfoPtr->seq);
      InfoPtr = (struct clockinfo *)&MsgInfoPtr[1]; 
      eLogCrit( 0, "count = %d", MsgInfoPtr -> count );
      for ( Count = MsgInfoPtr -> count; Count > 0; Count--)
      {
         *Locked = mCmtOutputNtpInfo( InfoPtr );
         InfoPtr++;
      }
   }

   return SYS_NOMINAL;

}



void mCmtQnxTimeOut( int i )
{

   i = i;
   mCmtQnxTimedOut = 1;

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

#endif

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

   /*
   ** Until QNX v6 version fully ported, return FALSE.
   */
   return FALSE;

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

static int mCmtProbeIOPort( void )
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

static int mCmtProbeSmbPort( void )
{
   unsigned BusNum;     /* PCI bus number */
   unsigned DevFn;      /* PCI Device=bits 5:3  PCI Function=bits 2:0 */
   unsigned LastBus;    /* Last bus found */
   unsigned Version;    /* PCI version number */
   unsigned Hardware;   /* PCI Hardware ID */
      
   int Chip;
   int Status;

#if 0
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

#endif

   mCmtPciHandle = pci_attach( NULL );

   /*
   ** Check if a PCI BIOS/bus is present
   */
   if( pci_present( &LastBus, &Version, &Hardware) != PCI_SUCCESS )
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
      Status = pci_find_device( mCmtOKChips[Chip].DevId, 
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
         pci_read_config8( BusNum, DevFn, 
                   M_CMT_SMBBA, M_CMT_ONE_ITEM, (char*)&mCmtSmbPort);
   
         /* 
         ** Read the SMB host configuration 
         */
         pci_read_config8( BusNum, DevFn, 
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

static int mCmtRpmAsustech( int Fan, int Divisor )
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

static int mCmtRpmGl520sm( int Fan, int Divisor )
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

static int mCmtRpmWinbond( int Fan, int Divisor )
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

static void mCmtInitSmbus( int Slave )
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

static int mCmtSmbWriteByte( int Cmd, int Value)
{
   out8( mCmtSmbHstCmd,  Cmd );               /* The register to be read */
   out8( mCmtSmbHstAdd,  mCmtSmbSlaveWrite );     /* Slave device write register */
   out8( mCmtSmbHstDat0, Value );             /* Zero receive buffer */
   out8( mCmtSmbHstDat1, 0);                  /* Zero receive buffer */
   out8( mCmtSmbHstSts,  in8( mCmtSmbHstSts ) );  /* Clear flags */
   out8( mCmtSmbHstCnt,  M_CMT_MSK_BYTE_DATA_RW );  /* Action write byte operation */

   /*
   ** The Smbus controller is slow so wait before checking completion
   */
   mCmtDelayNanosecs( M_CMT_MS_DELAY_SHORT );

   /*
   ** Wait for completion bit be set
   */
   while ( in8( mCmtSmbHstCnt ) & M_CMT_MSK_HOST_BUSY ) 
      mCmtDelayNanosecs( M_CMT_MS_DELAY_LONG ); 

   return in8( mCmtSmbHstDat0 );
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

static int mCmtSmbReadByte( int Cmd )
{
   out8( mCmtSmbHstCmd,  Cmd);                /* The register to be read */
   out8( mCmtSmbHstAdd,  mCmtSmbSlaveRead );      /* Slave device read register */
   out8( mCmtSmbHstDat0, 0);                  /* Zero receive buffer */
   out8( mCmtSmbHstDat1, 0);                  /* Zero receive buffer */
   out8( mCmtSmbHstSts,  in8( mCmtSmbHstSts ) );  /* Clear flags */
   out8( mCmtSmbHstCnt,  M_CMT_MSK_BYTE_DATA_RW );  /* Action read byte operation */
  
   /*
   ** The Smbus controller is slow so wait before checking completion
   */
   mCmtDelayNanosecs( M_CMT_MS_DELAY_SHORT );
      
   /* Wait for completion bit be set */
   while ( in8( mCmtSmbHstCnt ) & M_CMT_MSK_HOST_BUSY )
      mCmtDelayNanosecs( M_CMT_MS_DELAY_LONG ); 

   return in8( mCmtSmbHstDat0 );
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

static int mCmtSmbReadWord( int Cmd )
{
   /*
   ** Set-up the Smbus controller registers in the correct order
   */
   out8( mCmtSmbHstCmd,  Cmd);                /* The register to be read */ 
   out8( mCmtSmbHstAdd,  mCmtSmbSlaveRead );      /* Slave device read register */
   out8( mCmtSmbHstDat0, 0);                  /* Zero receive buffer */
   out8( mCmtSmbHstDat1, 0);                  /* Zero receive buffer */
   out8( mCmtSmbHstSts,  in8( mCmtSmbHstSts ) );  /* Clear flags */ 
   out8( mCmtSmbHstCnt,  M_CMT_MSK_WORD_DATA_RW );  /* Action read word operation */
  
   /*
   ** The Smbus controller is slow so wait before checking completion
   */

   mCmtDelayNanosecs( M_CMT_MS_DELAY_SHORT );
      
   /*
   ** Wait for completion bit be set
   */
   while ( in8( mCmtSmbHstCnt ) & M_CMT_MSK_HOST_BUSY )
      mCmtDelayNanosecs( M_CMT_MS_DELAY_LONG ); 


   return( ( in8( mCmtSmbHstDat0 ) + in8( mCmtSmbHstDat1 ) ) << CHAR_BIT );
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

static int mCmtIOWriteByte( int Addr, int Value )
{
  out8( mCmtIOAddr, Addr );
  out8( mCmtIOData, Value );
  return Value;
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

static int mCmtIOReadByte( int Addr )
{
  out8( mCmtIOAddr, Addr );
  return in8( mCmtIOData ); 
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
**    NB This is a word read from the chip and is not equivalent to an in8w() becaus
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

static int mCmtIOReadWord( int Addr )
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

static void mCmtDelayNanosecs( Int32_t NsDelay )
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

static unsigned mCmtMAX1247ReadA2D( unsigned      GpIOaddr,
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

static void mCmtMAX1247SetControl( unsigned GpIOaddr,
                            unsigned char Ctl )
{
    unsigned GPIO0;
    GPIO0 = in8( GpIOaddr ); /* Read current status. */
    GPIO0 |= Ctl;            /* Set control bits. */

    out8( GpIOaddr, GPIO0 | M_CMT_GPIO0_DOUT );  /* Write new value. */

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

static void mCmtMAX1247ClearControl( unsigned      GpIOaddr,
                              unsigned char Ctl )
{
    unsigned GPIO0 = in8( GpIOaddr );           /* Read current status. */
    GPIO0 &= ~Ctl;                              /* Set control bits. */
    out8( GpIOaddr, GPIO0 | M_CMT_GPIO0_DOUT ); /* Write new value. */
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

static void mCmtMAX1247SendByte( unsigned      GpIOa,
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

static unsigned char mCmtMAX1247GetByte( unsigned GpIOa )
{
    unsigned char Ip = 0; /* Store for read byte, contructed bit by bit. */
    int           i;      /* Loop counter. */

    for (i = 0; i < 8; i++)                             /* For 8 bits data */
    {
      Ip = Ip << 1;                                     /* Move to next bit */

      mCmtMAX1247SetControl( GpIOa, M_CMT_GPIO0_SCLK ); /* Clock cycle to  */
      mCmtMAX1247ClearControl(GpIOa, M_CMT_GPIO0_SCLK); /* clock in data   */

      if ((in8( GpIOa ) & M_CMT_GPIO0_DOUT) != 0)       /* If data is 1  */
         Ip |= 1;                                       /* store in result byte */
    }                                                   /* Endfor */

    return Ip;
}

/*
 * qnx_osinfo.c - QNX 4 to QNX Neutrino migration functions
 */
 
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <devctl.h>
#include <errno.h>
#include <sys/syspage.h>
#include <sys/procfs.h>
#include <sys/resource.h>
#include <hw/pci.h>
/*
#include <mig4nto.h>
*/
#define UNSUPPORTED -1
#define BUFF_SIZE       1024   /* confstr buffer size.                     */
#define NSEC_TO_USEC    1000    /* Convert nsecs to usecs.                  */
#define VERSION_MULT    100     /* Multiply major number by this value.     */

static int get_total_mem(void);
static int get_free_mem(void);
static short unsigned get_tick_size(void);

/*
 *  The fields in the osdata structure are set to the following values:
 *                      
 *  info->tick_size     The current ticksize or resolution of the 
 *                      realtime clock in usecs.
 *  info->version       QNX 6.2.0 reports a version of 602, where QNX 4.25 
 *                      reported 425.
 *  info->sflags        Bitfield containing:        
 *                      _PSF_PROTECTED      Running in protected mode.
 *                      _PSF_NDP_INSTALLED  An fpu is installed.
 *                      _PSF_EMULATOR_INSTALLED An fpu emulator is installed
 *                      _PSF_PCI_BIOS       A PCI BIOS is present
 *                      _PSF_32BIT_KERNEL   32-bit kernel is being used.
 *
 *                      These flags are not supported:
 *                      _PSF_EMU16_INSTALLED    16-bit 80x87 emulator running
 *                      _PSF_EMU32_INSTALLED    32-bit 80x87 emulator running
 *                      _PSF_32BIT          Proc32 is running.
 *                      _PSF_APM_INSTALLED  Advanced Power Management
 *                      _PSF_RESERVE_DOS    
 *                      _PSF_RESERVE_DOS32  Lower 640K reserved for DOS.
 *  info->nodename      QNX 4 nid retrieved from the mig4nto Name Resource
 *                      Manager.
 *  info->cpu           Processor type 486,586,...
 *  info->machine       Name of this machine on the network.
 *  info->totpmem       Total physical memory.
 *  info->freepmem      Free physical memory.
 *  info->totmemk       Total memory in Kb, up to USHORT_MAX (65535).
 *  info->freememk      Free memory in Kb, up to USHORT_MAX (65535).
 *  info->cpu_features  Contains CPU Speed in Mhz.
 *
 *  The remaining fields are set to UNSUPPORTED.
 */
int
qnx_osinfo(nid_t nid, struct _osinfo *info)
{
   uint32_t    fpu;
   int         totalk;
   int         freek;
   unsigned    lastbus;
   unsigned    version;
   unsigned    hardware;
   char        buf[BUFF_SIZE];

   if (nid != 0) {
      errno = ENOSYS;
      return -1;
   }
      
   memset(info, 0, sizeof(struct _osinfo));
   info->cpu_speed = UNSUPPORTED; /* Different in QNX 4 and QNX Neutrino */
   info->num_procs = UNSUPPORTED; /* creates until it's out of memory */
   info->tick_size = get_tick_size();

   if (confstr(_CS_RELEASE, buf, sizeof(buf)) != 0) {
      char *c;

      info->version = atoi(buf) * VERSION_MULT;
      if ((c = strchr(buf, '.')) && *(c++))
         info->version += atoi(c);
   }

   info->timesel = UNSUPPORTED;   /* Time Segment */
   totalk = get_total_mem() / 1024;
   if (totalk > USHRT_MAX)
      info->totmemk = USHRT_MAX;
   else
      info->totmemk = totalk;

   freek = get_free_mem() / 1024;
   if ( USHRT_MAX < freek )    /* free mem Kb will fit */
      info->freememk = USHRT_MAX;
   else
      info->freememk = freek;

   info->primary_monitor   = UNSUPPORTED;
   info->secondary_monitor = UNSUPPORTED;
   info->machinesel        = UNSUPPORTED;
   info->disksel           = UNSUPPORTED;
   info->diskoff           = UNSUPPORTED;
   info->ssinfo_offset     = UNSUPPORTED;
   info->ssinfo_sel        = UNSUPPORTED;
   info->microkernel_size  = UNSUPPORTED; 
   info->release           = UNSUPPORTED;

   info->sflags = _PSF_PROTECTED;   /* Running in protected mode    */
   /* Check for FPU support. */
   fpu = (CPU_FLAG_FPU & SYSPAGE_ENTRY(cpuinfo)->flags) ? 1 : 0;
   if (fpu)
      info->sflags |= _PSF_NDP_INSTALLED;      /* An fpu is installed  */
   else /* An fpu emulator is running */
      info->sflags |= _PSF_EMULATOR_INSTALLED;

   if (PCI_SUCCESS == pci_present(&lastbus, &version, &hardware))
      info->sflags |= _PSF_PCI_BIOS;         /* A PCI BIOS is present  */

   if (INT_MAX == LONG_MAX)               /* 32-bit detection     */
      info->sflags |= _PSF_32BIT_KERNEL;      /* 32-bit kernel  */

   info->nodename = 0;
   info->cpu = SYSPAGE_ENTRY(cpuinfo)->cpu;   /* Proc type 486,586,... */

   if (confstr(_CS_HOSTNAME, buf, sizeof(buf)) != 0) {
      memcpy(info->machine, buf, sizeof(info->machine));
      info->machine[sizeof(info->machine) - 1] = '\0';
   }
   info->fpu          = UNSUPPORTED;
   info->bootsrc       = UNSUPPORTED;   /* 'F'loppy 'H'ard disk 'N'etwork */
   info->num_names     = UNSUPPORTED;   /* Max names      */
   info->num_timers    = UNSUPPORTED;   /* Maximum number of timers     */
   info->num_sessions   = UNSUPPORTED;   /* Maximum number of sessions   */
   info->num_handlers   = UNSUPPORTED;   /* Maximum interrupt handlers   */
   info->reserve64k   = UNSUPPORTED;   /* Relocation offset          */
   info->num_semaphores    = UNSUPPORTED;
   info->prefix_len        = UNSUPPORTED;
   info->max_nodes         = UNSUPPORTED;   /* Num licensed nodes   */
   info->proc_freemem      = UNSUPPORTED;
   info->cpu_loadmask      = UNSUPPORTED;
   info->fd_freemem        = UNSUPPORTED;
   info->ldt_freemem       = UNSUPPORTED;

   info->num_fds[0]        = UNSUPPORTED; /* Min fd's             */
   info->num_fds[1]        = UNSUPPORTED; /* Max fd's             */
   info->num_fds[2]        = UNSUPPORTED; /* Total fd's           */

   info->pidmask           = UNSUPPORTED; /* Process ID bit mask 
                                      (to determine cyclic and 
                                     indexing portions of pid) */
   info->name_freemem      = UNSUPPORTED;
   info->top_of_mem        = UNSUPPORTED;
   info->freevmem          = UNSUPPORTED;
   info->freepmem          = get_free_mem();
   info->totpmem           = get_total_mem();
   info->totvmem           = UNSUPPORTED;
   info->cpu_features      = SYSPAGE_ENTRY(cpuinfo)->speed;

   return 0;
}

static int
get_total_mem(void)
{
   char               *str = SYSPAGE_ENTRY(strings)->data;
   struct asinfo_entry      *as = SYSPAGE_ENTRY(asinfo);
   int                     total = 0;
   unsigned            num;

   for(num = _syspage_ptr->asinfo.entry_size / sizeof(*as); num > 0; --num) {
      if(strcmp(&str[as->name], "ram") == 0) {
         total += as->end - as->start + 1;
      }
      ++as;
   }
   return total;
}

static int
get_free_mem(void)
{
   struct stat st;
   int    rval = 0;

   if (stat( "/proc", &st) == 0)
      rval = st.st_size;
   
   return rval;
}

static short unsigned
get_tick_size(void)
{
   struct timespec res;

   if (clock_getres(CLOCK_REALTIME, &res) == -1)
      return -1;
   return res.tv_nsec / NSEC_TO_USEC;
}

/*
 * qnx_psinfo.c - QNX 4 to QNX Neutrino migration functions
 */
 
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <devctl.h>
#include <errno.h>
#include <sys/dcmd_chr.h>
#include <termios.h>
#include <libgen.h>
#include <sys/procfs.h>
#include <time.h>
#include <sched.h>
/*
#include <mig4nto.h>
*/
#define PROC_BASE "/proc"       /* Location of the proc filesystem */

static pid_t next_proc_num(pid_t pnum);

/*
 * struct _seginfo was in sys/seginfo.h
 * note that the qnx_psinfo() function ignores the parameter that uses
 * the struct _seginfo type
 */

struct _seginfo {
        short unsigned  selector,
                                        flags;
        long            addr,
                                        nbytes;
};

struct dinfo_s {
   procfs_debuginfo    info;
   char                pathbuffer[PATH_MAX];
};

/* 
 *  Some of the information that was associated with the process
 *  in QNX 4 is associated with the thread in QNX Neutrino
 *  (e.g. state, blocked_on, ...).  The assumption here is that
 *  if you are migrating a QNX 4 process to QNX Neutrino, you
 *  will only have one thread, so this information is taken from
 *  the thread with thread id 1 (the main() thread).
 *
 *  The following fields are populated in the migration version of
 *   qnx_psinfo():
 *
 *      psdata->pid             The process id
 *      psdata->blocked_on      What process is blocked on (pid).  
 *      psdata->pid_group       Process group
 *      psdata->ruid            Real user ID
 *      psdata->rgid            Real group ID
 *      psdata->euid            Effective User ID
 *      psdata->egid            Effective Group ID
 *      psdata->umask           Process umask
 *      psdata->sid             Session ID
 *      psdata->signal_ignore   Process signal ignore mask
 *      psdata->signal_mask     Thread signal block mask
 *      psdata->state           Thread state
 *      psdata->priority        Process priority
 *      psdata->max_priority    Max Priority
 *      psdata->sched_algorithm 
 *
 *      psdata->un.proc.name    The name of the program image.
 *                              This name is limited to 100 bytes 
 *                              including the NULL.
 *      psdata->un.proc.father  Parent Process
 *      psdata->un.proc.son     Child Process
 *      psdata->un.proc.brother Sibling process
 *      psdata->un.proc.times   All times set to zero
 *  
 *  All other psdata structure elements are set to UNSUPPORTED.
 *
 *  Notes:
 *      Neutrino 2.0 doesn't support time-accounting information, 
 *      so the members of the tms structures are always set to 0.
 *
 *      qnx_psinfo can currently examine processes only, as there are
 *      no /proc entries for virtual circuits and pulses.
 *
 *      The segdata parameter is ignored.
 *
 *      Currently proc_pid can only be 0 or 1.
 */
pid_t
qnx_psinfo(pid_t proc_pid, pid_t pid, struct _psinfo *psdata,
         unsigned segindex, struct _seginfo *segdata)
{
   struct dinfo_s   dinfo;
   procfs_status   dthread;         /* Thread info struct  */
   procfs_info      dprocess;         /* Process info struct */
   pid_t         my_pid = pid;
   int           count;                  
   int           proc_fd;
   int           prio;
   int           max_prio = -1;
   char          *ptr;
   char          buf[_POSIX_PATH_MAX+1];

   if (proc_pid != 0 && proc_pid != 1) {
      errno = ENOSYS;
      return -1;
   }
   if (pid == 0)
      my_pid = getpid();
   
   memset(psdata, 0, sizeof(struct _psinfo));

   sprintf(buf, "%s/%d", PROC_BASE, my_pid);
   if ((proc_fd = open(buf, O_RDONLY)) == -1) {
      /* search for next available pid.   */

      my_pid = next_proc_num(my_pid);

      sprintf(buf, "%s/%d/as", PROC_BASE, my_pid);
      if ((proc_fd = open(buf, O_RDONLY)) == -1) {
         errno = EINVAL;   /* No process with a process id >= pid exists */
         return -1;
      }
   }

   if (devctl(proc_fd, DCMD_PROC_INFO, &dprocess, sizeof(dprocess), NULL) == -1) {
      close(proc_fd);
      return -1;
   }
   
   if (devctl(proc_fd, DCMD_PROC_MAPDEBUG_BASE, &dinfo, sizeof(dinfo), NULL) == -1) {
      close(proc_fd);
      return -1;
   }

   dthread.tid = 1;
   if (devctl(proc_fd, DCMD_PROC_TIDSTATUS, &dthread, sizeof(dthread), NULL) == -1) {
      close(proc_fd);
      return -1;
   }

   psdata->pid = dprocess.pid;
   switch (dthread.state) {
   case STATE_SEND:
   case STATE_REPLY:
      psdata->blocked_on = dthread.blocked.connect.pid;
      break;
   case STATE_RECEIVE:
      psdata->blocked_on = dthread.blocked.channel.chid;
      break;
   case STATE_SEM:
      psdata->blocked_on = dthread.blocked.sync.id;
      break;
   default:
      psdata->blocked_on = 0;
      break;
   }
   psdata->pid_group   = dprocess.pgrp;
   psdata->ruid        = dprocess.uid;   /* map ruid to uid */
   psdata->rgid        = dprocess.gid;   /* getpgid(my_pid); */
   psdata->euid        = dprocess.euid;
   psdata->egid        = dprocess.egid; 
   psdata->flags       = UNSUPPORTED;

   /* Stack Pointer won't fit 64 -> 32     */
   psdata->sp_reg      = UNSUPPORTED; 
   /* May or may not have segments.        */
   psdata->ss_reg      = UNSUPPORTED;
   psdata->magic_off   = UNSUPPORTED; 
   psdata->magic_sel   = UNSUPPORTED;
   psdata->ldt         = UNSUPPORTED;
   psdata->umask       = dprocess.umask;
   psdata->signal_ignore   = dprocess.sig_ignore.bits[0]; 
   /* signals can be pending either on the process or on the thread
      so no way to have a QNX 4 ounterparts */
   psdata->signal_pending  = UNSUPPORTED; 

   psdata->signal_mask     = dthread.sig_blocked.bits[0];
   psdata->signal_off      = UNSUPPORTED; /* Signal offset.   */
   psdata->signal_sel      = UNSUPPORTED; /* Signal selector. */
   psdata->sid             = dprocess.sid;
   psdata->sid_nid         = UNSUPPORTED;
   psdata->state           = dthread.state; 
   psdata->priority        = dthread.priority;
   psdata->sched_algorithm = dthread.policy;
   for (count = SCHED_FIFO; count <= SCHED_OTHER; count++) {
      prio = sched_get_priority_max(count);
      if (max_prio < prio)
         max_prio = prio;
   }
   psdata->max_priority    = max_prio;     /* Max Priority     */

   ptr = basename(dinfo.info.path);
   memcpy(psdata->un.proc.name, ptr, sizeof(psdata->un.proc.name) );
   psdata->un.proc.name[sizeof(psdata->un.proc.name) - 1] = '\0';
   psdata->un.proc.father     = dprocess.parent;
   psdata->un.proc.son        = dprocess.child;
   psdata->un.proc.brother    = dprocess.sibling;
   psdata->un.proc.debugger   = UNSUPPORTED; /* Debugger Process */
   /* access rights from qnx_segment_arm() */
   psdata->un.proc.mpass_pid    = UNSUPPORTED;
   psdata->un.proc.mpass_pid_zero = UNSUPPORTED;
   psdata->un.proc.mpass_sel    = UNSUPPORTED;
   psdata->un.proc.mpass_flags = UNSUPPORTED;
   psdata->un.proc.links      = UNSUPPORTED; /* Num Links */
   psdata->un.proc.file_time    = UNSUPPORTED;
   /* num selectors in this process    */
   psdata->un.proc.nselectors    = UNSUPPORTED; 
   /* at what time the program started */
   psdata->un.proc.start_time    = UNSUPPORTED;
   psdata->un.proc.mxcount     = UNSUPPORTED;
   times(&psdata->un.proc.times);
   
   close(proc_fd);

   return dprocess.pid;
}

static pid_t
next_proc_num(pid_t pnum)
{
   DIR            *dirp;
   struct dirent   *dptr;
   pid_t         dnum = -1;
   int            rval = -1;

   if (pnum <= 0)
         return -1;
   if ((dirp = opendir(PROC_BASE)) != NULL) {
      do {
         if ((dptr = readdir(dirp)) != NULL) {
         
            dnum = atoi(dptr->d_name);
            /* looking for closest one that is greater than pnum */
            if (dnum > pnum && (rval == -1 || (dnum - pnum < rval - pnum)))
               rval = dnum;
         }
      } while (dptr);

      if (closedir(dirp) != 0)
         eLogErr( 0, "Unable to close directory %s, errno = %d", PROC_BASE, errno );

   }

   return rval;
}


/*
 * disk_space.c - QNX 4 to QNX Neutrino migration functions
 */
 
#include <sys/statvfs.h>
/*
#include <mig4nto.h>
*/

static int disk_space(int fd, long *freeblks, long *totalblks)
{
   struct statvfs   stfs;
   long         scale;

   if (fstatvfs(fd, &stfs) == -1)
      return(-1);
   scale = stfs.f_bsize / 512;
   *freeblks = stfs.f_bfree * scale;
   *totalblks = stfs.f_blocks * scale;
   return(0);
}

#endif /* #ifdef I_CMT_OS_QNX6 */


