/******************************************************************************* 
**
**  Module Name : 
**     Smf.h
**
**  Purpose :
**     Public header file for package.
**
**  Description :
**     Used to declare exported data, functions and definitions for the package.
**
**  Authors :
**     Pete Hall    (pxh)
**     Martyn Ford  (mkf)
**     Mark Bowman  (mkb)
**
**  Version :
**     $Id: Smf.h,v 0.28 2007/09/07 15:39:00 mjf Exp $
**
**  History: 
**     $Log: Smf.h,v $
**     Revision 0.28  2007/09/07 15:39:00  mjf
**     Re-format comment for tidier IdTable generation - no functional changes.
**
**     Revision 0.27  2006/07/12 14:26:13  mjf
**     SMF_1_07.
**
**     Revision 0.26  2005/08/18 10:15:31  mjf
**     SMF_1_05.
**
**     Revision 0.25  2005/06/17 12:47:29  mjf
**     Addition of functionality to move to and detect whether at :-
**     - nominal focus position
**     - position at which to start homing linear encoders
**     - position at which to stop homing linear encoders
**
**     Revision 0.24  2004/09/01 15:48:21  mkb
**     Allow larger range of coefficients for altitude focus tracking.
**
**     Revision 0.23  2004/03/31 14:58:45  mkb
**     Increase range of allowed focus correction for altitude
**     from +-100 microns to +-300 microns.
**
**     Revision 0.22  2003/10/03 13:07:22  man
**     Included functionality to check the MicroLynx error
**     status and set the process status to warning if an
**     error is detected.
**
**     Revision 0.21  2002/10/15 10:08:23  mkb
**     Correct units for focus correction coefficients.
**
**     Revision 0.20  2002/10/15 08:58:22  mkb
**     Debug previous modification.
**
**     Revision 0.19  2002/10/14 16:20:55  mkb
**     Add definitions of range of allowed coefficients for focus
**     correction.
**
**     Revision 0.18  2002/10/11 15:41:00  mkb
**     Added focus compensation for tube flexure.
**     Modified ProcState model to give warning on focus correction
**     failure and Failed on mechanism failure.
**     Modified simulator to support STOP and focus correction.
**
**     Revision 0.17  2002/07/31 09:49:14  mjf
**     Change of name of limit switch datum entries - now unused.
**
**     Revision 0.16  2001/11/28 14:39:20  mjf
**     Re-introduction of 'halt demand' OID.
**
**     Revision 0.15  2001/10/29 11:47:25  mkb
**     Added definitions required for temperature compensation of
**     focus position.
**
**     Revision 0.14  2001/10/12 13:56:10  mjf
**     Removal of TABs from the header.
**
**     Revision 0.13  2001/10/10 08:16:56  mjf
**     Change to state model and OID table, such that an 'enable' simply enables
**     subsequent homing, but no longer homes. A new 'home' OID has been added 
**     that allows the mechanism to be homed after the task has been enabled. 
**     Hence, an 'enabled' state has been added between 'disabled' and 'ready'. 
**     The name of the 'HALT_DEMAND' has been alter to be 'STOP_DEMAND', and the
**     name of the 'STOPPED' OID has been altered to remove the package prefix.
**
**     Revision 0.12  2001/08/21 12:57:13  mjf
**     Implementation of the HALT_DEMAND flag as a latch that must be cleared.
**
**     Revision 0.11  2001/08/16 11:27:29  mjf
**     Addition of OID to denote if the current position is at (or close to)
**     zero.
**
**     Revision 0.10  2001/08/14 15:54:19  mjf
**     Addition of flag denoting when all movement is stopped.
**
**     Revision 0.9  2001/08/14 09:56:49  mjf
**     Enum name added to states.
**
**     Revision 0.8  2001/08/14 08:51:27  mjf
**     Implementation of maximum moving time in the configuration file, and
**     alteration to the debug levels.
**
**     Revision 0.7  2001/08/10 10:05:34  mjf
**     Addition of OID for maximum moving time.
**
**     Revision 0.6  2001/04/27 10:54:31  pxh
**     Added D_SMF_SMF_RANGE_LO, D_SMF_SMF_RANGE_HI & D_SMF_SMF_HOME_TOLERANCE
**     to OID list.
**
**     Revision 0.5  2001/03/07 10:32:08  pxh
**     Added D_SMF_APP_VERSION OID.
**     Task states are now offset by package ID.
**
**     Revision 0.4  2001/02/07 17:09:14  pxh
**     Corrected server path to include executable name.
**
**     Revision 0.3  2001/02/01 09:55:37  pxh
**     Corrected error in server naming convention.
**
**     Revision 0.2  2001/01/31 16:08:14  pxh
**     Changed server name and path definitions for compatibility with
**     OidServer.
**
**     Revision 0.1  2001/01/08 11:32:11  pxh
**     Initial revision - tested with CanServer, untested with hardware.
**
******************************************************************************/


