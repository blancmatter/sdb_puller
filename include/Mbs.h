/*****************************************************************************
** File Name :
**   Mbs.h
**
** Purpose :
**   Modbus server public header file.
**
** Description :
**   Used to declare exported data, functions and definitions for the
**   ModBus Server package.
**   The ModBus Server supports multiple instances each with a seperate
**   ModBus networks via a command line argument. Each supported variant
**   requires:
**      - A process name defined as a string of form E_TTL_MBS_*** and
**        assigned to a symbol of form E_MBS_REG_SERVER_***
**      - A number of unique OIDs for network specific data submitted
**        to the Status DataBase. These should have the form D_MBS_******
**
** Traceability to Software Design:
**    System architect project -
**    \\Files\ops\Software_Group\System Arch\TScope_Project\AMN\Mbs
**
** Author(s) :
**      Mark Bowman (mkb)
**
** Copyright (C) Telesope Technologies Limited (TTL), 2001
**
** Version :
**      $Id: Mbs.h,v 0.9 2003/03/04 09:30:02 mkb Exp $
**
**  History :
**      $Log: Mbs.h,v $
**      Revision 0.9  2003/03/04 09:30:02  mkb
**      Add status code for OID error.
**
**      Revision 0.8  2002/10/09 15:50:57  mkb
**      Create definitions for minimum and maximum temperatures returned
**      by sensors and the sensor error value.
**
**      Revision 0.7  2002/08/22 15:19:05  mkb
**      Added hardware simulate mode for use during commissioning.
**      The simulate mode varies all enabled sensors between the
**      configured extremes in using a linear function. The hardware
**      simulate mode can be triggered either by a command line
**      switch or though a dedicated OID.
**
**      Revision 0.6  2002/08/22 08:34:30  mkb
**      Added OIDs to enable calculation of mean temperatures.
**
**      Revision 0.5  2002/08/21 13:10:45  mkb
**      Added new error code used by process configuration routines.
**
**      Revision 0.4  2001/12/13 16:14:06  mkb
**      Add OIDs for primary mirror, secondary mirror and A&G box.
**      Add error codes for temperature averaging.
**
**      Revision 0.3  2001/10/25 11:21:27  mkb
**      Add mean temperature Oid.
**
**      Revision 0.2  2001/10/24 08:49:51  mkb
**      Minor modifications after testing.
**
**      Revision 0.1  2001/10/18 15:07:56  mkb
**      First revision.
**
**
******************************************************************************/


#ifndef _MBS_H_DEFINED
#define _MBS_H_DEFINED


/*
** Required include files.
*/

#include "TtlSystem.h"       /* TTL standard system header file.             */


/*
** Allow multiple instances of ModBus server.
** The default will be the telescope structure temperature monitoring
** network.
*/

#define E_MBS_REG_SERVER_TTM      "E_TTL_MBS_TTM"
#define E_MBS_REG_SERVER_NAME     E_MBS_REG_SERVER_TTM
#define E_MBS_SERVER_PROC         "ModBusServer"
#define E_MBS_SERVER_PATH         "//1/opt/ttl/bin/Mbs"
#define E_MBS_SERVER_PRI          (10)


enum eMbsStatus
{
    E_MBS_GEN_ERROR = STATUS_START(MBS),  /* General error.                   */
    E_MBS_INIT_FAIL,          /* Initialisation error.                        */
    E_MBS_SHUTDOWN_ERROR,     /* Failed to shutdown.                          */
    E_MBS_OPEN_FAIL,          /* Failed to open a ModBus connection.          */
    E_MBS_CLOSE_FAIL,         /* Failed to close a ModBus connection.         */
    E_MBS_READ_FAIL,          /* Failed to read data from a slave device.     */
    E_MBS_SEND_FAIL,          /* Failed to send a request to a slave device.  */
    E_MBS_INVALID_DATA_LEN,   /* An invalid data length was specified.        */
    E_MBS_INVALID_COMMAND,    /* An invlaid command was received.             */
    E_MBS_TIMER_SETUP_ERR,    /* Failed to setup a timer.                     */
    E_MBS_TIMER_CLOSE_ERR,    /* Failed to close a timer.                     */
    E_MBS_INVALID_DATA_VALUE, /* A data message was found to have an          */
                              /* invalid data length.                         */
    E_MBS_SEND_ERR,           /* An error occured while sending a message.    */
    E_MBS_RECEIVE_ERR,        /* An error occured while receiveing a message. */
    E_MBS_BUFFER_OVERFLOW,    /* A supplied buffer was of insuficient size.   */
    E_MBS_EMPTY_REPLY,        /* A reply from a slave device was empty.       */
    E_MBS_REPLY_ERR,          /* An error was returned by a slave device.     */
    E_MBS_INSUFFICIENT_DATA,  /* Insufficient data vailable for operation.    */
    E_MBS_SUSPICIOUS_VALUE,   /* A data value deviates significantly from     */
                              /* that given for other similiar data elements. */
    E_MBS_OID_CONFIG_ERR,     /* Error while attempting to configure an OID.  */
    E_MBS_OID_TABLE_ERR,      /* Error generated accessing the OID table.     */
   
