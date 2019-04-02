/* 
**
**  Module Name : 
**     Sfd.h
**
**  Purpose :
**     Public header file for SFD package.
**
**  Description :
**     Used to declare exported data, functions and definitions for the package.
**
**  Authors :
**     PXH : Pete Hall (TTL)
**
**  Version :
**     $Id: Sfd.h,v 0.22 2007/09/07 15:29:48 mjf Exp $
**
**  History: 
**     $Log: Sfd.h,v $
**     Revision 0.22  2007/09/07 15:29:48  mjf
**     Re-format comment for tidier IdTable generation - no functional changes.
**
**     Revision 0.21  2006/07/12 14:16:54  mjf
**     SFD_1_02.
**
**     Revision 0.20  2005/09/27 13:28:39  sxf
**     Added new datum D_SFD_SFD_POS_TOLERANCE used to check if
**     actual - demand position is close enough, otherwise instigate
**     a new move.
**
**     Revision 0.19  2003/10/03 10:40:17  man
**     Included functionalilty to check MicroLynx error
**     status following a home or a move.
**
**     Revision 0.18  2002/10/15 16:22:25  mjf
**     Version 0.18.
**
**     Revision 0.17  2002/07/31 09:27:14  mjf
**     Change of name of limit switch datum entries - now unused.
**
**     Revision 0.16  2001/11/28 14:36:21  mjf
**     Re-introduction of 'halt demand' OID.
**
**     Revision 0.15  2001/10/12 15:05:56  mjf
**     Removal of TABs from header.
**
**     Revision 0.14  2001/10/10 08:15:25  mjf
**     Change to state model and OID table, such that an 'enable' simply enables
**     subsequent homing, but no longer homes. A new 'home' OID has been added 
**     that allows the mechanism to be homed after the task has been enabled. 
**     Hence, an 'enabled' state has been added between 'disabled' and 'ready'.
**     The name of the 'HALT_DEMAND' has been alter to be 'STOP_DEMAND', and the
**     name of the 'STOPPED' OID has been altered to remove the package prefix.
**
**     Revision 0.13  2001/08/21 12:56:26  mjf
**     Implementation of the HALT_DEMAND flag as a latch that must be cleared.
**
**     Revision 0.12  2001/08/16 11:27:29  mjf
**     Addition of OID to denote if the current position is at (or close to)
**     zero.
**
**     Revision 0.11  2001/08/14 15:54:19  mjf
**     Addition of flag denoting when all movement is stopped.
**
**     Revision 0.10  2001/08/14 10:02:57  mjf
**     Enum name added to states.
**
**     Revision 0.9  2001/08/14 08:48:25  mjf
**     Implementation of maximum moving time in the configuration file, and
**     alteration to the debug levels.
**
**     Revision 0.8  2001/08/10 10:06:01  mjf
**     Addition of OID for maximum moving time.
**
**     Revision 0.7  2001/04/23 09:10:35  pxh
**     Added definition of D_SFD_SFD_HOME_TOLERANCE OID.
**
**     Revision 0.6  2001/03/21 09:12:36  pxh
**     Created external definitions for demand & actual OIDs.
**
**     Revision 0.5  2001/03/14 16:49:47  pxh
**     Added OIDs D_SFP_APP_VERSION, D_SFP_ACTUAL_LINEAR, D_SFD_STOW_POSITION,
**     D_SFD_DEPLOY_POSITION & D_SFD_STOW_DEPLOY_TOLERANCE.
**     Added status code E_SFD_OID_CONFIG_ERR.
**
**     Revision 0.4  2001/02/26 14:32:59  pxh
**     Task state is now offset by package ID.
**
**     Revision 0.3  2001/02/07 16:53:55  pxh
**     Corrected server path to include executable name.
**
**     Revision 0.2  2001/01/30 16:39:27  pxh
**     Changed name server and path definitions for compatibility with
**     OidServer.
**
**     Revision 0.1  2000/12/22 11:06:28  pxh
**     Initial revision - tested with CanServer, no hardware.
**
**
*/


#ifndef _SFD_H_DEFINED
#define _SFD_H_DEFINED

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_SFD_MAIN
   #define E_SFD_EXTERN
   #define E_SFD_INIT(Initialisation) = Initialisation
#else
   #define E_SFD_EXTERN extern
   #define E_SFD_INIT(Initaliasation)
#endif

/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition                       */


#define E_SFD_REG_SERVER_NAME    "E_TTL_AMN_SFD"
#define E_SFD_SERVER_PROC        "Sfd"
#define E_SFD_SERVER_PATH        "//1/opt/ttl/bin/Sfd"
#define E_SFD_SERVER_PRI         (10)

