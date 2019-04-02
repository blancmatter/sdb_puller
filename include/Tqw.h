/*******************************************************************************
** Module Name:
**    Tqw.h
**
** Purpose:
**    Public header file for the library of TTl-QNX Wrapper functions (TQW).
**
** Description:
**    This file contains the definitions necessary to use the TQW library.
**
**    Note : This file replaces a the old files 'TTLNamedServices.h', 
**    'TTLTxRxServices.h' and 'TtlProcess.h'.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: Tqw.h,v 0.5 2004/04/22 09:06:11 mjf Exp $
**
** History:
**    $Log: Tqw.h,v $
**    Revision 0.5  2004/04/22 09:06:11  mjf
**    Removal of Inc.h, minor renaming.
**
**    Revision 0.4  2004/04/20 16:08:55  mjf
**    Removal of redundant INC.
**
**    Revision 0.3  2004/02/17 10:32:42  sxf
**    Port to Q6 using Wfl library.
**
**    Revision 0.2  2001/12/13 13:48:13  mjf
**    Addition of MODbus Server (MBS).
**
**    Revision 0.1  2001/09/19 15:42:24  mjf
**    Initial revision.
**
**
*******************************************************************************/


#ifndef TQW_H_DEFINED
#define TQW_H_DEFINED


/*
** Include relevant public header files
*/
#include "Wfl.h"

/* Compiler include files */

#include <limits.h>
#include <sys/types.h>

/* TTL system-wide definitions */

#include "TtlSystem.h"
#include "Asi.h"

/* Public header include files */

#include "Amc.h"
#include "Nmc.h"
#include "Nsc.h"
#include "Mif.h"
#include "Ven.h"
#include "Aes.h"
#include "Tfp.h"
#include "Asc.h"

#include "Agd.h"  
#include "Agf.h"
#include "Ams.h"  
#include "Sfd.h"
#include "Sfp.h"
#include "Smf.h"
#include "Acm.h"
#include "Mbs.h"

#include "Can.h"

#include "Pms.h"
#include "Sif.h"

#include "Cfl.h"


/*
** Macro Definitions.
*/

#define E_TTL_NO_NAMED_PROCES_PID      -1
#define E_TTL_MAX_PROC_NAME_LENGTH     32

#define PROCESS_NAME_TABLE_MAX_SIZE    25

#define MAIN_PROC_NAME                 "E_TTL_TtlProcess"

#define E_TTL_QNX_MSG_LENGTH           64   /* Std QNX Message size for TTL   */


/*
** Type Definitions.
*/

enum eTTLQNXStatus_e
{
   E_TTL_QNX_GEN_ERR = STATUS_START (TNF),  /* Miscellaneous error            */
   E_TTL_QNX_CLA_UNKNOWN,                   /* Unrecognised command line arg  */
   E_TTL_QNX_PROC_NAME_ATTACH_FAIL,         /* QNX proc name attach failure   */
   E_TTL_QNX_PROC_NAME_DETACH_FAIL,         /* QNX proc name detach failure   */
   E_TTL_QNX_PROC_NAME_LOCATE_FAIL,         /* QNX proc name locate failure   */
   E_TTL_QNX_PROC_NAME_TOO_LONG,            /* QNX process name too long      */

   E_TTL_QNX_EOSTATUS_LIST,  /* End of list marker (DO NOT USE FOR STATUS)    */
   E_TTL_QNX_STATUS_MAX_VALUE = INT_MAX     /* Req'd to force size to 4 bytes */
};

typedef Int32_t eTTLQNXStatus_t;

typedef struct {

   Uint32_t PackageId;                 /* TLA                                 */
   char     RegServerName [E_TTL_MAX_PROC_NAME_LENGTH ] ;          
   char     ExeName       [E_TTL_MAX_EXE_NAME_LENGTH  ] ;
   char     Pathname      [E_TTL_MAX_PATH_NAME_LENGTH ] ;
   Int32_t  Priority ;

} ProcessName_t;

typedef enum eNameServStatus_e
{
   E_TTL_QNX_SERVICE_OK        = STATUS_START (TMF),  /* Success              */
   E_TTL_QNX_INV_RECEIVE_SRC,         /* QNX Receive Source Invalid           */
   E_TTL_QNX_INV_REPLY_TYPE,          /* QNX Reply Type Invalid               */
   E_TTL_QNX_INV_REPLY_SUBTYPE,       /* QNX Reply Subtype Invalid            */
   E_TTL_QNX_REPLY_FAIL,              /* QNX Reply not succesful              */

   E_TTL_QNX_ENDSTATUS_LIST, /* End of list marker (DO NOT USE FOR STATUS)    */
   E_TTL_QNX_SERV_STATUS_MAX_VALUE = INT_MAX/* Req'd to force size to 4 bytes */
} eNameServStatus_t ;

typedef    struct
{
#ifdef E_WFL_OS_QNX6
   struct _pulse          hdr;         /* system message header */
#endif

#ifdef E_WFL_OS_QNX4
   struct   _sysmsg_hdr   hdr ;        /* system message header               */
#endif
                                       /* generic message payload             */
   Int32_t  GenMsgPayLoad [E_TTL_QNX_MSG_LENGTH + 32] ;
} iGenMsg_t ;


/*
** Variable Declarations.
*/

