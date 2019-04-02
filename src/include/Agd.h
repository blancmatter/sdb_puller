/* 
**
**  Module Name : 
**     Agd.h
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
**     $Id: Agd.h,v 0.27 2006/07/12 14:10:34 mjf Exp $
**
**  History: 
**     $Log: Agd.h,v $
**     Revision 0.27  2006/07/12 14:10:34  mjf
**     AGD_1_06.
**
**     Revision 0.26  2005/08/18 10:08:29  mjf
**     AGD_1_04.
**
**     Revision 0.25  2004/06/10 08:11:58  sxf
**     No functional changes.
**
**     Revision 0.24  2004/04/05 09:53:17  mkb
**     First version to run under QNX6
**
**     Revision 0.23  2003/10/03 08:20:40  man
**     Included a function to check the error state after
**     the mechanism changes from moving or homing state
**     to system ready state. If the error is non-zero then
**     the mechanism enters a warning state and the MicroLynx
**     error is reset to zero.
**
**     Revision 0.22  2002/10/15 16:20:45  mjf
**     Version 0.21.
**
**     Revision 0.21  2002/07/31 09:21:10  mjf
**     Change of name of limit switch datum entries - now unused.
**
**     Revision 0.20  2001/11/28 14:31:05  mjf
**     Re-introduction of 'halt demand' OID.
**
**     Revision 0.19  2001/10/12 14:12:14  mjf
**     Removal of TABs from header.
**
**     Revision 0.18  2001/10/10 08:13:24  mjf
**     Change to state model and OID table, such that an 'enable' simply enables
**     subsequent homing, but no longer homes. A new 'home' OID has been added 
**     that allows the mechanism to be homed after the task has been enabled. 
**     Hence, an 'enabled' state has been added between 'disabled' and 'ready'. 
**     The name of the 'HALT_DEMAND' has been alter to be 'STOP_DEMAND', and the
**     name of the 'STOPPED' OID has been altered to remove the package prefix.
**
**     Revision 0.17  2001/08/21 12:55:30  mjf
**     Implementation of the HALT_DEMAND flag as a latch that must be cleared.
**
**     Revision 0.16  2001/08/16 11:27:29  mjf
**     Addition of OID to denote if the current position is at (or close to)
**     zero.
**
**     Revision 0.15  2001/08/14 15:54:19  mjf
**     Addition of flag denoting when all movement is stopped.
**
**     Revision 0.14  2001/08/14 09:46:20  mjf
**     Enum name added to states.
**
**     Revision 0.13  2001/08/14 08:42:08  mjf
**     Implementation of maximum moving time in the configuration file, and
**     alteration to the debug levels.
**
**     Revision 0.12  2001/08/10 10:06:33  mjf
**     Addition of OID for maximum moving time.
**
**     Revision 0.11  2001/05/25 15:41:00  pxh
**     Added OID D_AGD_DARK_SLIDE_FITTED.
**
**     Revision 0.10  2001/04/23 08:36:10  pxh
**     Added D_AGD_AGD_HOME_TOLERANCE OID.
**
**     Revision 0.9  2001/04/09 13:43:01  pxh
**     Added new OIDs to contain upper & lower range of travel for the
**     mechanism.
**
**     Revision 0.8  2001/03/21 16:08:24  pxh
**     Created public enumerations for IR Filter & Dark Slide positions.
**
**     Revision 0.7  2001/02/26 14:29:02  pxh
**     Task state is now offset by package ID.
**
**     Revision 0.6  2001/02/07 16:35:14  pxh
**     Corrected server path to include executable name.
**
**     Revision 0.5  2001/01/30 15:52:11  pxh
**     Changed name server and path identifiers for comptaibility with
**     OidServer.
**
**     Revision 0.4  2001/01/29 14:48:40  pxh
**     Renamed OID D_AGD_BRAKE_STATUS to D_AGD_BRAKE_OFF.
**
**     Revision 0.3  2001/01/18 12:03:04  pxh
**     Added OIDs for Dark Slide mechanism.
**
**     Revision 0.2  2001/01/09 15:01:39  pxh
**     Added OIDs for IR filter and air state.
**     Added extra states for brake, filter and air state.
**
**     Revision 0.1  2001/01/07 15:32:28  pxh
**     Initial revision.
**
**
*/


#ifndef _AGD_H_DEFINED
#define _AGD_H_DEFINED

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_AGD_MAIN
   #define E_AGD_EXTERN
   #define E_AGD_INIT(Initialisation) = Initialisation
#else
   #define E_AGD_EXTERN extern
   #define E_AGD_INIT(Initaliasation)
#endif

/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition                       */


#define E_AGD_REG_SERVER_NAME    "E_TTL_AMN_AGD"
#define E_AGD_SERVER_PROC        "Agd"

#ifdef E_WFL_OS_QNX4
#define E_AGD_SERVER_PATH        "//1/opt/ttl/bin/Agd"
#define E_AGD_SERVER_PRI         (10)
#else
#define E_AGD_SERVER_PATH        "/ttl/sw/bin/Agd"
#define E_AGD_SERVER_PRI         (10)
#endif

typedef struct
{
   Int32_t Value;    /* Actual value of the global task data   */
   Int32_t Units;    /* The units of that value (for decoding) */
} eAgdTaskData_t;

