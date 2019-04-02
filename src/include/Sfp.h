/* 
**
**  Module Name : 
**     Sfp.h
**
**  Purpose :
**     Public header file for package.
**
**  Description :
**     Used to declare exported data, functions and definitions for the package.
**
**  Authors :
**     PXH : Pete Hall (TTL)
**
**  Version :
**     $Id: Sfp.h,v 0.26 2007/09/07 15:32:52 mjf Exp $
**
**  History: 
**     $Log: Sfp.h,v $
**     Revision 0.26  2007/09/07 15:32:52  mjf
**     Re-format comment for tidier IdTable generation - no functional changes.
**
**     Revision 0.25  2006/10/25 07:40:39  mjf
**     Maximum port is port 8, not 4. Ports 5-8 to be 45 degrees beyond ports
**     1-4 respectively.
**
**     Revision 0.24  2006/07/12 14:22:19  mjf
**     SFP_1_01.
**
**     Revision 0.23  2005/09/29 10:05:25  sxf
**     Ported to QNX6 and brought in line with QNX4 baseline SFP_0_28.
**
**     Revision 0.22  2003/10/03 11:49:54  man
**     Included functionality to check MicroLynx error
**     state and set the process state to warning if
**     we have an error.
**
**     Revision 0.21  2002/10/15 16:24:27  mjf
**     Version 0.20.
**
**     Revision 0.20  2002/07/31 09:46:59  mjf
**     Change of name of limit switch datum entries - now unused.
**
**     Revision 0.19  2001/11/28 14:37:19  mjf
**     Re-introduction of 'halt demand' OID.
**
**     Revision 0.18  2001/11/01 12:06:39  mjf
**     Introduction of range-chacking into demanded port positions.
**
**     Revision 0.17  2001/10/12 15:11:56  mjf
**     Removal of TABs from the public header.
**
**     Revision 0.16  2001/10/10 08:16:15  mjf
**     Change to state model and OID table, such that an 'enable' simply enables
**     subsequent homing, but no longer homes. A new 'home' OID has been added 
**     that allows the mechanism to be homed after the task has been enabled. 
**     Hence, an 'enabled' state has been added between 'disabled' and 'ready'.
**     The name of the 'HALT_DEMAND' has been alter to be 'STOP_DEMAND', and the
**     name of the 'STOPPED' OID has been altered to remove the package prefix.
**
**     Revision 0.15  2001/08/21 12:56:49  mjf
**     Implementation of the HALT_DEMAND flag as a latch that must be cleared.
**
**     Revision 0.14  2001/08/16 11:27:29  mjf
**     Addition of OID to denote if the current position is at (or close to)
**     zero.
**
**     Revision 0.13  2001/08/14 15:54:19  mjf
**     Addition of flag denoting when all movement is stopped.
**
**     Revision 0.12  2001/08/14 09:57:37  mjf
**     Enum name added to states. Identifier name for port unset changed to
**     avoid HTI clashes with SFD unset.
**
**     Revision 0.11  2001/08/14 08:49:52  mjf
**     Implementation of maximum moving time in the configuration file, and
**     alteration to the debug levels.
**
**     Revision 0.10  2001/08/10 10:06:19  mjf
**     Addition of OID for maximum moving time.
**
**     Revision 0.9  2001/05/04 11:03:02  pxh
**     Added OIDs D_SFP_PORT_x_POSITION (x=1..8), D_SFP_NO_PORTS &
**     D_SFP_SFP_TOLERANCE to OID list.
**     Removed OIDs D_SFP_PORT_0_OFFSET, D_SFP_POSITION_TOLERANCE &
**     D_SFP_SFP_HOME_TOLERANCE.
**     Added definitions for side-ports 5 to 8 to enumerated list.
**
**     Revision 0.8  2001/04/23 14:29:36  pxh
**     Added definitions for D_SFP_DEMAND_ANGULAR & D_SFP_SFP_HOME_TOLERANCE
**     OIDs.
**
**     Revision 0.7  2001/03/21 09:51:18  pxh
**     Created external enumerated type for side-port position definitions.
**
**     Revision 0.6  2001/03/12 11:24:33  pxh
**     OIDs D_SFP_APP_VERSION, D_SFP_ACTUAL_ANGULAR, D_SFP_PORT_0_POSITION and
**     D_SFP_POSITION_TOLERANCE added.
**
**     Revision 0.5  2001/02/26 14:33:53  pxh
**     Task state is now offset by package ID.
**
**     Revision 0.4  2001/02/07 17:05:48  pxh
**     Corrected server path to include executable name.
**
**     Revision 0.3  2001/01/30 17:15:24  pxh
**     Changed name server & path definitions for compatibility with
**     OidServer.
**
**     Revision 0.2  2001/01/15 15:48:22  pxh
**     Added new state E_SFP_STATE_MOVING_UNHOMED.
**
**     Revision 0.1  2001/01/07 15:42:52  pxh
**     Initial revision.
**
**
*/


#ifndef _SFP_H_DEFINED
#define _SFP_H_DEFINED

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_SFP_MAIN
   #define E_SFP_EXTERN
   #define E_SFP_INIT(Initialisation) = Initialisation
#else
   #define E_SFP_EXTERN extern
   #define E_SFP_INIT(Initaliasation)
#endif

/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition                       */


#define E_SFP_REG_SERVER_NAME    "E_TTL_AMN_SFP"
#define E_SFP_SERVER_PROC        "Sfp"
#define E_SFP_SERVER_PATH        "//1/opt/ttl/bin/Sfp"
#define E_SFP_SERVER_PRI         10

