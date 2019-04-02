/* 
**
**  Module Name : 
**     Agf.h
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
**     $Id: Agf.h,v 0.23 2006/07/12 14:13:12 mjf Exp $
**
**  History: 
**     $Log: Agf.h,v $
**     Revision 0.23  2006/07/12 14:13:12  mjf
**     AGF_1_05.
**
**     Revision 0.22  2005/08/18 10:11:06  mjf
**     AGF_1_03.
**
**     Revision 0.21  2003/10/03 10:14:26  man
**     Included functionality to check the MicroLynx error.
**
**     Revision 0.20  2002/10/15 16:21:37  mjf
**     Version 0.21.
**
**     Revision 0.19  2002/07/31 09:24:20  mjf
**     Change of name of limit switch datum entries - now unused.
**
**     Revision 0.18  2001/11/28 14:32:48  mjf
**     Re-introduction of 'halt demand' OID.
**
**     Revision 0.17  2001/10/12 14:51:24  mjf
**     Removal of TABs from the header.
**
**     Revision 0.16  2001/10/10 08:14:29  mjf
**     Change to state model and OID table, such that an 'enable' simply enables
**     subsequent homing, but no longer homes. A new 'home' OID has been added 
**     that allows the mechanism to be homed after the task has been enabled. 
**     Hence, an 'enabled' state has been added between 'disabled' and 'ready'. 
**     The name of the 'HALT_DEMAND' has been alter to be 'STOP_DEMAND', and the
**     name of the 'STOPPED' OID has been altered to remove the package prefix.
**
**     Revision 0.15  2001/08/21 12:56:01  mjf
**     Implementation of the HALT_DEMAND flag as a latch that must be cleared.
**
**     Revision 0.14  2001/08/16 11:27:29  mjf
**     Addition of OID to denote if the current position is at (or close to)
**     zero.
**
**     Revision 0.13  2001/08/14 15:54:19  mjf
**     Addition of flag denoting when all movement is stopped.
**
**     Revision 0.12  2001/08/14 09:47:05  mjf
**     Enum name added to states.
**
**     Revision 0.11  2001/08/14 08:45:33  mjf
**     Implementation of maximum moving time in the configuration file, and
**     alteration to the debug levels.
**
**     Revision 0.10  2001/08/10 10:06:47  mjf
**     Addition of OID for maximum moving time.
**
**     Revision 0.9  2001/04/24 09:27:05  pxh
**     Added definition for D_AGF_APP_VERSION to OID list.
**
**     Revision 0.8  2001/04/23 08:53:27  pxh
**     Added D_AGF_AGF_HOME_TOLERANCE definition.
**
**     Revision 0.7  2001/04/09 14:27:07  pxh
**     Added OIDs D_AGF_AGF_RANGE_LO & D_AGF_AGF_RANGE_HI.
**
**     Revision 0.6  2001/02/26 14:30:49  pxh
**     Task state is now offset by package ID.
**
**     Revision 0.5  2001/02/07 16:49:55  pxh
**     Corrected server path to include executable name.
**
**     Revision 0.4  2001/01/31 14:33:56  pxh
**     Changed definition E_AGD_TASK_PRI to E_AGD_SERVER_PRI for
**     compatibilty with OidServer.
**
**     Revision 0.3  2001/01/30 16:15:01  pxh
**     Changed name server and path definitions for compatibility with
**     OidServer.
**     
**     Revision 0.2  2001/01/04 14:37:16  pxh
**     Removed D_AGF_BRAKE_STATUS OID.
**
**     Revision 0.1  2001/01/04 11:21:18  pxh
**     Initial revision - tested with Autoguider translation stage on bench.
**
**
*/


#ifndef _AGF_H_DEFINED
#define _AGF_H_DEFINED

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_AGF_MAIN
   #define E_AGF_EXTERN
   #define E_AGF_INIT(Initialisation) = Initialisation
#else
   #define E_AGF_EXTERN extern
   #define E_AGF_INIT(Initaliasation)
#endif

/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition                       */


#define E_AGF_REG_SERVER_NAME    "E_TTL_AMN_AGF"
#define E_AGF_SERVER_PROC        "Agf"
#define E_AGF_SERVER_PATH        "//1/opt/ttl/bin/Agf"
#define E_AGF_SERVER_PRI         10

typedef struct
{
   Int32_t Value;    /* Actual value of the global task data   */
   Int32_t Units;    /* The units of that value (for decoding) */
} eAgfTaskData_t;

