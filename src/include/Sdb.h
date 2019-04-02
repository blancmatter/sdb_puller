/*
**       Copyright (c) Telescope Technologies Limited, 2000
**
** Header File Name:
**    Sdb.h
**
** Purpose:
**    Public header file for the Status Database (SDB)
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay
**
** Version:
**    $Id: Sdb.h,v 0.79 2007/07/17 10:20:02 mjf Exp $
**
** History:
**    $Log: Sdb.h,v $
**    Revision 0.79  2007/07/17 10:20:02  mjf
**    Addition of new units.
**
**    Revision 0.78  2005/05/13 12:51:07  mjf
**    Consistent with TQB_0_44.
**
**    Revision 0.77  2004/05/06 08:50:30  mjf
**    Version 1.01 - addition of SPLC units for T2-408 Services PLC.
**
**    Revision 0.76  2003/08/08 09:50:02  mjf
**    SDB v0.59 - addition of PLC System Status units.
**
**    Revision 0.75  2003/06/05 12:47:23  dws
**    Changed eSdbCode_t from signed to unsigned Int 32. This was to solve
**    problem of too many cilids thus exceeding the size allocated for the code.
**
**    Revision 0.74  2003/05/23 13:30:59  mjf
**    Addition of unit type to pack major/minor version in 16 bits.
**
**    Revision 0.73  2003/02/14 09:53:24  sxf
**    Added E_SDB_BAUD_UNITS for baud rate.
**
**    Revision 0.72  2002/11/15 17:24:37  mjf
**    Addition of autoguider state, in accordance with AGS ICD v0.02.
**
**    Revision 0.71  2002/10/04 12:52:37  sxf
**    Added correction to units list.
**
**    Revision 0.70  2002/10/04 12:50:33  sxf
**    Added E_SDB_MW_PERMPERM_UNITS
**
**    Revision 0.69  2002/09/20 14:33:00  man
**    Included a new unit for use in the inclinometer
**    calibration (milli-degrees per volt).
**
**    Revision 0.68  2002/08/05 15:24:00  mjf
**    Addition of units for PLC device types - note that these must exist
**    in a continguous list in the larger units list. Also addition of
**    humidity unit.
**
**    Revision 0.67  2002/07/27 11:02:55  mjf
**    Addition of two command-line parameters, to specify the path of the
**    datafile storage, and to specify the number of days after which to
**    clean up SDB files.
**
**    Revision 0.66  2002/04/05 15:38:36  djm
**    Added new unit "E_SDB_UAS_UNITS" (microarcseconds).
**
**    Revision 0.65  2002/03/25 14:03:06  sxf
**    Correct E_SDB_MSTARMAG_UNITS.
**
**    Revision 0.64  2002/03/25 13:53:58  sxf
**    Added additional units for AGS.
**
**    Revision 0.63  2002/01/09 09:18:29  mjf
**    Addition of units for telescopes, litres, millilitres and ml/min.
**
**    Revision 0.62  2001/12/17 17:30:07  mjf
**    Addition of package ID and IET data units.
**
**    Revision 0.61  2001/12/13 15:10:19  mjf
**    Addition of units and EPT and SPT data states.
**
**    Revision 0.60  2001/11/06 16:38:48  djm
**    Added new units (E_SDB_NM_PER_CEL_UNITS and
**    E_SDB_SSE_STRING_UNITS).
**
**    Revision 0.59  2001/10/23 09:35:04  mjf
**    Addition of RETRIEVE_L service, to retrieve the last 'n' measurements
**    from disk - like RETRIEVE_F but for the last (not first) measurements.
**
**    Revision 0.58  2001/10/12 11:24:58  mjf
**    Addition of a 'CLEAR_1' service, which will allow the originator of the
**    message to selectively clear their own datum entries. The message takes
**    the same form as the 'CLEAR_S' service, but the data to clear are
**    specified rather than the data to be perserved.
**
**    Revision 0.57  2001/09/14 11:08:01  djm
**    Added new error code.
**
**    Revision 0.56  2001/04/24 11:20:59  mjf
**    Addition of new unit E_SDB_UNSUPPORTED_UNITS to be used where a given
**    datum is not used in a particular software variant.
**
**    Revision 0.55  2001/04/03 11:28:28  mkv
**    Changed inclinometer units to milli degrees.
**
**    Revision 0.54  2001/03/23 15:29:53  mjf
**    Addition of arc-second units.
**
**    Revision 0.53  2001/03/12 10:31:26  mkv
**    Tenths of a degree units, for Inclinometer output.
**
**    Revision 0.52  2001/02/20 14:26:59  mjf
**    Addition of hours units.
**
**    Revision 0.51  2001/02/07 12:02:50  mjf
**    Force the data and unit files to be located on QNX node 1 '//1/...'.
**
**    Revision 0.50  2001/02/07 11:38:40  mjf
**    Change of path for database files from //1/tmp to /opt/ttl/data. Note
**    that this directory must exist.
**
**    Revision 0.49  2001/01/04 17:18:05  mjf
**    Addition of distance units, marker for end of 32-bit units, and
**    64-bit units now start at 0x10000.
**
**    Revision 0.48  2000/12/21 15:42:52  mjf
**    Addition of enumeration name qualifiers to enum typedefs.
**
**    Revision 0.47  2000/12/09 15:02:51  mjf
**    Addition of units for services PLC and milli-version.
**
**    Revision 0.46  2000/12/09 14:58:07  mjf
**    Addition of SUBMIT_1P service - post a submission expecting no reply.
**
**    Revision 0.45  2000/11/29 10:20:44  mkv
**    Added Sdb Units E_SDB_OID_UNITS & E_SDB_INDEX_UNITS.
**
**    Revision 0.44  2000/11/07 13:20:56  mjf
**    Addition of units for MCP system requests and WMS units.
**
**    Revision 0.43  2000/11/03 16:04:25  djm
**    Added the D_MCP_FISRT_USER_DATUM offset.
**
**    Revision 0.42  2000/11/01 13:44:41  rdl
**    Change of constant used to force enumerated type list to
**    occupy four (4) bytes.
**
**    Revision 0.41  2000/11/01 11:05:30  rdl
**    Removal of tabs entered by vedit.
**
**    Revision 0.40  2000/11/01 10:49:32  rdl
**    Additional entry to all enumerated type lists to force last entry to
**    be equal to maximum integer size (4 bytes).
**
**    Revision 0.39  2000/10/16 14:04:53  djm
**    Added two new errors for additional check on file writing.
**
**    Revision 0.38  2000/10/10 14:50:29  mjf
**    Updated for new generic MCP datum IDs.
**
**    Revision 0.37  2000/10/10 09:08:37  mjf
**    Addition of units for bytes, kilobytes, megabytes and revolutions
**    per minute (RPM).
**
**    Revision 0.36  2000/10/10 08:22:11  mjf
**    Addition of units for a CIL identifier, and requested/granted
**    authorisation state.
**
**    Revision 0.35  2000/10/10 08:00:38  mjf
**    Addition of units for shutter state, for the EPT.
**
**    Revision 0.34  2000/10/10 07:52:35  mjf
**    Addition of SDB units : milliseconds per year and day, and also
**    milliarcseconds per year and day.
**
**    Revision 0.33  2000/10/10 07:43:35  mjf
**    Addition of minutes unit.
**
**    Revision 0.32  2000/10/04 10:58:47  djm
**    Changed the size of the storage ID code that is used in SDB data files.
**    Also specified an absolute path for writing SDB data files.
**
**    Revision 0.31  2000/09/21 11:11:13  djm
**    Added two new error codes and "units" filename.
**
**    Revision 0.30  2000/09/19 07:33:19  mjf
**    Addition of units.
**
**    Revision 0.29  2000/09/08 14:44:20  asp
**    Removed double-quotes from within eSdbDataId_e enum comments as they
**    break the automatic ID table generator (hdr2idtable)
**
**    Revision 0.28  2000/09/08 14:33:48  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.27  2000/09/07 15:59:31  mjf
**    Addition of milli-celsius and milli-percentage units.
**
**    Revision 0.26  2000/09/06 14:14:53  djm
**    Added enumerations for new "exported" data.
**
**    Revision 0.25  2000/09/01 11:42:11  djm
**    Added a newline character for strict ANSI compliance.
**
**    Revision 0.24  2000/08/31 09:36:20  djm
**    Added new services.
**
**    Revision 0.23  2000/08/30 11:39:38  djm
**    Added new global function prototypes. Added (no)filestore switch text.
**
**    Revision 0.22  2000/08/22 17:55:42  djm
**    Moved some internal globals to public globals.
**
**    Revision 0.21  2000/08/18 14:38:37  djm
**    Moved the SDB storage file path to a global variable.
**
**    Revision 0.20  2000/07/21 14:03:12  djm
**    Added new status and service codes for file storage operation.
**
**    Revision 0.19  2000/07/14 15:58:42  djm
**    Added some new status codes.
**
**    Revision 0.18  2000/07/05 10:34:22  djm
**    Now makes use of the SERVICE_START macro for services.
**
**    Revision 0.17  2000/06/26 14:37:25  djm
**    Added extra error/status codes.
**
**    Revision 0.16  2000/06/23 10:22:29  djm
**    Added enumerated items for the COUNT- services.
**
**    Revision 0.15  2000/06/05 16:03:51  djm
**    Added more units.
**
**    Revision 0.14  2000/05/31 08:45:14  djm
**    Added new error codes for linked list manipulations.
**    Also made some slight changes to the unit codes.
**
**    Revision 0.13  2000/05/30 14:04:47  djm
**    Addition of composite units enumeration.
**    Also changed resolution on angular velocity and acceleration units.
**
**    Revision 0.12  2000/05/26 10:37:15  djm
**    Changes made to enumerated list of units. Added end-of-list marker
**    and provided some extra units for 64-bit measurements.
**
**    Revision 0.11  2000/05/23 15:22:45  djm
**    Added some new unit types.
**
**    Revision 0.10  2000/05/21 15:29:39  djm
**    Added new services to SDB's service list.
**
**    Revision 0.9  2000/05/18 14:59:17  djm
**    Added "end-of-list" marker enumeration for eSdbTaskDataIds.
**
**    Revision 0.8  2000/05/18 13:40:36  djm
**    Added a new structure definition, along with enumerated
**    identifiers, for managing the SDB's global task data.
**
**    Revision 0.7  2000/05/18 09:04:31  djm
**    Slight changes to global data structures in order to
**    correct and neaten them for the ICD definition.
**
**    Revision 0.6  2000/05/17 11:15:04  djm
**    Expanded units list.
**
**    Revision 0.5  2000/05/17 10:30:36  djm
**    Rationalisation of public structures that may be used to assist
**    with the encoding and decoding of SDB transaction messages.
**
**    Revision 0.4  2000/05/12 15:29:23  djm
**    Added two new fields to global task-data structure definition.
**
**    Revision 0.3  2000/05/12 08:18:49  djm
**    Change to data structures.
**
**    Revision 0.2  2000/05/11 09:23:06  djm
**    Changed to include provision for new CIL header structure.
**
**    Revision 0.1  2000/05/09 08:44:11  djm
**    Initial creation.
**
**
*/