typedef struct
{
   Int32_t Value;    /* Actual value of the global task data   */
   Int32_t Units;    /* The units of that value (for decoding) */
} eSfpTaskData_t;

typedef enum eSfpStatus_e
{
    E_SFP_GEN_ERROR = STATUS_START(SFP), /* General SFP error */                           
    E_SFP_INIT_FAIL,                   /* Initialisation failure */                        
    E_SFP_SHUTDOWN_ERROR,              /* Error shutting process down */                   
    E_SFP_TIMER_SETUP_ERR,             /* Error setting up timer */                        
    E_SFP_TIMER_CLOSE_ERR,             /* Error closing timer */                           
    E_SFP_INVALID_STATE,               /* Invalid command for current state */             
    E_SFP_SERVER_NOT_FOUND,            /* Unable to locate server */                       
    E_SFP_SEND_FAIL,                   /* Failure sending command */                       
    E_SFP_INVALID_COMMAND,             /* Invalid command */                               
    E_SFP_OID_CONFIG_ERR,              /* Data ID configuration error */                   
    E_SFP_RECEIVE_ERR,                 /* Message receive failure */                       
    E_SFP_HOME_ERR,                    /* Error homing science fold rotation mechanism */
    E_SFP_MOVE_ERR,                    /* Error moving science fold rotation mechanism */
    E_SFP_STOPPED                      /* Command disallowed whilst stopped */             
} eSfpStatus_t;

/*
** Package OID list.
*/

typedef enum eSfpDataId_e
{
   D_SFP_DATAID_BOL = OID_START(SFP),  /* Offset OIDs by package Id      */

   D_SFP_PROC_STATE,                   /* Task process state             */
   D_SFP_APP_VERSION,                  /* Package Version Number         */
   D_SFP_SHUTDOWN,                     /* Task Shutdown flag             */
   D_SFP_STATE,                        /* Autoguider Focus State         */
   D_SFP_ENABLE,                       /* Task Enable flag               */
   D_SFP_HOME,                         /* Home the main mechanism        */
   D_SFP_DEMAND,                       /* Requested side-port position   */
   D_SFP_ACTUAL,                       /* Actual side-port position      */
   D_SFP_DEMAND_ANGULAR,               /* Requested angular position     */
   D_SFP_ACTUAL_ANGULAR,               /* Actual angular position        */
   D_SFP_UNUSED_HOME_SWITCH,           /* Home switch status             */
   D_SFP_STOP_DEMAND,                  /* Task stop flag                 */
   D_SFP_SFP_TOLERANCE,                /* Tolerance for homing/port pos. */
   D_SFP_PORT_1_POSITION,              /* Port 1 offset from home        */
   D_SFP_PORT_2_POSITION,              /* Port 2 offset from home        */
   D_SFP_PORT_3_POSITION,              /* Port 3 offset from home        */
   D_SFP_PORT_4_POSITION,              /* Port 4 offset from home        */
   D_SFP_PORT_5_POSITION,              /* Port 5 offset from home        */
   D_SFP_PORT_6_POSITION,              /* Port 6 offset from home        */
   D_SFP_PORT_7_POSITION,              /* Port 7 offset from home        */
   D_SFP_PORT_8_POSITION,              /* Port 8 offset from home        */
   D_SFP_NO_PORTS,                     /* No. of available side-ports    */
   D_SFP_SFP_MAX_MOVING_TIME,          /* Maximum moving time (msec)     */
   D_SFP_STOPPED,                      /* Denoting all movement stopped  */
   D_SFP_SFP_AT_ZERO,                  /* Denoting current position ~ 0  */
   D_SFP_HALT_DEMAND,                  /* Task halt flag                 */
   D_SFP_SFP_ERROR,                    /* Error state of MicroLynx       */

   D_SFP_SFP_POS_TOLERANCE,            /* Tolerance if demand != actual  */

   D_SFP_SFP_ULYNX_ERROR,              /* MicroLynx error code           */

   D_SFP_SFP_AT_LO,                    /* At lower limit                 */
   D_SFP_SFP_AT_HI,                    /* At upper limit                 */
   D_SFP_SFP_MOVE_LO,                  /* Move to lower limit            */
   D_SFP_SFP_MOVE_HI,                  /* Move to upper limit            */

   D_SFP_DATAID_EOL                    /* End-of-list marker             */
} eSfpDataId_t;

typedef enum eSfpStates_e
{
   E_SFP_STATE_DISABLED = STATUS_START(SFP),
   E_SFP_STATE_ENABLED,
   E_SFP_STATE_HOMING,
   E_SFP_STATE_READY,
   E_SFP_STATE_MOVING,
   E_SFP_STATE_ERROR,
   E_SFP_STATE_OK,
   E_SFP_STATE_FAIL
} eSfpStates_t;

/*
** Define enum for valid side-port numbers.
*/

typedef enum eSfpPort_e
{
   E_SFP_POS_PORT_UNSET = 0,
   E_SFP_POS_PORT_1,
   E_SFP_POS_PORT_2,
   E_SFP_POS_PORT_3,
   E_SFP_POS_PORT_4,
   E_SFP_POS_PORT_5,
   E_SFP_POS_PORT_6,
   E_SFP_POS_PORT_7,
   E_SFP_POS_PORT_8
} eSfpPort_t;

/*
** Definition of maximum and minimum side ports.
*/

#define E_SFP_POS_MIN        E_SFP_POS_PORT_1
#define E_SFP_POS_MAX        E_SFP_POS_PORT_8


/*
** External function prototypes
*/

#endif

/*
** EOF
*/
