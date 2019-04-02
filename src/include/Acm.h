/* 
**
**  Module Name : 
**     Acm.h
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
**     $Id: Acm.h,v 0.21 2006/07/12 14:01:31 mjf Exp $
**
**  History: 
**     $Log: Acm.h,v $
**     Revision 0.21  2006/07/12 14:01:31  mjf
**     Brought in line with ACM_0_22.
**
**     Revision 0.20  2003/11/28 14:40:58  man
**     Check the MicroLynx error flag following a home or move, specifically
**     for a stall or limit error (checked in by MJF posing as MAN).
**
**     Revision 0.19  2001/11/28 14:28:20  mjf
**     Re-introduction of 'halt demand' OID.
**
**     Revision 0.18  2001/10/19 11:02:56  mjf
**     Addition of functionality to re-collimate from saved parameters.
**
**     Revision 0.17  2001/10/12 13:46:44  mjf
**     Removal of TABs in the header.
**
**     Revision 0.16  2001/10/08 07:24:49  mjf
**     Re-design of state machine to allow each actuator and linear encoder
**     to be homed individually.
**
**     Revision 0.15  2001/10/04 15:42:29  mjf
**     Implementation of relative and absolute moving states.
**
**     Revision 0.14  2001/10/04 14:35:45  mjf
**     Linear encoder manufacturer's calibration now held in OIDs to enable
**     setting from a configuration file.
**
**     Revision 0.13  2001/10/04 13:59:30  mjf
**     Addition of OIDs to home actuators and linear encoders individually.
**
**     Revision 0.12  2001/10/03 10:23:10  mjf
**     Change of wording - linear encoder is not extended when homed.
**
**     Revision 0.11  2001/10/03 10:09:02  mjf
**     Change of OID names to truncate colours to three letters.
**
**     Revision 0.10  2001/10/02 15:37:45  mjf
**     Stripped identifer from 'stopped' OID.
**
**     Revision 0.9  2001/10/02 14:23:28  mjf
**     Re-ording of OIDs into a more logical order.
**
**     Revision 0.8  2001/10/02 14:11:46  mjf
**     Re-writing of code to use loops around each actuator/encoder. Zero
**     MicroLynx IO 26 on start-up (linear encoder plunger) for each. Addition
**     of an 'enabled' state which is entered prior to homing - a relative
**     move may be done in this state.
**
**     Revision 0.7  2001/10/01 15:08:01  mjf
**     Addition of functionality to read and home the linear encoders. The
**     linear encoder positions may be saved as 'collimated position'. When
**     the actuators have not been homed, a relative move is permitted.
**
**     Revision 0.6  2001/09/26 14:23:14  mjf
**     Extensive re-writing of code to improve handling of actuators as single
**     entities. Successfully tested on the bench with a single actuator.
**
**     Revision 0.5  2001/08/24 09:30:56  mjf
**     Addition of MAX_MOVING_TIME and AT_ZERO OIDs.
**
**     Revision 0.4  2001/08/24 09:28:20  mjf
**     Change of debug levels.
**
**     Revision 0.3  2001/08/16 15:25:17  mjf
**     Implementation of 'stopped' OID to indicate movement is stopped.
**
**     Revision 0.2  2001/05/01 09:19:02  pxh
**     Added E_ACM_OID_CONFIG_ERR to list of status codes.
**     Added OIDs for linear encoder reading & home switch status for each
**     axis, and travel limits for all axes to OID list.
**
**     Revision 0.1  2001/03/06 17:10:24  pxh
**     Initial revision.
**
**
*/


#ifndef _ACM_H_DEFINED
#define _ACM_H_DEFINED

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_ACM_MAIN
   #define E_ACM_EXTERN
   #define E_ACM_INIT(Initialisation) = Initialisation
#else
   #define E_ACM_EXTERN extern
   #define E_ACM_INIT(Initaliasation)
#endif

/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition                       */


#define E_ACM_REG_SERVER_NAME    "E_TTL_AMN_ACM"
#define E_ACM_SERVER_PROC        "Acm"
#define E_ACM_SERVER_PATH        "//1/opt/ttl/bin/Acm"
#define E_ACM_SERVER_PRI         10

typedef struct
{
   Int32_t Value;    /* Actual value of the global task data   */
   Int32_t Units;    /* The units of that value (for decoding) */
} eAcmTaskData_t;

/*
** Status and error codes.
*/

enum eAcmStatus
{
    E_ACM_GEN_ERROR = STATUS_START(ACM),
    E_ACM_INIT_FAIL,
    E_ACM_SHUTDOWN_ERROR,
    E_ACM_TIMER_SETUP_ERR,
    E_ACM_TIMER_CLOSE_ERR,
    E_ACM_INVALID_STATE,
    E_ACM_SERVER_NOT_FOUND,
    E_ACM_SEND_FAIL,
    E_ACM_INVALID_COMMAND,
    E_ACM_INVALID_OID_INCREMENT,
    E_ACM_OID_CONFIG_ERR,
    E_ACM_RECEIVE_ERR,
    E_ACM_HOME_ERR,
    E_ACM_MOVE_ERR,
    E_ACM_STOPPED,

    E_ACM_DEMAND_ERR,
    E_ACM_PROXIMITY_SWITCH_OPEN,
    E_ACM_ENCODER_NOT_HOMED,
    E_ACM_COLLIMATION_ERR

};

/*
** Package OID list.
*/