typedef struct
{
   Int32_t Value;    /* Actual value of the global task data   */
   Int32_t Units;    /* The units of that value (for decoding) */
} eSfdTaskData_t;

typedef enum eSfdStatus_e
{
    E_SFD_GEN_ERROR = STATUS_START(SFD), /* General SFD error */                    
    E_SFD_INIT_FAIL,                   /* Initialisation failure */                 
    E_SFD_SHUTDOWN_ERROR,              /* Error shutting process down */            
    E_SFD_TIMER_SETUP_ERR,             /* Error setting up timer */                 
    E_SFD_TIMER_CLOSE_ERR,             /* Error closing timer */                    
    E_SFD_INVALID_STATE,               /* Invalid command for current state */      
    E_SFD_SERVER_NOT_FOUND,            /* Unable to locate server */                
    E_SFD_SEND_FAIL,                   /* Failure sending command */                
    E_SFD_INVALID_COMMAND,             /* Invalid command */                        
    E_SFD_OID_CONFIG_ERR,              /* Data ID configuration error */            
    E_SFD_RECEIVE_ERR,                 /* Message receive failure */                
    E_SFD_HOME_ERR,                    /* Error homing science fold deployment mechanism */
    E_SFD_MOVE_ERR,                    /* Error moving science fold deployment mechanism */
    E_SFD_STOPPED                      /* Command disallowed whilst stopped */      
} eSfdStatus_t;

/*
** Package OID list.
*/

typedef enum eSfdDataId_e
{
   D_SFD_DATAID_BOL = OID_START(SFD),  /* Offset OIDs by package Id      */

   D_SFD_PROC_STATE,                   /* Task process state             */
   D_SFD_APP_VERSION,                  /* Task shutdown flag             */
   D_SFD_SHUTDOWN,                     /* Task shutdown flag             */
   D_SFD_STATE,                        /* Sci. Fold Deployment State     */
   D_SFD_ENABLE,                       /* Task Enable Flag               */
   D_SFD_HOME,                         /* Home the main mechanism        */
   D_SFD_DEMAND,                       /* Requested mechanism position   */
   D_SFD_ACTUAL,                       /* Actual mechanism position      */
   D_SFD_ACTUAL_LINEAR,                /* Actual linear position         */
   D_SFD_DEPLOY_POSITION,              /* Deploy Position                */
   D_SFD_STOW_POSITION,                /* Stow Position                  */
   D_SFD_STOW_DEPLOY_TOLERANCE,        /* Used in position calculation   */
   D_SFD_UNUSED_LIMIT_LO,              /* Low limit switch status        */
   D_SFD_UNUSED_LIMIT_HI,              /* High limit switch status       */
   D_SFD_STOP_DEMAND,                  /* Task stop Flag                 */
   D_SFD_SFD_HOME_TOLERANCE,           /* Used in homing sequence        */
   D_SFD_SFD_MAX_MOVING_TIME,          /* Maximum moving time (msec)     */
   D_SFD_STOPPED,                      /* Denoting all movement stopped  */
   D_SFD_SFD_AT_ZERO,                  /* Denoting current position ~ 0  */
   D_SFD_HALT_DEMAND,                  /* Task halt flag                 */
   D_SFD_SFD_ERROR,                    /* Error state of the MicroLynx   */

   D_SFD_DEMAND_LINEAR,                /* Requested mech posn, microns   */
   D_SFD_SFD_POS_TOLERANCE,            /* Tolerance if demand != actual  */

   D_SFD_SFD_ULYNX_ERROR,              /* MicroLynx error code           */

   D_SFD_SFD_AT_LO,                    /* At lower limit                 */
   D_SFD_SFD_AT_HI,                    /* At upper limit                 */
   D_SFD_SFD_MOVE_LO,                  /* Move to lower limit            */
   D_SFD_SFD_MOVE_HI,                  /* Move to upper limit            */

   D_SFD_DATAID_EOL                    /* End-of-list marker             */
} eSfdDataId_t;

typedef enum eSfdStates_e
{
   E_SFD_STATE_DISABLED = STATUS_START(SFD),
   E_SFD_STATE_ENABLED,
   E_SFD_STATE_HOMING,
   E_SFD_STATE_READY,
   E_SFD_STATE_MOVING,
   E_SFD_STATE_ERROR,
   E_SFD_STATE_OK,
   E_SFD_STATE_FAIL
} eSfdStates_t;

/*
** Define enum for valid side-port numbers.
*/

typedef enum eSfdPos_e
{
   E_SFD_POS_UNSET  = 0,
   E_SFD_POS_STOW,
   E_SFD_POS_DEPLOY
} eSfdPos_t;

/*
** External function prototypes
*/

#endif

/*
** EOF
*/