typedef enum eAgdStatus_e
{
    E_AGD_GEN_ERROR = STATUS_START(AGD), /* General AGD error */
    E_AGD_INIT_FAIL,                   /* Initialisation failure */
    E_AGD_SHUTDOWN_ERROR,              /* Error shutting process down */
    E_AGD_TIMER_SETUP_ERR,             /* Error setting up timer */
    E_AGD_TIMER_CLOSE_ERR,             /* Error closing timer */
    E_AGD_INVALID_STATE,               /* Invalid command for current state */
    E_AGD_SERVER_NOT_FOUND,            /* Unable to locate server */
    E_AGD_SEND_FAIL,                   /* Failure sending command */
    E_AGD_INVALID_COMMAND,             /* Invalid command */
    E_AGD_BRAKE_FAIL,                  /* Autoguider brake failure */
    E_AGD_OID_CONFIG_ERR,              /* Data ID configuration error */
    E_AGD_RECEIVE_ERR,                 /* Message receive failure */
    E_AGD_HOME_ERR,                    /* Error homing autoguider deployment mechanism */
    E_AGD_MOVE_ERR,                    /* Error moving autoguider deployment mechanism */
    E_AGD_STOPPED                      /* Command disallowed whilst stopped */
} eAgdStatus_t;

/*
** Package OID list.
*/

typedef enum eAgdDataId_e
{
   D_AGD_DATAID_BOL = OID_START(AGD),  /* Offset OIDs by package Id      */

   D_AGD_PROC_STATE,                   /* Task process state             */
   D_AGD_APP_VERSION,                  /* Package revision number        */
   D_AGD_SHUTDOWN,                     /* Task shutdown flag             */
   D_AGD_STATE,                        /* Autoguider Focus State         */
   D_AGD_ENABLE,                       /* Task enable flag               */
   D_AGD_HOME,                         /* Home the main mechanism        */
   D_AGD_DEMAND,                       /* Position demand                */
   D_AGD_ACTUAL,                       /* Actual linear position         */
   D_AGD_UNUSED_LIMIT_LO,              /* Low limit switch state         */
   D_AGD_UNUSED_LIMIT_HI,              /* High limit switch state        */
   D_AGD_BRAKE_STATE,                  /* Brake operating state          */
   D_AGD_BRAKE_OFF,                    /* Brake status                   */
   D_AGD_FILTER_STATE,                 /* Filter operating state         */
   D_AGD_FILTER_DEMAND,                /* Filter position demand         */
   D_AGD_FILTER_ACTUAL,                /* Actual filter position         */
   D_AGD_DARK_SLIDE_STATE,             /* Dark Slide operating state     */
   D_AGD_DARK_SLIDE_DEMAND,            /* Slide position demand          */
   D_AGD_DARK_SLIDE_ACTUAL,            /* Actual dark slide position     */
   D_AGD_AIR_STATE,                    /* Air pressure state             */
   D_AGD_STOP_DEMAND,                  /* Task stop flag                 */
   D_AGD_AGD_RANGE_LO,                 /* Lower travel limit for axis    */
   D_AGD_AGD_RANGE_HI,                 /* Upper travel limit for axis    */
   D_AGD_AGD_HOME_TOLERANCE,           /* Used in homing sequence        */
   D_AGD_DARK_SLIDE_FITTED,            /* Flag for Dark Slide present    */
   D_AGD_AGD_MAX_MOVING_TIME,          /* Maximum moving time (msec)     */
   D_AGD_STOPPED,                      /* Denoting all movement stopped  */
   D_AGD_AGD_AT_ZERO,                  /* Denoting current position ~ 0  */
   D_AGD_HALT_DEMAND,                  /* Task halt flag                 */
   D_AGD_AGD_ERROR,                    /* MicroLynx error state          */

   D_AGD_AGD_POSITION_OFFSET,          /* Offset to apply to read pos    */
   D_AGD_AGD_POSITION_SCALAR,          /* Factor to apply to offset pos  */
   D_AGD_AGD_ULYNX_DEMAND,             /* Demand pos sent to MicroLynx   */
   D_AGD_AGD_ULYNX_ACTUAL,             /* Actual pos read from ,icroLynx */

   D_AGD_AGD_POS_TOLERANCE,            /* Allowable positional tolerance */

   D_AGD_AGD_ULYNX_ERROR,              /* MicroLynx error code           */

   D_AGD_AGD_AT_LO,                    /* At lower limit                 */
   D_AGD_AGD_AT_HI,                    /* At upper limit                 */
   D_AGD_AGD_MOVE_LO,                  /* Move to lower limit            */
   D_AGD_AGD_MOVE_HI,                  /* Move to upper limit            */

   D_AGD_DATAID_EOL                    /* End-of-list marker             */
} eAgdDataId_t;

typedef enum eAgdStates_e
{
   E_AGD_STATE_DISABLED = STATUS_START(AGD),
   E_AGD_STATE_ENABLED,
   E_AGD_STATE_HOMING,
   E_AGD_STATE_READY,
   E_AGD_STATE_MOVING,
   E_AGD_STATE_ERROR,
   E_AGD_STATE_OK,
   E_AGD_STATE_FAIL
} eAgdStates_t;

/*
** Define enum for valid IR Filter positions.
*/

typedef enum eAgdIRPos_e
{
   E_AGD_IR_UNSET  = 0,
   E_AGD_IR_RETRACT,
   E_AGD_IR_IN_LINE
} eAgdIRPos_t;

/*
** Define enum for valid side-port numbers.
*/

typedef enum eAgdDSPos_e
{
   E_AGD_SLIDE_UNSET  = 0,
   E_AGD_SLIDE_OPEN,
   E_AGD_SLIDE_CLOSE
} eAgdDSPos_t;


/*
** External function prototypes
*/

#endif

/*
** EOF
*/