typedef enum eAcmDataId_e
{
   D_ACM_DATAID_BOL = OID_START(ACM),  /* Offset OIDs by package Id        */

   D_ACM_PROC_STATE,                   /* Task process state               */
   D_ACM_APP_VERSION,                  /* Software revision number         */
   D_ACM_SHUTDOWN,                     /* Task shutdown flag               */
   D_ACM_STATE,                        /* Application State                */
   D_ACM_ENABLE,                       /* Task enable flag                 */
   D_ACM_HOME,                         /* Home mechanisms flag             */
   D_ACM_HOME_LINEAR_ENCS,             /* Home the linear encoders         */
   D_ACM_STOP_DEMAND,                  /* Task stop flag                   */

   D_ACM_RED_HOME,                     /* Home the red actuator            */
   D_ACM_RED_STATE,                    /* State of red actuator            */
   D_ACM_RED_ACTUAL,                   /* Red actual position              */
   D_ACM_RED_DEMAND,                   /* Red position demand              */
   D_ACM_RED_HOME_ENC,                 /* Home the red linear encoder      */
   D_ACM_RED_ENC_HOMED,                /* Red linear encoder homed         */
   D_ACM_RED_LINEAR,                   /* Red linear encoder reading       */
   D_ACM_RED_PROXIMITY,                /* Red proximity switch status      */
   D_ACM_RED_COLLIMATED_POS,           /* Red collimated position          */
   D_ACM_RED_ENC_CALIBRATION,          /* Red encoder calibration param    */

   D_ACM_YEL_HOME,                     /* Home the yellow actuator         */
   D_ACM_YEL_STATE,                    /* State of yellow mechanism        */
   D_ACM_YEL_ACTUAL,                   /* Yellow actual position           */
   D_ACM_YEL_DEMAND,                   /* Yellow position demand           */
   D_ACM_YEL_HOME_ENC,                 /* Home the yellow linear encoder   */
   D_ACM_YEL_ENC_HOMED,                /* Yellow linear encoder homed      */
   D_ACM_YEL_LINEAR,                   /* Yellow linear encoder reading    */
   D_ACM_YEL_PROXIMITY,                /* Yellow proximity switch status   */
   D_ACM_YEL_COLLIMATED_POS,           /* Yellow collimated position       */
   D_ACM_YEL_ENC_CALIBRATION,          /* Yellow encoder calibration param */

   D_ACM_BLU_HOME,                     /* Home the blue actuator           */
   D_ACM_BLU_STATE,                    /* State of blue mechanism          */
   D_ACM_BLU_ACTUAL,                   /* Blue actual position             */
   D_ACM_BLU_DEMAND,                   /* Blue position demand             */
   D_ACM_BLU_HOME_ENC,                 /* Home the blue linear encoder     */
   D_ACM_BLU_ENC_HOMED,                /* Blue linear encoder homed        */
   D_ACM_BLU_LINEAR,                   /* Blue linear encoder reading      */
   D_ACM_BLU_PROXIMITY,                /* Blue proximity switch status     */
   D_ACM_BLU_COLLIMATED_POS,           /* Blue collimated position         */
   D_ACM_BLU_ENC_CALIBRATION,          /* Blue encoder calibration param   */

   D_ACM_ACM_RANGE_LO,                 /* Lower travel limit for actuators */
   D_ACM_ACM_RANGE_HI,                 /* Upper travel limit for actuators */
   D_ACM_ACM_REL_MOVING_TIME,          /* Maximum relative moving time, ms */
   D_ACM_ACM_ABS_MOVING_TIME,          /* Maximum absolute moving time, ms */
   D_ACM_STOPPED,                      /* Denoting all movement stopped    */
   D_ACM_ACM_AT_ZERO,                  /* Denoting current position ~ 0    */
   D_ACM_ACM_TOLERANCE,                /* Positional tolerance (nm)        */
   D_ACM_LINEAR_ENC_DELAY,             /* Delay to home linear encoder     */
   D_ACM_RELATIVE_RANGE_POS,           /* Relative move maximum increment  */
   D_ACM_RELATIVE_RANGE_NEG,           /* Relative move maximum increment  */
   D_ACM_SAVE_COLLIMATION,             /* Save the collimated positions    */
   D_ACM_RECOLLIMATE,                  /* Re-collimate using saved data    */

   D_ACM_RECOLLIMATE_DEFAULT,          /* Re-collimate using defaults      */

   D_ACM_RED_DFLT_COLLIMATED,          /* Red default collimated position  */
   D_ACM_YEL_DFLT_COLLIMATED,          /* Yellow dflt collimated position  */
   D_ACM_BLU_DFLT_COLLIMATED,          /* Blue default collimated position */

   D_ACM_HALT_DEMAND,                  /* Task halt flag                   */
  
   D_ACM_COLLIMATED_TOLERANCE,         /* Permitted collimation tolerance  */
   D_ACM_RED_COLLIMATE_DIFF,           /* Difference from collimated (red) */
   D_ACM_YEL_COLLIMATE_DIFF,           /* Difference from collimated (yel) */
   D_ACM_BLU_COLLIMATE_DIFF,           /* Difference from collimated (blu) */

   D_ACM_DATAID_EOL                    /* End-of-list marker               */
} eAcmDataId_t;

/*
** States - may be assumed by the process or each axis. When determining an 
** overall state from the component axis states, there is a precedence in the
** order below.
*/

typedef enum eAcmStates_e
{
   E_ACM_STATE_DISABLED = STATUS_START(ACM),
   E_ACM_STATE_READY,
   E_ACM_STATE_ENABLED,
   E_ACM_STATE_HOMING,
   E_ACM_STATE_MOVING_REL,
   E_ACM_STATE_MOVING_ABS,
   E_ACM_STATE_ERROR,
   E_ACM_STATE_OK,
   E_ACM_STATE_FAIL
} eAcmStates_t;

/*
** External function prototypes
*/

#endif

/*
** EOF
*/
