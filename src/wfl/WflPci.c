/*
** Module Name:
**    WflPci.c
**
** Purpose:
**    Provides wrappers around common PCI funxtions.
**
** Description:
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Authors:
**    Steve Foale (sxf)
**
** Version:
**    $Id: WflPci.c,v 0.6 2007/10/05 09:23:33 mjf Exp $
**
** History:
**    $Log: WflPci.c,v $
**    Revision 0.6  2007/10/05 09:23:33  mjf
**    Return E_WFL_NOOSSUPPORT for Linux until functionality is ported.
**
**    Revision 0.5  2007/03/12 14:59:53  sxf
**    Minor modifications for building without error under linux.
**
**    Revision 0.4  2005/06/20 13:48:45  mjf
**    First phase of changes to aid porting to Linux. Public header compiles
**    but library not yet ported.
**
**    Revision 0.3  2004/05/06 15:21:20  asp
**    Modified eWflPciMap to use mmap_device_memory() instead of mmap().
**
**    Revision 0.2  2004/03/02 14:10:42  man
**    Added additional low-level IO calls.
**
**    Revision 0.1  2004/02/10 11:46:03  sxf
**    Initial version.
**
*/


/*
** Compiler include files
*/

/*
** System include files
*/

#include "TtlSystem.h"
#include "Wfl.h"

/*
** Local include files
*/

#include "WflPrivate.h"

#ifndef I_WFL_OS_UNSUPPORTED

/* 
** Global variables
*/

#if defined I_WFL_OS_VMS

#elif defined I_WFL_OS_QNX4  

#elif defined I_WFL_OS_QNX6

static mWflPciHandle;

#else

#endif


/******************************************************************************
** Function Name:
**    eWflPciSetup
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOIOPRIVITY - unable to gain IO privity
**         E_WFL_NOPCIBIOS   - PCI bios not found
**         E_WFL_NOOSINFO    - unable to read OS info
**
** Purpose:
**    Performs initial checks and setup prior to using PCI functions.
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflPciSetup( void )
{

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   struct _osinfo OsInfo;              /* Operating-system info structure */

   /*
   ** Read the _osinfo structure from the processor
   */
   if ( qnx_osinfo( 0, &OsInfo ) == -1 )
   {
      /*
      ** Attempt to read operating system info has failed
      */
      return ( E_WFL_NOOSINFO );
   }
   else
   {
      eLogDebug( "Operating system information retrieved OK" );
   }

   /* 
   ** Check for a PCI BIOS
   */
   if ( !( OsInfo.sflags & _PSF_PCI_BIOS ) )
   {
      /*
      ** No PCI BIOS
      */
      return ( E_WFL_NOPCIBIOS );
   }
   else
   {
      eLogDebug( "PCI BIOS present" );
   }

#elif defined I_WFL_OS_QNX6

   mWflPciHandle = pci_attach( NULL );

   if ( pci_present( NULL, NULL, NULL ) != PCI_SUCCESS )
   {
      /*
      ** No PCI BIOS
      */
      return ( E_WFL_NOPCIBIOS );
   }
   else
   {
      eLogDebug( "PCI BIOS present" );

      /*
      ** Request IO privity
      */
      if ( ThreadCtl( _NTO_TCTL_IO, 0 ) == -1 )
      {
         return ( E_WFL_NOIOPRIVITY );
      }
   }

#else

   /* 
   ** To be implemented for Linux.
   */

   return E_WFL_NOOSSUPPORT;

#endif

   return SYS_NOMINAL;
 
} /* End of eWflPciSetup() */