#ifndef SDB_H_DEFINED
#define SDB_H_DEFINED


/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_SDB_MAIN
   #define E_SDB_EXTERN
   #define E_SDB_INIT(Initialisation) = Initialisation
#else
   #define E_SDB_EXTERN extern
   #define E_SDB_INIT(Initaliasation)
#endif


/* Required include files */

#include <limits.h>
#include "TtlSystem.h"       /* For Status_t definition */
#include "Mcp.h"             /* For MCP defined services */


/* Enumerate list of status/error values */

typedef enum eSdbStatus_e
{
   E_SDB_GEN_ERR = STATUS_START(SDB),  /* Miscellaneous error */
   E_SDB_SRC_UNKNOWN,        /* Msg rx'd from unrecognised CIL src */
   E_SDB_WRONG_SRC,          /* Msg rx'd from inappropriate CIL src */
   E_SDB_WRONG_DST,          /* Msg rx'd with non-SDB destination */
   E_SDB_CLA_UNKNOWN,        /* Unrecognised command line arg */
   E_SDB_NOT_COMMAND,        /* The SDB has rejected a rx'd non-command */
   E_SDB_CLASS_ERR,          /* Message received with incorrect CIL class */
   E_SDB_TRUNCATED,          /* Insufficient data found in CIL message data */
   E_SDB_MALLOC_FAIL,        /* Failure to allocate dynamic memory */
   E_SDB_UNKNOWN_DEFN,       /* Data element definition not known */
   E_SDB_NO_VALUES,          /* Data element has no values stored */
   E_SDB_LL_NOTEMPTY,        /* Linked list not empty, when it was expected */
   E_SDB_LL_EMPTY,           /* Linked list empty, when it wasn't expected */
   E_SDB_LL_NULLENTRY,       /* Attempted to insert null entry in linked list */
   E_SDB_LL_ENDOFLIST,       /* Unexpected end of link list encountered */
   E_SDB_UNKNOWN_SERVICE,    /* The SDB has rx'd an unrecognised command */
   E_SDB_BUFFER_OVERFLOW,    /* Too much data generated for buffer capacity */
   E_SDB_FWRITE_FAIL,        /* Unable to write data to file */
   E_SDB_FOPEN_FAIL,         /* Unable to open file for storage of data */
   E_SDB_HDR_MN_WRITE_ERR,   /* Error writing magic-no. to storage file hdr */
   E_SDB_HDR_TS_WRITE_ERR,   /* Error writing timestamp to storage file hdr */
   E_SDB_ENCODE_FAILURE,     /* Unable to correctly generate a storage code */
   E_SDB_SFR_SPAWN_FAIL,     /* Unable to spawn the storage file reader */
   E_SDB_SFR_NOT_AVAIL,      /* Storage file reading not available (not QNX) */
   E_SDB_NO_UNITS_FILE,      /* Units file empty or non-existant */
   E_SDB_NO_SHELL,           /* There's no shell or command handler available */
   E_SDB_WRITE_ERR_LIMIT,    /* Max no. write-to-file failures exceeded */
   E_SDB_HBEAT_FAIL,         /* No heartbeats or error processing response */
   E_SDB_NOT_AUTH,           /* Not authorised to preform this command */

   E_SDB_EOERR_LIST,         /* End error list marker (DON'T USE FOR STATUS) */
   E_SDB_STATUS_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes */
} eSdbStatus_t;



