/*******************************************************************************
** Module Name:
**    Gsc.h
**
** Purpose:
**    Public header file for Generic (node) State Controller definitions.
**
** Description:
**    This file contains generic OID offsets to enable the implementation of a 
**    generic OID interface for node state controllers (NSC, AMS & PMS).
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    -
**
** Author(s):
**    Martyn J. Ford (mjf)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: Gsc.h,v 0.6 2003/04/10 10:23:48 mkb Exp $
**
** History:
**    $Log: Gsc.h,v $
**    Revision 0.6  2003/04/10 10:23:48  mkb
**    Added recover from limit command.
**
**    Revision 0.5  2001/11/27 15:08:32  mjf
**    Addition of 'halt demand'.
**
**    Revision 0.4  2001/10/17 10:49:28  mjf
**    Changes made to data IDs for revised node sub-system state model.
**
**    Revision 0.3  2001/09/19 13:24:09  mjf
**    Now called 'GSC'.
**
**    Revision 0.2  2001/03/26 15:08:49  mjf
**    Add the missing 'endif' for the sentry.
**
**    Revision 0.1  2001/03/26 14:37:41  mjf
**    Initial revision.
**
**
*******************************************************************************/

#ifndef GSC_H_DEFINED
#define GSC_H_DEFINED


/*
** Enumerated types
*/

/* Generic OID offsets */

typedef enum eGscDataId_e
{
   D_GSC_DATAID_BOL = 0,               /* Start the OID offset at zero */

   D_GSC_PROC_STATE = 1,               /* The process state */
   D_GSC_APP_VERSION,                  /* Package revision */

   D_GSC_TOP_LEVEL_STATE,              /* The top-level state */
   D_GSC_SHUTDOWN,                     /* Node shutdown flag */
   D_GSC_ENABLE,                       /* Node enable flag */
   D_GSC_HOME,                         /* Node home flag */
   D_GSC_STOP_DEMAND,                  /* Node stop demand */
   D_GSC_SAFE_STATE_COMMAND,           /* Node safe-state command flag */
   D_GSC_ACTIVATE_COMMAND,             /* Node activate command flag */
   D_GSC_AXIS_STOPPED,                 /* Axis stopped status */
   D_GSC_HALT_DEMAND,                  /* Node halt demand */
   D_GSC_RECOVER_LIMIT,                /* Recover from a limit. */

   D_GSC_DATAID_EOL                    /* End of list marker */
} eGscDataId_t;


#endif


/*
** EOF
*/


