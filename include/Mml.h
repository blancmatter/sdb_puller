/*******************************************************************************
** Module Name:
**    Mml.h
**
** Purpose:
**    Public header file for the MML package.
**
** Description:
**    This is the public header file for the Matrix Manipulation Library
**    (MML) package.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    ...
**
** Author(s):
**    Derek J. McKay (djm)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000-2001
**
** Version:
**    $Id: Mml.h,v 0.1 2001/05/17 14:59:10 djm Exp $
**
** History:
**    $Log: Mml.h,v $
**    Revision 0.1  2001/05/17 14:59:10  djm
**    Initial creation.
**
**
*******************************************************************************/


#ifndef _MML_H_DEFINED
#define _MML_H_DEFINED


/*
** Code for coping with decl'n/def'n/init'n of global variables
*/

#ifdef M_MML_MAIN
   #define E_MML_EXTERN
   #define E_MML_INIT(Initialisation) = Initialisation
#else
   #define E_MML_EXTERN extern
   #define E_MML_INIT(Initaliasation)
#endif


/*
** Required include files
*/

#include <limits.h>          /* For INT_MAX             */
#include "TtlSystem.h"       /* For Status_t and MML definitions */

/*
** Define error stati and return values
*/

enum eMmlStatus_e
{
   E_MML_GEN_ERROR = STATUS_START(AEI),

   E_AEI_STATUS_EOL,             /* End Of List               */
   E_AEI_STATUS_MAX = INT_MAX    /* enum forced to be 4 bytes */
};

/*
** Function prototypes for exported functions.
*/


E_MML_EXTERN Status_t eMmlMatrixMul
(
   double *MtrxIn1,
   int RowsIn1,
   int ColsIn1,
   double *MtrxIn2,
   int RowsIn2,
   int ColsIn2,
   double *result
);


E_MML_EXTERN Status_t eMmlMatrixAdd
(
   double *MtrxIn1,
   int RowsIn1,
   int ColsIn1,
   double *MtrxIn2,
   int RowsIn2,
   int ColsIn2,
   double *result
);


E_MML_EXTERN Status_t eMmlMatrixAutoSum
(
   double  *MtrxIn1,
   int     RowsIn1,
   int     ColsIn1,
   double  *result
);


E_MML_EXTERN Status_t eMmlMatrixCopy
(
   double  *MtrxIn,
   int     RowsIn1,
   int     ColsIn1,
   double  *MtrxOut
);

E_MML_EXTERN Status_t eMmlPrint
(
   char    *Label,
   char    *Format,
   double  *Mtrx,
   int     RowsIn1,
   int     ColsIn1
);


#endif

/*******************************************************************************
** End of File Name: Mml.h
*******************************************************************************/