/* SDB command set (services offered to other programs) */

typedef enum eSdbCommands_e
{
   E_SDB_HEARTBEAT = E_MCP_HEARTBEAT,  /* Heartbeat command */
   E_SDB_SHUTDOWN  = E_MCP_SHUTDOWN,   /* Shutdown command */
   E_SDB_SAFESTATE = E_MCP_SAFESTATE,  /* Safestate command */
   E_SDB_ACTIVATE  = E_MCP_ACTIVATE,   /* Activate command */

   E_SDB_PURGE = SERVICE_START(SDB),   /* Cause the SDB to delete old entries */
   E_SDB_SUBMIT_1,           /* Submit data for storage */
   E_SDB_RETRIEVE_1,         /* Request data from the database */
   E_SDB_SUBMIT_N,           /* Submit block data for storage */
   E_SDB_RETRIEVE_N,         /* Request block data from the database */
   E_SDB_LISTSOURCES,        /* Request source IDs for all submitted data */
   E_SDB_LISTDATA,           /* Request data IDs for a particular source */
   E_SDB_COUNTSOURCES,       /* Reuqest number of different sources in SDB */
   E_SDB_COUNTDATA,          /* Request number of data definitions for a Src */
   E_SDB_COUNTMSRMENTS,      /* Request number of measurements for a Src:Dat */
   E_SDB_RETRIEVE_1R,        /* Request data from the database (robust mode) */
   E_SDB_RETRIEVE_F,         /* Request data from storage file (robust mode) */
   E_SDB_SUBMIT_1P,          /* Post data for submission - no response */
   E_SDB_CLEAR_S,            /* Clear data with a particular SourceID */
   E_SDB_CLEAR_1,            /* Clear specific data submissions */
   E_SDB_RETRIEVE_L,         /* Request latest data from file (robust mode) */
   E_SDB_COMMAND_EOL,        /* End of enumerated list of commands */
   E_SDB_COMMAND_MAX_VALUE = INT_MAX   /* Req'd to force size to 4 bytes */
} eSdbCommands_t;