typedef enum eAgfStatus_e
{
    E_AGF_GEN_ERROR = STATUS_START(AGF), /* General AGF error */                         
    E_AGF_INIT_FAIL,                   /* Initialisation failure */                      
    E_AGF_SHUTDOWN_ERROR,              /* Error shutting process down */                 
    E_AGF_TIMER_SETUP_ERR,             /* Error setting up timer */                      
    E_AGF_TIMER_CLOSE_ERR,             /* Error closing timer */                         
    E_AGF_INVALID_STATE,               /* Invalid command for current state */           
    E_AGF_SERVER_NOT_FOUND,            /* Unable to locate server */                     
    E_AGF_SEND_FAIL,                   /* Failure sending command */                     
    E_AGF_INVALID_COMMAND,             /* Invalid command */                             
    E_AGF_OID_CONFIG_ERR,              /* Data ID configuration error */                 
    E_AGF_RECEIVE_ERR,                 /* Message receive failure */                     
    E_AGF_HOME_ERR,                    /* Error homing autoguider focus mechanism */
    E_AGF_MOVE_ERR,                    /* Error moving autoguider focus mechanism */
    E_AGF_STOPPED                      /* Command disallowed whilst stopped */           
} eAgfStatus_t;                        

/*
** Package OID list.
*/

typedef enum eAgfDataId_e
{
   D_AGF_DATAID_BOL = OID_START(AGF),  /* Offset OIDs by package Id      */

   D_AGF_PROC_STATE,                   /* Task process state             */
   D_AGF_APP_VERSION,                  /* Package revision number        */
   D_AGF_SHUTDOWN,                     /* Task shutdown flag             */
   D_AGF_STATE,                        /* Autoguider Focus State         */
   D_AGF_ENABLE,                       /* Task enable flag               */
   D_AGF_HOME,                         /* Home the main mechanism        */
   D_AGF_DEMAND,                       /* Position demand                */
   D_AGF_ACTUAL,                       /* Actual linear position         */
   D_AGF_UNUSED_LIMIT_LO,              /* Low limit switch state         */
   D_AGF_UNUSED_LIMIT_HI,              /* High limit switch state        */
   D_AGF_STOP_DEMAND,                  /* Task stop flag                 */
   D_AGF_AGF_RANGE_LO,                 /* Lower travel limit for axis    */
   D_AGF_AGF_RANGE_HI,                 /* Upper travel limit for axis    */
   D_AGF_AGF_HOME_TOLERANCE,           /* Used in homing sequence        */
   D_AGF_AGF_MAX_MOVING_TIME,          /* Maximum moving time (msec)     */
   D_AGF_STOPPED,                      /* Denoting all movement stopped  */
   D_AGF_AGF_AT_ZERO,                  /* Denoting current position ~ 0  */
   D_AGF_HALT_DEMAND,                  /* Task halt flag                 */
   D_AGF_AGF_ERROR,                    /* Error detected on uLynx        */

   D_AGF_AGF_POSITION_OFFSET,          /* Offset to apply to read pos    */
   D_AGF_AGF_POSITION_SCALAR,          /* Factor to apply to offset pos  */
   D_AGF_AGF_ULYNX_DEMAND,             /* Demand pos sent to MicroLynx   */
   D_AGF_AGF_ULYNX_ACTUAL,             /* Actual pos read from MicroLynx */

   D_AGF_AGF_POS_TOLERANCE,            /* Allowable positional tolerance */

   D_AGF_AGF_ULYNX_ERROR,              /* MicroLynx error code           */

   D_AGF_AGF_AT_LO,                    /* At lower limit                 */
   D_AGF_AGF_AT_HI,                    /* At upper limit                 */
   D_AGF_AGF_MOVE_LO,                  /* Move to lower limit            */
   D_AGF_AGF_MOVE_HI,                  /* Move to upper limit            */

   D_AGF_DATAID_EOL                    /* End-of-list marker             */
} eAgfDataId_t;

typedef enum eAgfStates_e
{
   E_AGF_STATE_DISABLED = STATUS_START(AGF),
   E_AGF_STATE_ENABLED,
   E_AGF_STATE_HOMING,
   E_AGF_STATE_READY,
   E_AGF_STATE_MOVING,
   E_AGF_STATE_ERROR,
   E_AGF_STATE_OK,
   E_AGF_STATE_FAIL
} eAgfStates_t;

/*
** External function prototypes
*/

#endif

/*
** EOF
*/