/******************************************************************************
** Function Name:
**    eWflPciFindDevice
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOPCIDEVICE - PCI device specified not found
**
** Purpose:
**    Find the PCI device with a given device ID and vendor ID
**
** Description:
**
** Arguments:
**    unsigned Device 
**       The Device ID. 
**    unsigned Vendor 
**       The Vendor ID. 
**    unsigned Index 
**       The index (n) of the device or function sought. 
**    unsigned *BusPtr 
**       The Bus Number of the device or function found. 
**    unsigned *DevFuncPtr 
**       The Device or Function ID of the nth device or function found with
**       the specified Device and Vendor IDs. The device number is in bits 7
**       through 3, and the function number in bits 2 through 0. 
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflPciFindDevice( unsigned Device,
                            unsigned Vendor,
                            unsigned Index,
                            unsigned *BusPtr,
                            unsigned *DevFuncPtr )
{

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   if ( _CA_PCI_Find_Device( Device, Vendor, Index, 
                             BusPtr, 
                             DevFuncPtr ) != PCI_SUCCESS )
   {
      return E_WFL_NOPCIDEVICE;
   }

#elif defined I_WFL_OS_QNX6

   if ( pci_find_device( Device, Vendor, Index,
                         BusPtr,
                         DevFuncPtr ) != PCI_SUCCESS )
   {
      return E_WFL_NOPCIDEVICE;
   }

#else

   /* 
   ** To be implemented for Linux.
   */
   return E_WFL_NOOSSUPPORT;

#endif


   return SYS_NOMINAL;

} /* End of eWflPciFindDevice() */

/******************************************************************************
** Function Name:
**    eWflPciMap
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOPCIMMAP   - unable to map memory to PCI device
**
** Purpose:
**    Maps physical memory 
**
** Description:
**
** Arguments:
**    Uint32_t Address2Map
**        Offset into shared memory of location to start mapping.
**    void **  MappedAddress
**        The address of the mapped-in object.
**    size_t   MapSize
**        Number of bytes to map into the caller's address space.
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflPciMap( Uint32_t Address2Map,
                     void   **MappedAddress,
                     size_t   MapSize )
{
   Status_t                Result = E_WFL_NOPCIMMAP;

#ifdef E_WFL_OS_QNX4

   volatile unsigned char *MemPtr;

   int                     ShmemFileDescriptor = -1;
      
   ShmemFileDescriptor = shm_open( "Physical", O_RDWR | O_CREAT, 0777 );       /* Prepare filedescriptor */   

   if (ShmemFileDescriptor != -1)                                  /* Confirm availability */
   {
        MemPtr = ( volatile unsigned char * )( mmap( 0,             /* Map into Local space */
                            MapSize,                                /* Size to map */
                            (PROT_READ | PROT_WRITE | PROT_NOCACHE),/* Access rights */
                            MAP_SHARED,                             /* Share changes to others */
                            ShmemFileDescriptor,                    /* Physical memory space */
                            ( Address2Map & 0xfffff000 )));         /* Start page */

        if (MemPtr != MAP_FAILED)
        {
            *MappedAddress = ( void * )( MemPtr + ( Address2Map & 0x0fff ) ); /* Offset Memory Location */
            eLogDebug("Local Memory Mapped to PCI Memory Successfully at %08X", ((long)(*MappedAddress)));
            Result = SYS_NOMINAL;
        }
        else
        {
            eLogCrit(NULL,"Unable to Map Local Memory to PCI Memory (mmap failed), errno = %d",errno);
            Result = E_WFL_NOPCIMMAP;
        } 

        close( ShmemFileDescriptor );                     /* Release filedescriptor */
   }
   else
   {
      eLogCrit(NULL,"Unable to Map Local Memory to TFP Memory (shm_open failed), errno = %d",errno);
      Result = E_WFL_NOPCIMMAP;
   }   

#elif defined E_WFL_OS_QNX6

   volatile void *MemPtr;
   uint32_t       PageMaskHi;
   uint32_t       PageMaskLo;

   /*
   ** Generate the high and low address masks for this architecture
   */
   PageMaskLo = sysconf( _SC_PAGESIZE ) -1;
   PageMaskHi = UINTPTR_MAX & ~PageMaskLo;
     
   MemPtr = mmap_device_memory( NULL, 
            MapSize,                                  /* Size to map */
            PROT_READ | PROT_WRITE | PROT_NOCACHE,    /* Protection rights */
            0,                                        /* Mapping properties */
            (Address2Map & PageMaskHi) );             /* Start page */

   if (MemPtr == MAP_FAILED)
   {
      eLogCrit(NULL,"Unable to map PCI device memory (mmap_device_memory failed), errno = %d",errno);
      Result = E_WFL_NOPCIMMAP;
   }
   else
   {
      *MappedAddress = (void *)(MemPtr + (Address2Map & PageMaskLo)); /* Offset Memory Location */
      eLogDebug("OK mapping PCI device memory at %08X", ((long)(*MappedAddress)));
      
      Result = SYS_NOMINAL;
   }