/*
** Global task data
*/

typedef enum eSdbDataId_e
{
   D_SDB_DATAID_BOL,        /* Begining of data list - do not use as an index */

   D_SDB_PROC_STATE         /* Process state */
      = D_MCP_PROC_STATE,
   D_SDB_AUTH_STATE         /* Requested/granted suthorisation state */
      = D_MCP_AUTH_STATE,
   D_SDB_SYS_REQUEST        /* System requests made to MCP */
      = D_MCP_SYS_REQUEST,

   D_SDB_QTY_SUBMITTED      /* No. datum submissions since run-time */
      = D_MCP_FIRST_USER_DATUM,
   D_SDB_QTY_RETRIEVED,     /* No. datum retrievals since run-time */
   D_SDB_QTY_HBEAT,         /* No. heartbeats received since run-time */
   D_SDB_QTY_ERRORS,        /* No. comms/data errors since run-time */
   D_SDB_QTY_COUNT,         /* No. count commands received */
   D_SDB_QTY_LIST,          /* No. list commands received */
   D_SDB_QTY_MISC,          /* No. miscellaneous commands received */
   D_SDB_QTY_DEFNS,         /* No. data elements (definitions) being stored */
   D_SDB_QTY_HISTLIM,       /* No. data lists that have been size limited */
   D_SDB_TOT_VOLATILE_DATA, /* Total no. measurments in volatile storage */

   D_SDB_DATAID_EOL,        /* End of list marker - do not use as an index */
   E_SDB_DATAID_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes */
   
} eSdbDataId_t;


