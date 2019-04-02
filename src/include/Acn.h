/*******************************************************************************
** Module Name:
**    Acn.h
**
** Purpose:
**    Public header file for general Axis Control Node development.
**
** Description:
**    This file contains definitions to all ACN software, for the Azimuth, 
**    Altitude, Cassegrain, Auxiliary Mechanisms and Mirror Support sub-systems.
**
**    Notes :-
**    1) This file replaces a long-since-redundant previous 'Acn.h' file which 
**       had an entirely different purpose. 
**    2) This file contains the contents of the old header file 'AcnTypedef.h'.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: Acn.h,v 0.34 2004/06/07 09:44:54 mkb Exp $
**
** History:
**    $Log: Acn.h,v $
**    Revision 0.34  2004/06/07 09:44:54  mkb
**    Remove definitions for task specific agents. Replace with
**    definition for generic agent.
**
**    Revision 0.33  2004/05/25 13:58:55  mkb
**    Added nasmyth CIL ids
**
**    Revision 0.32  2004/05/25 09:31:56  mkb
**    Add definitions for nasmyth plus and minus axes.
**
**    Revision 0.31  2004/05/19 08:32:47  mkb
**    Fix QNX6 specific path macro.
**
**    Revision 0.30  2004/04/27 12:40:12  mkb
**    Added generic agent definition.
**
**    Revision 0.29  2004/04/20 09:54:57  mkb
**    Set path for process names and queues to /dev fro QNX 6.
**
**    Revision 0.28  2004/04/16 13:12:58  mkb
**    Set name attach path to /dev for QNX 6
**
**    Revision 0.27  2004/04/14 16:22:01  mkb
**    Correct order of CommsIn and CommsOut in sub-system definitions.
**
**    Revision 0.26  2001/09/19 13:24:09  mjf
**    Addition of contents of old source file 'AcnTypedef.h'.
**
**    Revision 0.25  2001/09/17 13:54:23  mjf
**    Correction of typo.
**
**    Revision 0.24  2001/09/14 15:58:19  mjf
**    Filename re-used to hold general definitions for node sub-systems.
**
**
*******************************************************************************/


#ifndef ACN_H_DEFINED
#define ACN_H_DEFINED


/*
** Include relevant public header files
*/

/* TTL system-wide definitions */

#include "TtlSystem.h"

/* Communications Interface Library (CIL) */

#include "Cil.h"
#include "Wfl.h"


/*
** Macro Definitions.
*/

/* 2.5 mS tick-rate = 400 ticks per second */

#define  E_ACN_TICKS_PER_SECOND  400      

/* 2^32 - the size of a 4-byte word */

#define  E_ACN_LONG_WORD_SIZE    4294967296.0

/* 2^48 - the size of a 6-byte word */

#define  E_ACN_48_BIT_WORD_SIZE  281474976710656.0

/* Storage scale-factor for storage of motor positions */

#define  E_ACN_MOTOR_POSITION_STORAGE_SCALE  4096.0

/* Length of strings used for names */

#define E_ACN_NAME_LEN       32

/* Number of possible nodes */

#define E_ACN_NODE_COUNT     7

/* Definition of invalid node (valid nodes in enum eAcnNodes_e) */

#define E_ACN_INVALID_NODE   -1

/* Node names - for command-line arguments */

#define E_ACN_STR_AZIMUTH    "azimuth"
#define E_ACN_STR_ELEVATION  "altitude"
#define E_ACN_STR_CASSEGRAIN "cassegrain"
#define E_ACN_STR_AUX_MECH   "auxmech"
#define E_ACN_STR_MIRROR     "mirror"
#define E_ACN_STR_NASPLUS    "nplus"
#define E_ACN_STR_NASMINUS   "nminus"

/* Number of letters to match for uniqueness */

#define E_ACN_STR_NODE_MATCH 2


/*
** Type Definitions.
*/

/* Motor Torque in milliNewtonMetres */

typedef  Int32_t  eAcnTorque_t;         
     
/* Frequency in microHertz */

typedef  Uint32_t eAcnFrequency_t;           

/* Amplifier gain in milliAmps per Volt */

typedef  Int32_t  eAcnAmplifierGain_t;       

/* Motor gain in mNm/A */

typedef  Int32_t  eAcnMotorGain_t;           

/* Motor current in mA */

typedef  Int32_t  eAcnCurrent_t;             

/* Motor drive voltage in mV */

typedef  Int32_t  eAcnVoltage_t;             

/* Axis position in milliArcSecs used externally */

typedef  Int32_t  eAcnMilliArcSecs_t;        

/* Axis & motor position in milliArcSecs used internally */

typedef  double   eAcnControllerMAS_t;       

/* Preload percentage in milli-percent */

typedef  Uint32_t eAcnMilliPercent_t;        

/* Axis & motor velocity in milliArcSecs/mS */

typedef  double   eAcnArcSecPerSec_t;        

/* Axis acceleration in ArcSec/Sec/Sec */

typedef  double   eAcnArcSecPerSecPerSec_t;  

/* PID controller gain - no units */

typedef  double   eAcnControllerGain_t;      