#else

   /* 
   ** To be implemented for Linux.
   */
   return E_WFL_NOOSSUPPORT;

#endif
   
   return( Result );

}  /* End of eWflPciMap() */


/******************************************************************************
** Function Name:
**    eWflPciUnmap
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOPCIUNMAP -  unable to unmap memory to PCI device
**
** Purpose:
**    Unmap physical memory of PCI device from local memory
**
** Description:
**
** Arguments:
**    void *  MappedAddress
**        The address of the mapped-in object.
**    size_t   MapSize
**        Number of bytes mapped into the caller's address space.
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflPciUnmap( void *MappedAddress, size_t MapSize )
{
   int Result;

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   Result = munmap( MappedAddress, MapSize );    /* Release Local space  */

#elif defined E_WFL_OS_QNX6

   Result = munmap_device_memory( MappedAddress, MapSize ); /* Release Local space  */

#else

   /* 
   ** To be implemented for Linux.
   */
   return E_WFL_NOOSSUPPORT;

#endif
   
   if (Result == -1)
   {
      eLogCrit( E_WFL_PCINOUNMAP,
                "Unable to Unmap Local Memory from PCI Memory (errno=%d)",
                errno);

      return( E_WFL_PCINOUNMAP );
   }
   else
   {
      eLogDebug("Local Memory Successfully Unmapped from PCI Memory");
      return( SYS_NOMINAL );
   }



} /* End of eWflPciUnmap() */

/******************************************************************************
** Function Name:
**    eWflPciReadConfig32
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOPCIDEVICE - PCI device specified not found
**
** Purpose:
**    Read 32-bit values from the configuration space of a device
**
** Description:
**
** Arguments:
**    unsigned Bus 
**       The bus number. 
**    unsigned DevFunc 
**       The name of the device or function. 
**    unsigned Offset 
**       The register offset into the configuration space. This offset must be aligned to a 32-bit boundary (that is 0, 4, 8, ..., 252 bytes). 
**    unsigned Count 
**       The number of 32-bit values to read. 
**    char * BufPtr 
**       A pointer to a buffer where the requested 32-bit values are placed. 
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflPciReadConfig32( unsigned Bus,
                              unsigned DevFunc,
                              unsigned Offset,
                              unsigned Count,
                              char    *BufPtr )
{

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   if ( _CA_PCI_Read_Config_DWord( Bus, 
                                   DevFunc,
                                   Offset, 
                                   Count,
                                   BufPtr ) != PCI_SUCCESS )
   {
      return E_WFL_NOPCIREAD;
   }

#elif defined I_WFL_OS_QNX6

   if ( pci_read_config32( Bus, 
                           DevFunc,
                           Offset, 
                           Count,
                           BufPtr ) != PCI_SUCCESS )
   {
      return E_WFL_NOPCIREAD;
   }

#else

   /* 
   ** To be implemented for Linux.
   */
   return E_WFL_NOOSSUPPORT;

#endif

   return SYS_NOMINAL;

} /* End of eWflPciReadConfig32() */

/******************************************************************************
** Function Name:
**    eWflPciReadConfig8
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOPCIDEVICE - PCI device specified not found
**
** Purpose:
**    Read 8-bit values from the configuration space of a device
**
** Description:
**
** Arguments:
**    unsigned Bus 
**       The bus number. 
**    unsigned DevFunc 
**       The name of the device or function. 
**    unsigned Offset 
**       The register offset into the configuration space. This offset must be aligned to a 8-bit boundary (that is 0, 4, 8, ..., 252 bytes). 
**    unsigned Count 
**       The number of 8-bit values to read. 
**    char * BufPtr 
**       A pointer to a buffer where the requested 8-bit values are placed. 
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/

Status_t eWflPciReadConfig8( unsigned Bus,
                              unsigned DevFunc,
                              unsigned Offset,
                              unsigned Count,
                              char    *BufPtr )
{

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   if ( _CA_PCI_Read_Config_Byte ( Bus, 
                                   DevFunc,
                                   Offset, 
                                   Count,
                                   BufPtr ) != PCI_SUCCESS )
   {
      return E_WFL_NOPCIREAD;
   }

#elif defined I_WFL_OS_QNX6

   if ( pci_read_config8 ( Bus, 
                           DevFunc,
                           Offset, 
                           Count,
                           BufPtr ) != PCI_SUCCESS )
   {
      return E_WFL_NOPCIREAD;
   }

#else

   /* 
   ** To be implemented for Linux.
   */
   return E_WFL_NOOSSUPPORT;