typedef struct
{
   Int32_t Value;            /* Actual value of the global task data */
   Int32_t Units;            /* The units of that value (for decoding) */
} eSdbTaskData_t;


/*
** Name of file to contain SDB units listing.
*/

#define E_SDB_UNITS_FILENAME "SdbUnits.dat"      /* Units listing file name */

/*
** Define custom command line switch text (exlcuding the "-" switch marker)
*/

#define E_SDB_FILESTORESTR "filestore"
#define E_SDB_NOFILESTORESTR "nofilestore"
#define E_SDB_DATAPATH     "datapath"
#define E_SDB_CLEANUP      "cleanup"
#define E_SDB_MYSQLHOST    "host"
#define E_SDB_MYSQLPORT    "port"


/* SDB type encodings (NOT IMPLEMENTED) */
/*
typedef enum
{
   E_SDB_INVALID_TYPE = 0,
   E_SDB_BOOL
} eSdbType_t;
*/

/* SDB units encodings */

typedef enum eSdbUnit_e
{
   E_SDB_INVALID_UNITS = 0,  /* Trap for non-specified units (=start of list) */

   E_SDB_UNSUPPORTED_UNITS,  /* Indicate datum not supported at this time */

   /*
   ** 32-bit signed integer units
   */

   E_SDB_NO_UNITS,           /* Unit-free measurement (counts, etc.) */
   E_SDB_MGAIN,              /* Gain factor x 1000 */
   E_SDB_MAS_UNITS,          /* Milliarcseconds */
   E_SDB_MKELVIN_UNITS,      /* Millikelvin */
   E_SDB_MCELSIUS_UNITS,     /* Millicelsius */
   E_SDB_MVOLT_UNITS,        /* Millivolts */
   E_SDB_MAMP_UNITS,         /* Milliamperes */
   E_SDB_UVOLT_UNITS,        /* Microvolts */
   E_SDB_UAMP_UNITS,         /* Microamperes */
   E_SDB_MAMP_PER_VOLT_UNITS,/* Milliamperes per volt */
   E_SDB_BITS_PER_VOLT_UNITS,/* Bits per volt */
   E_SDB_BITS_UNITS,         /* Bit field */
   E_SDB_MINUTES_UNITS,      /* Minutes */
   E_SDB_SEC_UNITS,          /* Seconds */
   E_SDB_MSEC_UNITS,         /* Milliseconds */
   E_SDB_USEC_UNITS,         /* Microseconds */
   E_SDB_NSEC_UNITS,         /* Nanoseconds */
   E_SDB_MASPERMS_UNITS,     /* Milliarcseconds per millisecond */
   E_SDB_MASPERMSPERMS_UNITS,/* Milliarcseconds per millisecond squared */
   E_SDB_MNEWTONMETRES_UNITS,/* Millinewton metres, Torque */
   E_SDB_MNM_PER_AMP_UNITS,  /* Millinewton metres per ampere */
   E_SDB_PROCSTATE_UNITS,    /* TTL process state */
   E_SDB_TRUEFALSE_UNITS,    /* Boolean True/False */
   E_SDB_ONOFF_UNITS,        /* Boolean On/Off */
   E_SDB_YESNO_UNITS,        /* Boolean Yes/No */
   E_SDB_MPERCENT_UNITS,     /* Milli percentage (1000 = 1%) */
   E_SDB_HERTZ_UNITS,        /* Hertz for frequency */
   E_SDB_SECPERYR_UNITS,     /* Seconds per year (eg proper motion of a star) */
   E_SDB_MYR_UNITS,          /* Milliyear */
   E_SDB_METREPERSEC_UNITS,  /* Metres per second */
   E_SDB_MSEC_PER_YR,        /* Milliseconds per year */
   E_SDB_MAS_PER_YR,         /* Milliarcseconds per year */
   E_SDB_MSEC_PER_DAY,       /* Milliseconds per day */
   E_SDB_MAS_PER_DAY,        /* Milliarcseconds per day */
   E_SDB_SHUTTER_STATE_UNITS,/* State of an enclosure mechanism */
   E_SDB_CIL_ID_UNITS,       /* CIL identifier */
   E_SDB_AUTH_STATE_UNITS,   /* Requested/granted authorisation state */
   E_SDB_BYTES_UNITS,        /* Bytes */
   E_SDB_KBYTES_UNITS,       /* Kilobytes (1024 bytes) */
   E_SDB_MBYTES_UNITS,       /* Megabytes (1024 kilobytes) */
   E_SDB_RPM_UNITS,          /* Revolutions per minute */
   E_SDB_SYSREQ_UNITS,       /* System requests made to MCP */
   E_SDB_MM_PER_SEC_UNITS,   /* Milli-metres per second */
   E_SDB_UHERTZ_UNITS,       /* Micro-hertz for frequency */
   E_SDB_MBAR_UNITS,         /* Milli-bar for (atmospheric) pressure */
   E_SDB_OID_UNITS,          /* Oid contains an Oid as a value, ie CFL */
   E_SDB_INDEX_UNITS,        /* Table or Array Offset */
   E_SDB_BAR_UNITS,          /* Bar for PLC fluid pressures */
   E_SDB_DCELSIUS_UNITS,     /* Deci-celcius for PLC temperatures */
   E_SDB_LTR_PER_MIN_UNITS,  /* Litres per minute for PLC flow-rates */
   E_SDB_MVERSION_UNITS,     /* Milli-version, eg v1.23 is 1230 */
   E_SDB_METRES_UNITS,       /* Metres */
   E_SDB_MILLIMETRES_UNITS,  /* Milli-Metres */
   E_SDB_MICRONS_UNITS,      /* Microns */
   E_SDB_NANOMETRES_UNITS,   /* Nano-metres */
   E_SDB_HOURS_UNITS,        /* Hours */
   E_SDB_MILLIDEGREES_UNITS, /* Milli-degrees */
   E_SDB_ARCSEC_UNITS,       /* Arc-seconds */
   E_SDB_SSE_STRING_UNITS,   /* SSE encoded string */
   E_SDB_NM_PER_CEL_UNITS,   /* Nanometres per degree celsius */
   E_SDB_EPT_DATA_UNITS,     /* EPT Data State Units */
   E_SDB_SPT_DATA_UNITS,     /* SPT Data State Units */
   E_SDB_IET_DATA_UNITS,     /* IET Data State Units */
   E_SDB_PACKAGE_ID_UNITS,   /* TTL Package ID Units */
   E_SDB_TELESCOPE_UNITS,    /* Identification of a telescope, or group of */
   E_SDB_LITRES_UNITS,       /* Litres */
   E_SDB_MILLILITRES_UNITS,  /* Milli-litres */
   E_SDB_MLTR_PER_MIN_UNITS, /* Milli-litres per minute */
   E_SDB_MPIXEL_UNITS,       /* Milli-pixels */
   E_SDB_MSTARMAG_UNITS,     /* Milli-star magnitudes */
   E_SDB_UAS_UNITS,          /* Microarcseconds */
   E_SDB_MPERCENT_RH_UNITS,  /* Milli-percent relative humidity */
                             /* Note that PLC device units must be contiguous */
   E_SDB_PLC_BOL_UNITS,      /* PLC device units beginning of list */
   E_SDB_PLC_2_STATE_UNITS,  /* PLC 2-state device */
   E_SDB_PLC_4_STATE_UNITS,  /* PLC 4-state device */
   E_SDB_PLC_LIMIT_UNITS,    /* PLC Limit device */
   E_SDB_PLC_SENSOR_UNITS,   /* PLC Sensor device */
   E_SDB_PLC_MOTOR_UNITS,    /* PLC Motor device */
   E_SDB_PLC_ACTUATOR_UNITS, /* PLC Actuator device */
   E_SDB_PLC_STATUS_UNITS,   /* PLC status */
   E_SDB_PLC_VERSION_UNITS,  /* PLC version */
   E_SDB_PLC_SYSSTATUS_UNITS,/* PLC system status */
   E_SDB_PLC_HPLCSTATUS_UNITS,/* HPLC system status */
   E_SDB_PLC_SPARE_1_UNITS,  /* PLC spare units 1 */
   E_SDB_PLC_SPARE_2_UNITS,  /* PLC spare units 2 */
   E_SDB_PLC_SPARE_3_UNITS,  /* PLC spare units 3 */
   E_SDB_PLC_SPARE_4_UNITS,  /* PLC spare units 4 */
   E_SDB_PLC_EOL_UNITS,      /* PLC device units end of list */

   E_SDB_MILLIDEG_PER_VOLT,  /* Milli-degrees per volt */
   E_SDB_MW_PERMPERM_UNITS,  /* Milli-watts per metre squared */
   E_SDB_AGSTATE_UNITS,      /* Autoguider state */
   E_SDB_BAUD_UNITS,         /* Baud rate (bits per second) */
   E_SDB_VERSION_16_UNITS,   /* Major/minor version packed into 16 bits */
   E_SDB_MASPERSEC_UNITS,    /* Milliarcseconds per second */

   E_SDB_PLC_ALARM_UNITS,    /* PLC alarm level */
   E_SDB_SPLC_DEMAND_UNITS,  /* Services PLC demand */
   E_SDB_AXIS_TOP_UNITS,     /* Axis top state */
   E_SDB_AXIS_USER_UNITS,    /* Axis user-reported state */
   E_SDB_NODE_MOTION_UNITS,  /* Node Motion Controller state */
   E_SDB_AXIS_MOTION_UNITS,  /* Axis Motion Controller state */
   E_SDB_LIN_ENC_STATE_UNITS,/* Linear Encoder state */
   E_SDB_ENL_STATE_UNITS,    /* Encoder Non-Linearity state */
   E_SDB_HOMING_DIR_UNITS,   /* Encoder homing direction */
   E_SDB_ENC_STATE_UNITS,    /* Encoder state */
   E_SDB_ENC_REF_STATE_UNITS,/* Encoder reference state */
   E_SDB_ENC_STRENGTH_UNITS, /* Encoder signal strength */
   E_SDB_AGD_STATE_UNITS,    /* Autoguider Deployment state */
   E_SDB_AGF_STATE_UNITS,    /* Autoguider Focus state */
   E_SDB_SFD_STATE_UNITS,    /* Science Fold Deployment state */
   E_SDB_SFP_STATE_UNITS,    /* Science Fold Position state */
   E_SDB_SMF_STATE_UNITS,    /* Secondary Mirror Focus state */
   E_SDB_IR_FILTER_POS_UNITS,/* Autoguider IR Filter state */
   E_SDB_DARKSLIDE_POS_UNITS,/* Dark Slide state */
   E_SDB_SFD_POS_UNITS,      /* Science Fold Dployment position */
   E_SDB_SFP_PORT_UNITS,     /* Science Fold Position port */
   E_SDB_ACM_STATE_UNITS,    /* ACADS Mechanisms state */
   E_SDB_SIF_STATE_UNITS,    /* Mirror Support Interface state */

   E_SDB_NOMORE_32BIT_UNITS, /* No more 32-bit units in list */

   /*
   ** 64-bit signed integer units
   ** (must be paired in application to be valid)
   */

   E_SDB_MSW_NO_UNITS        /* MSW of signed 64-bit unit free measurement */
                  = 0x10000,
   E_SDB_LSW_NO_UNITS,       /* LSW of signed 64-bit unit free measurement */
   E_SDB_MSW_TTS_UNITS,      /* TTL TimeStamp Most Significant Word (sec) */
   E_SDB_LSW_TTS_UNITS,      /* TTL TimeStamp Least Significant Word (nsec) */
   E_SDB_MSW_RAWENC_UNITS,   /* Most Sig. Word of raw encoder counts */
   E_SDB_LSW_RAWENC_UNITS,   /* Least Sig. Word of raw encoder counts */

   /*
   ** Identifying the use of a composite unit. E.g. any unit that is
   ** stored as a single quantity, but is broken down for transmission
   ** to the SDB.
   */

   E_SDB_TTS_UNITS,          /* TTL TimeStamp structure */
   E_SDB_RAWENC_UNITS,       /* Representation of 48-bit raw encoder counts */

   /*
   ** End of list marker
   ** (never to be used as a units for the applicaion)
   */

   E_SDB_NOMORE_UNITS,       /* End of enumerated list of units */
   E_SDB_UNITS_MAX_VALUE = INT_MAX     /* Req'd to force size to 4 bytes */

} eSdbUnit_t;