#ifndef _SMF_H_DEFINED
#define _SMF_H_DEFINED


/* 
** Code for coping with decl'n/def'n/init'n of global variables.
*/

#ifdef M_SMF_MAIN
   #define E_SMF_EXTERN
   #define E_SMF_INIT(Initialisation) = Initialisation
#else
   #define E_SMF_EXTERN extern
   #define E_SMF_INIT(Initaliasation)
#endif


/* 
** Required include files.
*/

#include "TtlSystem.h"       /* For Status_t definition                       */


#define E_SMF_REG_SERVER_NAME    "E_TTL_AMN_SMF"
#define E_SMF_SERVER_PROC        "Smf"
#define E_SMF_SERVER_PATH        "//1/opt/ttl/bin/Smf"
#define E_SMF_SERVER_PRI         10

typedef struct
{
   Int32_t Value;    /* Actual value of the global task data   */
   Int32_t Units;    /* The units of that value (for decoding) */
} eSmfTaskData_t;

enum eSmfStatus
{
    E_SMF_GEN_ERROR = STATUS_START(SMF),
    E_SMF_INIT_FAIL,
    E_SMF_SHUTDOWN_ERROR,
    E_SMF_TIMER_SETUP_ERR,
    E_SMF_TIMER_CLOSE_ERR,
    E_SMF_INVALID_STATE,
    E_SMF_SERVER_NOT_FOUND,
    E_SMF_SEND_FAIL,
    E_SMF_INVALID_COMMAND,
    E_SMF_OID_CONFIG_ERR,
    E_SMF_RECEIVE_ERR,
    E_SMF_HOME_ERR,
    E_SMF_MOVE_ERR,
    E_SMF_STOPPED,
    E_SMF_TEMP_COMP_ERR,              /* Failed to compensate focus for */
                                      /* change in truss temperature.   */
    E_SMF_ALT_COMP_ERR                /* Failed to compensate focus for */
                                      /* change in telescope altitude.  */
};


/*
** Coeeficient limits.
*/

#define E_SMF_TEMP_COMP_COEF_MIN  -1000000.0  /* nm per celsius. */     
#define E_SMF_TEMP_COMP_COEF_MAX   1000000.0  /* nm per celsius. */
#define E_SMF_ALT_COMP_COEF_MIN   -1000000.0  /* nm per degree zenith angle.  */
#define E_SMF_ALT_COMP_COEF_MAX    1000000.0  /* nm per degree zeenith angle. */


/*
** Package OID list.
*/

