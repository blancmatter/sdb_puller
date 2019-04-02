/*
**
** Module Name :
**    Agt.h
**
** Purpose:
**
** Description:
**
** Imported Definitions:
**
** Exported Definitions:
**
** Private Definitions:
**
** Authors:
**    rdl:     R. D. Lett (TTL).
**
** Version:
**    $Id: Agt.h,v 0.10 2004/01/21 12:09:45 sxf Exp $
**
** History:
**    $Log: Agt.h,v $
**    Revision 0.10  2004/01/21 12:09:45  sxf
**    No requirement for setting enum to INT_MAX to force to type int.
**    This is now done for QNX4 via a compiler option.
**
**    Revision 0.9  2001/09/24 13:05:49  mjf
**    Change of line endings to be LF-only. No other changes made.
**
**    Revision 0.8  2001/09/19 13:41:45  mjf
**    Updates as a result of source file name changes. No functional changes.
**
**    Revision 0.7  2000/11/01 12:46:15  rdl
**    Change to constant used to force enumerated list types to occupy four (4) bytes.
**
**    Revision 0.6  2000/10/10 08:33:50  rdl
**    Rationalisation of header files including :
**    a). Conformance to TTL Coding Standard.
**    b). Additional function prototypes for TtlProcess.c
**    c). Additional error and status enumerations.
**
**    Revision 0.5  2000/10/09 13:34:50  rdl
**    Consolidation of definitions.
**
**    Revision 0.4  2000/10/06 16:02:26  rdl
**    Additional error enumerations.
**
**    Revision 0.3  2000/09/28 09:00:28  rdl
**    Consolidation before major changes.
**
**    Revision 0.2  2000/09/28 08:12:32  rdl
**    Minor Modifications.
**
**
** Copyright (C) TTL - 2000.
**
*/

#ifndef __AGENT_H_DEFINED
#define __AGENT_H_DEFINED                        1

/* Required include files */

#include "TtlSystem.h"

/* 
** This header file is included by the SERVER to maintain the 
** agent availability table.
*/

/* Format of an agent availability table element */

typedef struct
{
   Int32_t  nService ;                 /* Service Provided by the Agent       */
   Int32_t  nStatus ;                  /* Status of the Agent                 */
   pid_t    nAgentPid ;                /* Process id of the agent process     */
   pid_t    nClientPid ;               /* Process id of the client process    */
   pid_t    nResponsePid ;             /* Process id of the response process  */
} AgentEntry_t ;

/* Agent Status values */

typedef enum eAgentStatus_e
{
   E_AGT_AGENT_OK = STATUS_START (AGT),
   E_AGT_AGENT_AVAILABLE,              /* Agent is avaliable for allocation   */
   E_AGT_AGENT_NOT_AVAILABLE,          /* Specified Agent not avaliable       */
   E_AGT_AGENT_WORKING,                /* Specified Agent not working         */
   E_AGT_AGENT_NOT_FOUND,              /* Specified Agent not found           */
   E_AGT_AGENT_NO_SPACE,               /* No space for new Agent Entry        */
   E_AGT_AGENT_ALREADY_PRESENT,        /* Agent Entry already present         */
   E_AGT_AGENT_ALREADY_CREATED,        /* Agent Entry already created         */
   E_AGT_AGENT_MALLOC_FAIL,            /* Memory allocation failed            */
   E_AGT_AGENT_TABLE_NOT_PRESENT,      /* Agent Entry Availablity Table Not Present     */
   E_AGT_AGENT_TABLE_NOT_ERASED,       /* Agent Entry Availablity Table Removal Failure */
   E_AGT_AGENT_DEAD_NOT_FOUND,         /* Signalled Dead Agent not found      */
   
   /* Error codes associated with the spawning of agents by a server          */
   E_AGT_AGENT_NO_AGENTS_SPAWNED,      /* No Agents Spawned                   */
   E_AGT_AGENT_NUM_SPAWN_ERROR,        /* Incorrect number of agents spawned  */
   E_AGT_AGENT_SPAWN_FAILURE,          /* More Agents spawned than specified  */
   E_AGT_PROCESS_NOT_EXECUTING,        /* Specified process not executing     */
      
   E_AGT_EOSTATUS_LIST,      /* End of list marker (DO NOT USE FOR STATUS)    */
   E_AGT_AGENT_STATUS_MAX_VALUE
} eAgentStatus_t ;

/* Function prototypes */

eAgentStatus_t eAGT_CreateAgentAvailabilityTable (Int32_t nMaxNumAgents) ;
eAgentStatus_t eAGT_DeleteAgentAvailabilityTable (void) ;
eAgentStatus_t eAGT_AddAgentToTable (pid_t AgentPid, Int32_t nService) ;
eAgentStatus_t eAGT_RemoveAgentFromTable (pid_t pid) ;
static void    eAGT_RemoveElementFromTable (AgentEntry_t * pTable) ;
eAgentStatus_t eAGT_FindAndRemoveDeadAgent (Int32_t    *NumRemovedPtr) ;
Bool_t         eAGT_IsAgent (pid_t ProcessPid) ;
eAgentStatus_t eAGT_AcquireFreeAgent (pid_t    ClientPid, 
                                      Int32_t  nService, 
                                      pid_t  *AgentPid) ;
eAgentStatus_t eAGT_ClientPid (pid_t AgentPid, pid_t  *ClientPid) ;
eAgentStatus_t eAGT_ReleaseAgent (pid_t AgentPid) ;
eAgentStatus_t eAGT_ConfirmProcessAlive (pid_t    ProcPid) ;


#endif                                 /* __AGENT_H_DEFINED                   */

/*
** =============================================================================
** End of File : Agent.h.
** =============================================================================
*/