#ifdef M_TQW_PROCESS_MAIN
ProcessName_t  ProcessNameTable [PROCESS_NAME_TABLE_MAX_SIZE] = {
   { NSC, E_NSC_REG_SERVER_NAME, E_NSC_SERVER_PROC, E_NSC_SERVER_PATH, 0},                    /* Node State Controller     00 */
   { MIF, E_MIF_REG_SERVER_NAME, E_MIF_SERVER_PROC, E_MIF_SERVER_PATH, E_MIF_SERVER_PRI},     /* Motor Interface           01 */
   { VEN, E_VEN_REG_SERVER_NAME, E_VEN_SERVER_PROC, E_VEN_SERVER_PATH, 0},                    /* Virtual Encoder           02 */
   { AES, E_AES_REG_SERVER_NAME, E_AES_SERVER_PROC, E_AES_SERVER_PATH, E_AES_SERVER_PRI},     /* Absolute Encoder          03 */
   { NMC, E_NMC_REG_SERVER_NAME, E_NMC_SERVER_PROC, E_NMC_SERVER_PATH, 0},                    /* Node Motion Controller    04 */
   { TFP, E_TFP_REG_SERVER_NAME, E_TFP_SERVER_PROC, E_TFP_SERVER_PATH, E_TFP_SERVER_PRI},     /* Time & Frequency Process  05 */
   { AMC, E_AMC_REG_SERVER_NAME, E_AMC_SERVER_PROC, E_AMC_SERVER_PATH, 0},                    /* Axis Motion Controller    06 */
  
   { AGD, E_AGD_REG_SERVER_NAME, E_AGD_SERVER_PROC, E_AGD_SERVER_PATH, E_AGD_SERVER_PRI},     /* Autoguider Deployment     07 */
   { AGF, E_AGF_REG_SERVER_NAME, E_AGF_SERVER_PROC, E_AGF_SERVER_PATH, E_AGF_SERVER_PRI},     /* Autoguider Focus          08 */
   { SFD, E_SFD_REG_SERVER_NAME, E_SFD_SERVER_PROC, E_SFD_SERVER_PATH, E_SFD_SERVER_PRI},     /* Science Fold Deployment   09 */
   { SFP, E_SFP_REG_SERVER_NAME, E_SFP_SERVER_PROC, E_SFP_SERVER_PATH, E_SFP_SERVER_PRI},     /* Science Fold Positioning  10 */
   { SMF, E_SMF_REG_SERVER_NAME, E_SMF_SERVER_PROC, E_SMF_SERVER_PATH, E_SMF_SERVER_PRI},     /* Secondary Mirror Focus    11 */
   { SIF, E_SIF_REG_SERVER_NAME, E_SIF_SERVER_PROC, E_SIF_SERVER_PATH, E_SIF_SERVER_PRI},     /* Support Interface         12 */
   { CFL, E_CFL_REG_SERVER_NAME, E_CFL_SERVER_PROC, E_CFL_SERVER_PATH, E_CFL_SERVER_PRI},     /* Configureable Fast Logger 13 */
   { CAN, E_CAN_REG_SERVER_NAME, E_CAN_SERVER_PROC, E_CAN_SERVER_PATH, E_CAN_SERVER_PRI},     /* CAN BUS                   14 */
   { AMS, E_AMS_REG_SERVER_NAME, E_AMS_SERVER_PROC, E_AMS_SERVER_PATH, E_AMS_SERVER_PRI},     /* Aux Mech. State Control   15 */
   { ACM, E_ACM_REG_SERVER_NAME, E_ACM_SERVER_PROC, E_ACM_SERVER_PATH, E_ACM_SERVER_PRI},     /* ACADS                     16 */

   { ASC, E_ASC_REG_SERVER_NAME, E_ASC_SERVER_PROC, E_ASC_SERVER_PATH, E_ASC_SERVER_PRI},     /* Axis Stop Controller      17 */
   { PMS, E_PMS_REG_SERVER_NAME, E_PMS_SERVER_PROC, E_PMS_SERVER_PATH, E_PMS_SERVER_PRI},     /* Mirror Support State Ctrl 18 */
   { MBS, E_MBS_REG_SERVER_NAME, E_MBS_SERVER_PROC, E_MBS_SERVER_PATH, E_MBS_SERVER_PRI},     /* MODbus Server             19 */
   {   0,                     0,                 0,                 0, 0},                    /*                           20 */
   {   0,                     0,                 0,                 0, 0},                    /*                           21 */
   {   0,                     0,                 0,                 0, 0},                    /*                           22 */
   {   0,                     0,                 0,                 0, 0},                    /*                           23 */
   {   0,                     0,                 0,                 0, 0}                     /*                           24 */
} ;
#else
extern ProcessName_t ProcessNameTable [PROCESS_NAME_TABLE_MAX_SIZE] ;
#endif


/*
** Function Prototypes.
*/

extern Status_t   eTtlProcessSetName (Int32_t        Tla, 
                                      ProcessName_t  *pSetName) ;

extern Status_t   eTtlGetProcTableEntry (Int32_t        Tla, 
                                         ProcessName_t  *pProcTableEntry) ;

extern Status_t   eTtlGetProcName (Int32_t  Tla, 
                                   char     *ProcName) ;

extern Status_t   eTtlGetExeName (Int32_t  Tla, 
                                  char     *ExeName) ;

extern Status_t   eTtlGetProcPriority (Int32_t  Tla, 
                                       Int32_t  *ProcPriority) ;

#endif