typedef enum eSmfDataId_e
{
   D_SMF_DATAID_BOL = OID_START(SMF),  /* Offset OIDs by package Id.       */

   D_SMF_PROC_STATE,                   /* Task process state               */
   D_SMF_APP_VERSION,                  /* Application version no.          */
   D_SMF_SHUTDOWN,                     /* Task shutdown flag               */
   D_SMF_STATE,                        /* Secondary mirror State           */
   D_SMF_ENABLE,                       /* Task enable flag                 */
   D_SMF_HOME,                         /* Home the main mechanism          */
   D_SMF_DEMAND,                       /* (Virtual) focus position demand  */
   D_SMF_ACTUAL,                       /* (Virtual) focus position         */

   D_SMF_UNUSED_LIMIT_LO,              /* Low limit switch state           */
   D_SMF_UNUSED_LIMIT_HI,              /* High limit switch state          */
   D_SMF_STOP_DEMAND,                  /* Task stop flag                   */
   D_SMF_SMF_RANGE_LO,                 /* Lower travel limit for axis      */
   D_SMF_SMF_RANGE_HI,                 /* Upper travel limit for axis      */
   D_SMF_SMF_HOME_TOLERANCE,           /* Used in homing sequence          */
   D_SMF_SMF_MAX_MOVING_TIME,          /* Maximum moving time (msec)       */
   D_SMF_STOPPED,                      /* Denoting all movement stopped    */
   D_SMF_SMF_AT_ZERO,                  /* Denoting current position ~ 0    */

   D_SMF_TEMP_COMP_ENABLE,             /* Temperature compensation enabled */
   D_SMF_TEMP_COMP_COEF,               /* Temperature compensation coeff.  */
   D_SMF_TEMP_COMP_ZEROPT,             /* Temperature of zero correction   */
   D_SMF_TEMP_COMP_OFFSET,             /* Focus offset due to temperature  */
   D_SMF_TEMP_COMP_THRESHOLD,          /* Min. temp. correction threshold  */
   D_SMF_PHYSICAL_POSN,                /* Physical position of focus       */
   D_SMF_HALT_DEMAND,                  /* Task halt flag                   */
   D_SMF_ALT_COMP_ENABLE,              /* Altitude compensation enabled    */
   D_SMF_ALT_COMP_COEF,                /* Altitude compensation coeff.     */ 
   D_SMF_ALT_COMP_ZEROPT,              /* Elevation of zero correction     */
   D_SMF_ALT_COMP_OFFSET,              /* Focus offset due to altitude     */
   D_SMF_ALT_COMP_THRESHOLD,           /* Alt. temp. correction threshold  */
   D_SMF_TEMP_COMP_OPERATIONAL,        /* Temperature compensation is OK   */
   D_SMF_ALT_COMP_OPERATIONAL,         /* Altitude compensation is OK      */

   D_SMF_SMF_ERROR,                    /* MicroLynx error status           */

   D_SMF_SMF_NOMINAL_FOCUS,            /* Nominal focus position           */
   D_SMF_SMF_LIN_ENC_START_POS,        /* Start pos of lin enc ref mark    */
   D_SMF_SMF_LIN_ENC_END_POS,          /* End position of lin enc ref mark */
   D_SMF_SMF_AT_NOMINAL_FOCUS,         /* TRUE if at nominal focus pos     */
   D_SMF_SMF_AT_LIN_ENC_START,         /* TRUE if at lin enc start pos     */
   D_SMF_SMF_AT_LIN_ENC_END,           /* TRUE if at lin enc end pos       */
   D_SMF_SMF_MOVE_NOMINAL,             /* To move to nominal focus         */
   D_SMF_SMF_MOVE_LIN_ENC_START,       /* To move to linear encoder start  */
   D_SMF_SMF_MOVE_LIN_ENC_END,         /* To move to linear encoder end    */

   D_SMF_SMF_POSITION_OFFSET,          /* Offset to apply to read pos      */
   D_SMF_SMF_POSITION_SCALAR,          /* Factor to apply to offset pos    */
   D_SMF_SMF_ULYNX_DEMAND,             /* Demand pos sent to microlynx     */
   D_SMF_SMF_ULYNX_ACTUAL,             /* Actual pos read from microlynx   */

   D_SMF_SMF_POS_TOLERANCE,            /* Tolerance if demand != actual    */

   D_SMF_SMF_ULYNX_ERROR,              /* MicroLynx error code             */

   D_SMF_SMF_AT_LO,                    /* At lower limit                   */
   D_SMF_SMF_AT_HI,                    /* At upper limit                   */
   D_SMF_SMF_MOVE_LO,                  /* Move to lower limit              */
   D_SMF_SMF_MOVE_HI,                  /* Move to upper limit              */

   D_SMF_DATAID_EOL                    /* End-of-list marker.              */
} eSmfDataId_t;


typedef enum eSmfStates_e
{
   E_SMF_STATE_DISABLED = STATUS_START(SMF),
   E_SMF_STATE_ENABLED,
   E_SMF_STATE_HOMING,
   E_SMF_STATE_READY,
   E_SMF_STATE_MOVING,
   E_SMF_STATE_ERROR,
   E_SMF_STATE_OK,
   E_SMF_STATE_FAIL
} eSmfStates_t;


#endif

/*******************************************************************************
** End of File: Smf.h
*******************************************************************************/
