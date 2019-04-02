/* 
**
**  Module Name : 
**    	Ams.h
**
**  Purpose :
**		Public header file for package.
**
**  Description :
**		Used to declare exported data, functions and definitions for the
**		package.
**
**  Authors :
**		PXH : Pete Hall (TTL)
**
**  Version :
**      $Id: Ams.h,v 0.10 2004/04/23 16:04:14 mkb Exp $
**
**  History: 
**    	$Log: Ams.h,v $
**    	Revision 0.10  2004/04/23 16:04:14  mkb
**    	First working QNX 6 version.
**
**    	Revision 0.9  2003/05/06 14:00:09  mkb
**    	 Report state for each mechanism end client to SDB.
**
**    	Revision 0.8  2001/11/28 14:34:47  mjf
**    	Re-introduction of 'halt demand' OID.
**
**    	Revision 0.7  2001/10/17 15:33:21  mjf
**    	Change of OIDs to match the NSC, and implementation of enable,
**    	disable, home and stop to all end-clients in the sub-system.
**
**    	Revision 0.6  2001/09/19 16:12:00  mjf
**    	Updates as a result of source file name changes. No functional changes.
**
**    	Revision 0.5  2001/08/16 11:20:22  mjf
**    	Implementation of axis stopped OID, which is checked upon a safe-state
**    	request. A safe-state request halts mechanisms rather than disables
**    	them, and an activate request no longer automatically enables them.
**    	Shutdown mechanism improved, and log messages improved.
**
**    	Revision 0.4  2001/03/26 17:05:08  mjf
**    	Use generic OID offsets from AcnGsc.h, and use generic format for the
**    	configuration file.
**
**    	Revision 0.3  2001/03/23 15:42:59  pxh
**    	Commented all OID definitions.
**
**    	Revision 0.2  2001/03/08 14:12:02  pxh
**    	Removed temporary definition of AMS Package Id.
**
**    	Revision 0.1  2001/03/01 12:12:44  pxh
**    	Initial revision.
**
**
*/


#ifndef _AMS_H_DEFINED
#define _AMS_H_DEFINED

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_AMS_MAIN
   #define E_AMS_EXTERN
   #define E_AMS_INIT(Initialisation) = Initialisation
#else
   #define E_AMS_EXTERN extern
   #define E_AMS_INIT(Initaliasation)
#endif

/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition                       */
#include "Gsc.h"             /* Generic (node) State Controller OID offsets   */


#define E_AMS_REG_SERVER_NAME    "E_TTL_AMN_AMS"
#define E_AMS_SERVER_PROC        "Ams"
#define E_AMS_SERVER_PRI         10
#define E_AMS_SERVER_PATH        "" 


enum eAmsStatus
{
    E_AMS_GEN_ERROR = STATUS_START(AMS),
    E_AMS_INIT_FAIL,
    E_AMS_SHUTDOWN_ERROR,
    E_AMS_TIMER_SETUP_ERR,
    E_AMS_TIMER_CLOSE_ERR,
    E_AMS_INVALID_STATE,
    E_AMS_SERVER_NOT_FOUND,
    E_AMS_SEND_FAIL,
    E_AMS_INVALID_COMMAND,
    E_AMS_ENDCLIENT_SPAWN_FAIL,
    E_AMS_ENDCLIENT_NOT_FOUND,
    E_AMS_RECEIVE_ERR
};

/*
** Package OID list.
*/

typedef enum eAmsDataId_e
{
   D_AMS_DATAID_BOL                    /* Offset OIDs by generic AMS Id. */
    = OID_START(AMS) + D_GSC_DATAID_BOL,  

   D_AMS_PROC_STATE                    /* Overall process state          */
    = OID_START(AMS) + D_GSC_PROC_STATE,  
   D_AMS_APP_VERSION                   /* Software version number        */
    = OID_START(AMS) + D_GSC_APP_VERSION,  
   D_AMS_STATE                         /* Task state                     */
    = OID_START(AMS) + D_GSC_TOP_LEVEL_STATE,  
   D_AMS_SHUTDOWN                      /* Task shutdown flag             */
    = OID_START(AMS) + D_GSC_SHUTDOWN,  
   D_AMS_ENABLE                        /* Enable flag                    */
    = OID_START(AMS) + D_GSC_ENABLE,  
   D_AMS_HOME                          /* Home flag                      */
    = OID_START(AMS) + D_GSC_HOME,           
   D_AMS_STOP_DEMAND                   /* Stop demand                    */
    = OID_START(AMS) + D_GSC_STOP_DEMAND, 
   D_AMS_SAFE_STATE                    /* Safe-state command             */
    = OID_START(AMS) + D_GSC_SAFE_STATE_COMMAND,  
   D_AMS_ACTIVATE                      /* Activate command               */
    = OID_START(AMS) + D_GSC_ACTIVATE_COMMAND,  
   D_AMS_STOPPED_STATUS                /* Stopped status                 */
    = OID_START(AMS) + D_GSC_AXIS_STOPPED, 
   D_AMS_HALT_DEMAND                   /* Halt demand                    */
    = OID_START(AMS) + D_GSC_HALT_DEMAND, 

   D_AMS_ENDCLIENT_STATE_CAN,          /* CAN end-client process state   */
   D_AMS_ENDCLIENT_STATE_AGD,          /* AGD end-client process state   */
   D_AMS_ENDCLIENT_STATE_AGF,          /* AGF end-client process state   */
   D_AMS_ENDCLIENT_STATE_SFD,          /* SFD end-client process state   */
   D_AMS_ENDCLIENT_STATE_SFP,          /* SFP end-client process state   */
   D_AMS_ENDCLIENT_STATE_SMF,          /* SMF end-client process state   */
   D_AMS_ENDCLIENT_STATE_MBS,          /* MBS end-client process state   */

   D_AMS_DATAID_EOL                    /* End-of-list marker             */
} eAmsDataId_t;


/*
** External function prototypes
*/

#endif

/*
** EOF
*/