/*
** Structure for defining identifiers and names for each element, where an 
** element is a member of a software sub-system. Note that in some cases the 
** CilId may not be applicable, so E_CIL_BOL should be used. The Name will be 
** looked up at runtime to determine the existence and identity of other 
** processes.
*/

typedef struct
{
   Int32_t  CilId;
   char     Name[ E_ACN_NAME_LEN ];
} eAcnElementSpec_t;

/*
** Structure for defining parameters for each sub-system. The NodeName is used 
** in the configuration file to determine that an entry is for this sub-system.
** The NodeController is specified in terms of the package ID, which is used to 
** determine specific OIDs to access using the generic OID offsets in 'Gsc.h'. 
** Each element which may comprise a sub-system is then specified.
*/

typedef struct
{
   char     NodeName[ E_ACN_NAME_LEN ];
   Int32_t  NodeController;
   eAcnElementSpec_t OidServer;
   eAcnElementSpec_t CommsIn;
   eAcnElementSpec_t CommsOut;
   eAcnElementSpec_t Agent;
   eAcnElementSpec_t RepServer;
   eAcnElementSpec_t RepQueue;
} eAcnSubSystemSpec_t;


/*
** Variable Declarations.
*/

/*
** The suggested method of using this data is to include "Acn.h" from an 
** application's private header, and then define the symbol E_ACN_MAIN in the
** main source file, before the inclusion of the private header.
*/

#ifdef E_ACN_MAIN

/*
** Force name attach definitions to be created in /dev for QNX 6
*/
#ifdef  E_WFL_OS_QNX6
#define E_ACN_PATH "/"
#else
#define E_ACN_PATH ""
#endif

eAcnSubSystemSpec_t eAcnSubSystem[ E_ACN_NODE_COUNT ] 
   = {
       { "AZN",
         NSC,
         { E_CIL_AZN, "AZN/OidServer" },
         { E_CIL_AZC, "AZN/CommsIn" },
         { E_CIL_AZS, "AZN/CommsOut" },
         { E_CIL_BOL, "AZN/Agent" },
         { E_CIL_AZR, "AZN/SdbReporter" },   
         { E_CIL_BOL, E_ACN_PATH"AZN_SdbReportQueue" } 
       },
       { "ELN",
         NSC,
         { E_CIL_ELN, "ELN/OidServer" },
         { E_CIL_ELC, "ELN/CommsIn" },
         { E_CIL_ELS, "ELN/CommsOut" },
         { E_CIL_BOL, "ELN/Agent" },
         { E_CIL_ELR, "ELN/SdbReporter" },   
         { E_CIL_BOL, E_ACN_PATH"ELN_SdbReportQueue" } 
       },
       { "CSN",
         NSC,
         { E_CIL_CSN, "CSN/OidServer" },
         { E_CIL_CSC, "CSN/CommsIn" },
         { E_CIL_CSS, "CSN/CommsOut" },
         { E_CIL_BOL, "CSN/Agent" },
         { E_CIL_CSR, "CSN/SdbReporter" },
         { E_CIL_BOL, E_ACN_PATH"CSN_SdbReportQueue" }
       },
       { "AMN",
         AMS,
         { E_CIL_OMN, "AMN/OidServer" },
         { E_CIL_OMC, "AMN/CommsIn" },
         { E_CIL_OMS, "AMN/CommsOut" },
         { E_CIL_BOL, "AMN/Agent" },
         { E_CIL_OMR, "AMN/SdbReporter" },   
         { E_CIL_BOL, E_ACN_PATH"AMN_SdbReportQueue" } 
       },
       { "PMN",
         PMS,
         { E_CIL_MSN, "PMN/OidServer" },
         { E_CIL_MSC, "PMN/CommsIn" },
         { E_CIL_MSS, "PMN/CommsOut" },
         { E_CIL_BOL, "PMN/Agent" },
         { E_CIL_MSR, "PMN/SdbReporter" },   
         { E_CIL_BOL, E_ACN_PATH"PMN_SdbReportQueue" } 
       },
       { "NPN",
         NSC,
         { E_CIL_NDN, "NPN/OidServer" },
         { E_CIL_NDC, "NPN/CommsIn" },
         { E_CIL_NDS, "NPN/CommsOut" },
         { E_CIL_BOL, "NPN/Agent" },
         { E_CIL_NDR, "NPN/SdbReporter" },   
         { E_CIL_BOL, E_ACN_PATH"NPN_SdbReportQueue" } 
       },
       { "NMN",
         NSC,
         { E_CIL_NWN, "NMN/OidServer" },
         { E_CIL_NWC, "NMN/CommsIn" },
         { E_CIL_NWS, "NMN/CommsOut" },
         { E_CIL_BOL, "NMN/Agent" },
         { E_CIL_NWR, "NMN/SdbReporter" },   
         { E_CIL_BOL, E_ACN_PATH"NMN_SdbReportQueue" } 
       }
     };

#else

extern eAcnSubSystemSpec_t eAcnSubSystem[ E_ACN_NODE_COUNT ];

#endif


#endif

/*
** EOF
*/