#endif

   return SYS_NOMINAL;

} /* End of eWflPciReadConfig8() */

/******************************************************************************
** Function Name:
**    eWflPciWriteConfig8
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOPCIDEVICE - PCI device specified not found
**
** Purpose:
**    Write 8-bit values from the configuration space of a device
**
** Description:
**
** Arguments:
**    unsigned Bus 
**       The bus number. 
**    unsigned DevFunc 
**       The name of the device or function. 
**    unsigned Offset 
**       The register offset into the configuration space. This offset must be aligned to a 8-bit boundary (that is 0, 4, 8, ..., 252 bytes). 
**    unsigned Count 
**       The number of 8-bit values to read. 
**    char * BufPtr 
**       A pointer to a buffer where the requested 8-bit values are placed. 
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/

Status_t eWflPciWriteConfig8( unsigned Bus,
                              unsigned DevFunc,
                              unsigned Offset,
                              unsigned Count,
                              char    *BufPtr )
{

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   if ( _CA_PCI_Write_Config_Byte ( Bus, 
                                   DevFunc,
                                   Offset, 
                                   Count,
                                   BufPtr ) != PCI_SUCCESS )
   {
      return E_WFL_NOPCIREAD;
   }

#elif defined I_WFL_OS_QNX6

   if ( pci_write_config8 ( Bus, 
                           DevFunc,
                           Offset, 
                           Count,
                           BufPtr ) != PCI_SUCCESS )
   {
      return E_WFL_NOPCIREAD;
   }

#else

   /* 
   ** To be implemented for Linux.
   */
   return E_WFL_NOOSSUPPORT;

#endif

   return SYS_NOMINAL;

} /* End of eWflPciWriteConfig8() */

/******************************************************************************
** Function Name:
**    eWflOut8
**
** Type:
**    void (None)
**
** Purpose:
**    Write an 8-bit value to a port
**
** Description:
**
** Arguments:
**    uintptr_t Port 
**       The port you want to write the value to. 
**    uint8_t Val 
**       The value that you want to write. 
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

void eWflOut8( int Port,
               int Val )
{

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   outp( Port, Val );

#elif defined I_WFL_OS_QNX6

   out8( (uintptr_t)Port, (uint8_t)Val );

#else

   /* 
   ** To be implemented for Linux.
   */

#endif

} /* End of eWflOut8() */

/******************************************************************************
** Function Name:
**    eWflIn8
**
** Type:
**    uint8_t
**       Value read from port.
**
** Purpose:
**    Read an 8-bit value from a port
**
** Description:
**
** Arguments:
**    uintptr_t Port 
**       The port you want to write the value to. 
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

unsigned int eWflIn8( int Port )
{

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   return inp( Port );

#elif defined I_WFL_OS_QNX6

   return (unsigned int)in8( (uintptr_t)Port );

#else

   /* 
   ** To be implemented for Linux.
   */
   return E_WFL_NOOSSUPPORT;

#endif

} /* End of eWflOut8() */

/******************************************************************************
** Function Name:
**    eWflIn16
**
** Type:
**    uint8_t
**       Value read from port.
**
** Purpose:
**    Read a 16-bit value from a port
**
** Description:
**
** Arguments:
**    uintptr_t Port 
**       The port you want to write the value to. 
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

unsigned int eWflIn16( int Port )
{

#if defined I_WFL_OS_VMS

   /* 
   ** This function not supported under VMS
   */
   return E_WFL_NOOSSUPPORT;

#elif defined I_WFL_OS_QNX4  

   return inpw( Port );

#elif defined I_WFL_OS_QNX6

   return (unsigned int)in16( (uintptr_t)Port );

#else

   /* 
   ** To be implemented for Linux.
   */
   return E_WFL_NOOSSUPPORT;

#endif

} /* End of eWflOut16() */


#endif /* Operating system unsupported */

/*
** EOF
*/
