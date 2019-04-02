/*******************************************************************************
** Module Name:
**    Df1.h
**
** Purpose:
**    Public header file for Allen-Bradley DF1 protocol.
**
** Description:
**    Provides the function prototypes for the DF1 comms. library.
**
** Traceability to Software Design:
**    None - Externally supplied library.
**
** Author(s):
**    Andrzej S. Piacsik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: Df1.h,v 0.3 2001/11/08 18:41:40 mjf Exp $
**
** History:
**    $Log: Df1.h,v $
**    Revision 0.3  2001/11/08 18:41:40  mjf
**    Addition of error codes.
**
**    Revision 0.2  2001/05/08 17:00:08  ljs
**    Changed Df1Write to Df1WriteSingle and Df1WriteMulti to
**    Df1Write.
**
**    Revision 0.1  2001/01/09 10:04:04  asp
**    Initial version
**
**
**
*******************************************************************************/


#ifndef DF1_H_DEFINED
#define DF1_H_DEFINED


/*
** Status codes.
*/

enum eDf1Status_e
{
   E_DF1_GEN_ERR = STATUS_START( DF1 ),

   E_DF1_OPEN_ERROR,         /* Unable to open connection to PLC */
   E_DF1_CLOSE_ERROR,        /* Unable to close connection to PLC */
   E_DF1_STATUS_ERROR,       /* Unable to obtain status from PLC */
   E_DF1_READ_ERROR,         /* Unable to read data from PLC */
   E_DF1_READ_LEN_ERROR,     /* Error in length of data read from PLC */
   E_DF1_WRITE_ERROR,        /* Unable to write data to PLC */

   E_DF1_STATUS_EOL      
};


/*
** Function prototypes.
*/

extern Status_t eDf1Open ( char *HostAddressPtr );
extern Status_t eDf1Close( void );
extern Status_t eDf1WriteSingle( char *AddressPtr, Int32_t  Data );
extern Status_t eDf1Write( char *AddressPtr, Int32_t *DataPtr, Int32_t NumSet );
extern Status_t eDf1Read ( char *AddressPtr, Int32_t *DataPtr, Int32_t Count );
extern Status_t eDf1GetStatus( void );

#endif