    E_MBS_EOL
};


/*
** Predefined sensor values.
*/

#define E_MBS_MIN_TEMP ( -20000  ) /* Minimum temperature sensor reading (mC) */
#define E_MBS_MAX_TEMP (  40000  ) /* Maximum temperature sensor reading (mC) */
#define E_MBS_ERR_TEMP (  101000 ) /* Error value for temperature sensor (mC) */


/*
** Package OID list.
*/

typedef enum eMbsDataId_e
{
   D_MBS_DATAID_BOL = OID_START(MBS),/* Offset OIDs by package Id.            */

   D_MBS_PROC_STATE,                 /* MbsServer process state.              */
   D_MBS_APP_VERSION,                /* Software revision number.             */
   D_MBS_SHUTDOWN,                   /* MbsServer shutdown flag.              */

   D_MBS_SIMULATE_HW,                /* Simulate hardware interface flag.     */

   D_MBS_TTM1,                       /* First truss temperature value.        */
   D_MBS_TTM2,                       /* Second truss temperature value.       */
   D_MBS_TTM3,                       /* Third truss temperature value.        */
   D_MBS_TTM4,                       /* Fourth truss temperature value.       */

   D_MBS_TTM1_ENABLED,               /* First truss sensor enable flag.       */
   D_MBS_TTM2_ENABLED,               /* Second truss sensor enable flag.      */
   D_MBS_TTM3_ENABLED,               /* Third truss sensor enable flag.       */
   D_MBS_TTM4_ENABLED,               /* Fourth truss sensor enable flag.      */  

   D_MBS_TTM_MEAN,                   /* Mean truss temperature.               */

   D_MBS_TTM_MEAN_ENABLED,           /* Enable calculation of mean truss temp.*/

   D_MBS_PMT1,                       /* First primary mirror temp value.      */ 
   D_MBS_PMT2,                       /* Second primary mirror temp value.     */

   D_MBS_PMT1_ENABLED,               /* 1st primary mirr sensor enable flag.  */
   D_MBS_PMT2_ENABLED,               /* 2nd primary mirr sensor enable flag.  */

   D_MBS_PMT_MEAN,                   /* Mean primary mirror temperature.      */

   D_MBS_PMT_MEAN_ENABLED,           /* Enable calculation of mean primary    */
                                     /* mirror temp.*/
   D_MBS_SMT1,                       /* First secondary mirror temp value.    */ 
   D_MBS_SMT2,                       /* Second secondary mirror temp value.   */

   D_MBS_SMT1_ENABLED,               /* 1st 2ndary mirror sensor enable flag. */
   D_MBS_SMT2_ENABLED,               /* 2nd 2ndary mirror sensor enable flag. */

   D_MBS_SMT_MEAN,                   /* Mean secondary mirror temperature.    */

   D_MBS_SMT_MEAN_ENABLED,           /* Enable calculation of mean secondary  */
                                     /* mirror temp.*/
   D_MBS_AGT1,                       /* First A&G box temperature value.      */
   D_MBS_AGT2,                       /* Second A&G box temperature value.     */ 

   D_MBS_AGT1_ENABLED,               /* First A&G box sensor enable flag.     */
   D_MBS_AGT2_ENABLED,               /* Second A&G box sensor enable flag.    */

   D_MBS_AGT_MEAN,                   /* Mean A&G box temperature.             */

   D_MBS_AGT_MEAN_ENABLED,           /* Enable calculation of mean A&G temp.  */

   D_MBS_DATAID_EOL                  /* End-of-list marker.                   */
} eMbsDataId_t;


#endif

/******************************************************************************
** End of File: Mbs.h
******************************************************************************/
