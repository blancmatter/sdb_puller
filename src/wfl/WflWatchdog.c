/*
** Module Name:
**    WflWatchdog.c
**
** Purpose:
**    Interface to hardware watchdog fitted to host computer.
**
** Description:
**    A hardware watchdog is fitted in the host computer to reboot in the event 
**    of a software lock-up. This source file contains functions to interface to
**    it. Functions exist to initialise the watchdog, and reset the timer as
**    required.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**    sxf : Steve Foale (TTL)
**
** Version:
**    $Id: WflWatchdog.c,v 0.4 2007/10/05 09:23:33 mjf Exp $
**
** History:
**    $Log: WflWatchdog.c,v $
**    Revision 0.4  2007/10/05 09:23:33  mjf
**    Return E_WFL_NOOSSUPPORT for Linux until functionality is ported.
**
**    Revision 0.3  2007/03/12 14:59:53  sxf
**    Minor modifications for building without error under linux.
**
**    Revision 0.2  2004/06/03 10:25:26  sxf
**    Corrected WflWatchdog... functions.
**
**    Revision 0.1  2004/02/10 11:46:03  sxf
**    Initial version.
**
**
**
*/


/*
** Compiler include files
*/

#include <stdio.h>

/*
** System include files
*/

#include "TtlSystem.h"
#include "Log.h"
#include "Wfl.h"

/*
** Local include files
*/

#include "WflPrivate.h"

#if !defined I_WFL_OS_VMS

/*
** Variables
*/

/*
** Details of current watchdog 
*/
static iWflWatchdog_t mWflWatchdog;

static eWflWatchdogPci_t mWflWatchdogPci[] = 
{
   /*  VendorId DeviceId Base Init Off,Val  Reset Off,Val  Close Off,Val,Count */
   {   0x10B5,  0xBB02,  2,   0x00, 0x20,   0x00, 0x29,    0x00, 0x20, 1       },
   {   0x11E3,  0x5030,  0,   0x00, 0xFF,   0x00, 0xFF,    0x03, 0xA5, 2       },
   {   0x0000,  0x0000,  0,   0x00, 0x00,   0x00, 0x00,    0x00, 0x00, 0       }
};

/*
** Function Name:
**    eWflWatchdogInit
**
** Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to initialise the hardware watchdog.
**
** Description:
**    Initialises communication with the PCI hardware watchdog card. Initially
**    it is verified that a PCI BIOS is present, and then the device is located
**    using the device ID and vendor ID. The device register address is found 
**    and stored for future use, and then an initial status is set. Note that 
**    the hardware watchdog is not turned on at this point - each time the 
**    watchdog is periodically reset it is implicitly switched on.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**    sxf : Steve Foale (TTL)
**
*/