/*
** Raw SDB file storage structure. This is the format in which data is
** written to file. There may be a different format for processed/reduced
** data (for improved speed of file-reading). It is only intended for use
** by the SDB and SFR. This is now an unsigned int thus freeing up the sign
** for storage as the sign is not used. This has been done to resolve bug 
** #49. If bug #49 occurs again the a further solution will have to be 
** investigated. 
*/

typedef Uint32_t eSdbCode_t;  /* Type definition for data storage to file */

struct eSdbRawFmt_s
{
   eSdbCode_t Code;          /* ID code for the data definition */
   Uint32_t   TimeOffset;    /* Time since start of hour in microseconds */
   Int32_t    Value;         /* Actual value of the datum event */
};
typedef struct eSdbRawFmt_s eSdbRawFmt_t;


#define E_SDB_CODE_MASKSIZE  24        /* No. bits to mask for storage code */
#define E_SDB_HEADER_STRING "SDBR"     /* Magic key at start of storage files */



/*
** Storage structures for submitted/requested SDB data. These are for use
** by third party applications that wish to access the SDB.
*/


typedef struct {             /* -- Measurement (time-value pair) -- */
   eTtlTime_t  TimeStamp;    /* Timestamp associated with value */
   Int32_t     Value;        /* Actual value, encoded as per "Units" */
} eSdbMsrment_t;

