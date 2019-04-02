/*                       
** Module Name:
**    WflPhab.c
**
** Purpose:
**    Provides wrappers around various functions
**
** Description:
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Authors:
**    Steve Foale (sxf)
**
** Version:
**    $Id: WflPhab.c,v 0.3 2007/10/05 09:23:33 mjf Exp $
**
** History:
**    $Log: WflPhab.c,v $
**    Revision 0.3  2007/10/05 09:23:33  mjf
**    Ensure file is only compiled for QNX.
**
**    Revision 0.2  2004/04/16 10:47:50  sxf
**    Use eWflMsg_t for send/receive prototypes.
**
**    Revision 0.1  2004/03/24 10:38:02  sxf
**    Initial version.
**
**
**
*/


/*
** System include files
*/

#include "TtlSystem.h"
#include "Wfl.h"

/*
** Local include files
*/

#include "WflPrivate.h"


/*
** Only relevant to QNX.
*/

#if defined E_WFL_OS_QNX4 || defined E_WFL_OS_QNX6


#ifndef I_WFL_OS_UNSUPPORTED

/******************************************************************************
** Function Name:
**    eWflPtAddInput
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Associate a handler function with a pid
**
** Description:
**
** Arguments:
**       const char *name  - Name of the shared memory object
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflPtAddInput( PtAppContext_t app_context,
                         pid_t pid,
                         PtInputCallbackProc_t input_func,
                         void *data)
{
   Status_t Status = SYS_NOMINAL;
   PtInputId_t *pInpId;

   #if defined E_WFL_OS_QNX4

   pInpId = PtAppAddInput( app_context, pid, input_func, data );
   if( pInpId == NULL )
      Status = E_WFL_NOPHHAND;

   #endif

   #if defined E_WFL_OS_QNX6
   int Chid;

   eWflGetChannelId( &Chid );

   if( Chid != -1 )
   {
      if( PhChannelAttach( Chid, -1, NULL ) != -1 )
      {
         pInpId = PtAppAddInput( app_context, pid, input_func, data );
         if( pInpId == NULL )
            Status = E_WFL_NOPHHAND;
      }
      else
      {
         Status = E_WFL_NOCHANNEL;
      }      
   }
   else
   {
      Status = E_WFL_NOCHANNEL;
   }


   #endif

   return Status;
}

#endif /*End of  I_WFL_OS_UNSUPPORTED */

#endif