Status_t eWflWatchdogInit( void )
{
   Uint32_t PciIndex;                  /* number of PCI device type to find */
   Uint32_t DeviceRegAddr;             /* obtained device register address */

   unsigned Register;
   unsigned Value;
   unsigned Index;
   unsigned Base;

   /* initially assume watchdog doesn't exist */
   mWflWatchdog.Exists = FALSE;

#if defined I_WFL_OS_VMS || defined I_WFL_OS_LINUX

   return E_WFL_NOOSSUPPORT;

#else

   /* look for the first device of this type on the PCI bus */
   PciIndex = 0;

   /* log progress */
   eLogReport( E_LOG_INFO, 0, "Initialising hardware watchdog" );

   if ( eWflPciSetup() != SYS_NOMINAL )
   {

      eLogReport( E_LOG_NOTICE, E_WFL_ERR_WATCHDOG, 
                  "Unable to confirm a PCI BIOS" );
      return ( E_WFL_ERR_WATCHDOG );
   }

   mWflWatchdog.Index = 0;
   mWflWatchdog.Exists        = FALSE;
   
   do
   {   
      /* search for the watchdog card on the PCI bus */
      if ( eWflPciFindDevice( mWflWatchdogPci[ mWflWatchdog.Index ].DeviceId,
                              mWflWatchdogPci[ mWflWatchdog.Index ].VendorId,
                              PciIndex,
                             &mWflWatchdog.BusNum,
                             &mWflWatchdog.DevFunctionNum ) == SYS_NOMINAL )
      {
         mWflWatchdog.Exists = TRUE;
      }
      else
      {
         mWflWatchdog.Index ++;
      }
            
   } while ( ( mWflWatchdog.Exists == FALSE ) && 
             ( mWflWatchdogPci[ mWflWatchdog.Index ].DeviceId != 0x0 ) );
    
   if ( mWflWatchdog.Exists == FALSE )
   {
      /* unable to locate PCI device */
      eLogReport( E_LOG_NOTICE, E_WFL_ERR_WATCHDOG, 
                  "Unable to locate PCI hardware watchdog" );
      return ( E_WFL_ERR_WATCHDOG );
   }
   else
   {
      eLogDebug( "PCI hardware watchdog found" );
      eLogDebug( "Vendor ID = 0X%X, Device 0X%X",
                  mWflWatchdogPci[ mWflWatchdog.Index ].VendorId,
                  mWflWatchdogPci[ mWflWatchdog.Index ].DeviceId );
                                 
   }

   /* attempt to determine address of device register */
   Base = mWflWatchdogPci[ mWflWatchdog.Index ].BaseAddrRegsIndex;

   if ( eWflPciReadConfig32( mWflWatchdog.BusNum,
                             mWflWatchdog.DevFunctionNum,
                             offsetof( struct _pci_config_regs,
                                       Base_Address_Regs[ Base ] ),
                             1,
                   (char *) &DeviceRegAddr ) != SYS_NOMINAL )
   {
      /* unable to determine the device register address */
      mWflWatchdog.Exists = FALSE;
      
      eLogReport( E_LOG_NOTICE, E_WFL_ERR_WATCHDOG, 
                  "Unable to obtain address of device register" );
      return ( E_WFL_ERR_WATCHDOG );
   }
   else
   {
      /* need to perform bitwise AND for device register address */
      mWflWatchdog.DeviceReg = ( DeviceRegAddr & M_WFL_WD_REG_MASK );
      eLogReport( E_LOG_DEBUG, 0, "PCI hardware watchdog device address 0x%x",
                  mWflWatchdog.DeviceReg );
   }

   /* if got this far, now confident that watchdog exists */
   mWflWatchdog.Exists = TRUE;

   /* read the device register */
   eLogReport( E_LOG_DEBUG, 0, "Watchdog register 0x%x", 
               mWflWatchdog.DeviceReg );

   Index    = mWflWatchdog.Index;
   Register = mWflWatchdog.DeviceReg + mWflWatchdogPci[ Index ].InitOffset;
   Value    = mWflWatchdogPci[ Index ].InitValue;
      
   eWflOut8( Register, Value );

   /* read the device register */
   eLogReport( E_LOG_DEBUG, 0, "Watchdog init with 0x%x, read back 0x%x",
               mWflWatchdogPci[ Index ].InitValue, 
               eWflIn8( Register ) );

   /* read the device register */
   eLogReport( E_LOG_DEBUG, 0, "Watchdog start with 0x%x, read back 0x%x",
               mWflWatchdogPci[ Index ].InitValue, 
               eWflIn8( Register ) );

   return SYS_NOMINAL;

#endif

}


/*
** Function Name:
**    eWflWatchdogReset
**
** Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to reset the hardware watchdog.
**
** Description:
**    Resets the timeout period for the hardware watchdog, and implicitly turns
**    the watchdog timer on. This function must, of course, be called at least
**    once every timeout period.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**    sxf : Steve Foale (TTL)
**
*/

Status_t eWflWatchdogReset   ( void )
{

   unsigned Register;
   unsigned Value;
   unsigned Index;
   
   /* write the device register with the reset value */
   if ( mWflWatchdog.Exists == TRUE )
   {
      Index    = mWflWatchdog.Index;
      Register = mWflWatchdog.DeviceReg + mWflWatchdogPci[ Index ].ResetOffset;
      Value    = mWflWatchdogPci[ Index ].ResetValue;
      
      eWflOut8( Register, Value );
   }

   return SYS_NOMINAL;

} /* End of eWflWatchdogReset() */


/*
** Function Name:
**    eWflWatchdogClose 
**
** Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to end use of the hardware watchdog.
**
** Description:
**    This function turns off the watchdog timer. This function must be called 
**    on process shutdown, unless the computer is to be rebooted once the 
**    process has terminated.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**    sxf : Steve Foale (TTL)
**
*/

Status_t eWflWatchdogClose   ( void )
{

   unsigned Register;
   unsigned Value;
   unsigned Index;
   unsigned Count;
      
   /* if a watchdog card is installed */
   if ( mWflWatchdog.Exists == TRUE )
   {
      Index    = mWflWatchdog.Index;
      Register = mWflWatchdog.DeviceReg + mWflWatchdogPci[ Index ].CloseOffset;
      Value    = mWflWatchdogPci[ Index ].CloseValue;
      Count    = mWflWatchdogPci[ Index ].CloseCount;
      
      /* read the device register */
      eLogReport( E_LOG_DEBUG, 0, "Watchdog register 0x%x", 
                  eWflIn8( Register ) );

      for ( ; Count > 0; Count-- )
      {
         /* write the device register with the reset value */
         eWflOut8( Register, Value );
      }

      /* read the device register */
      eLogReport( E_LOG_DEBUG, 0, "Watchdog closed with 0x%x, read back 0x%x",
                  mWflWatchdogPci[ Index ].CloseValue, 
                  eWflIn8( Register ) );
   }

   return SYS_NOMINAL;

} /* End of eWflWatchdogClose() */

#endif 

/*
** EOF
*/