typedef struct {             /* -- Single datum -- */
   Int32_t     SourceId;     /* Source (parent) ID number */
   Int32_t     DatumId;      /* Data element ID number */
   Int32_t     Units;        /* Measurement units of value (enum) */
   eSdbMsrment_t Msrment;    /* Time-value pair */
} eSdbDatum_t;

typedef struct {             /* -- Block of data -- */
   Int32_t     SourceId;     /* Source (parent) ID number */
   Int32_t     DatumId;      /* Data element ID number */
   Int32_t     Units;        /* Measurement units of value (enum) */
   Uint32_t    NumMsrments;  /* Number of time-value pairs */
   eSdbMsrment_t *MsrmentPtr;/* Pointer to array of time-value pairs */
} eSdbBlock_t;

typedef struct {             /* -- Single datum request -- */
   Int32_t     SourceId;     /* Source (parent) ID number */
   Int32_t     DatumId;      /* Data element ID number */
} eSdbSngReq_t;

typedef struct {             /* -- Multiple (block) data request -- */
   Int32_t     SourceId;     /* Source (parent) ID number */
   Int32_t     DatumId;      /* Data element ID number */
   Uint32_t    NumMsrments;  /* Max number of measurements to retrieve */
   eTtlTime_t  OldestTime;   /* Earlier time limit of data range */
   eTtlTime_t  NewestTime;   /* Later time limit of data range */
} eSdbMulReq_t;


/* Public function prototypes */

extern Status_t eSdbStoreIdEncode(eSdbSngReq_t *ReqPtr, eSdbCode_t *CodePtr);
extern Status_t eSdbStoreIdDecode(eSdbCode_t *CodePtr, eSdbSngReq_t *ReqPtr);



#endif


/* EOF */
