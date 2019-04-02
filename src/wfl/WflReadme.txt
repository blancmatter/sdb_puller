=====
 WFL
=====

This package is the Wrapper Function Library (WFL).

The library provides wrapper functions around non-Ansi and 
non-Posix function calls for ease of portability between 
operating systems.

History:

   WFL_1_12
   Contents of Wfl.c relating to on-mount software moved into new file WflMsg.c.
   New function eWflDelay added. Linux porting status :-
   - Wfl.c
     - most functions ported but some untested
     - eWflKeyboardHit, eWflDevCtlMsg not yet ported, may not be required
     - eWflSetClockPeriod, eWflGetClockPeriod probably not relevant
   - WflMsg.c
     - no functions ported
   - WflPci.c
     - not yet ported, functions return E_WFL_NOOSSUPPORT for the interim
   - WflWatchdog.c
     - not yet ported, functions return E_WFL_NOOSSUPPORT for the interim
   - WflRes.c
     - autoguider functions probably not relevant for Linux
   - WflPhab.c
     - PhAB functions not relevant for Linux

   WFL_1_11
   Public header only includes O/S definitions if '-ansi' compiler switch used.

   WFL_1_10
   Minor modifications for building without errors under linux.

   WFL_1_09
   Reply with error to system messages in eWflMsgReceive() to
   avoid processes blocking on us.

   WFL_1_08
   Cygwin support added.

   WFL_1_07
   Tidied up to enable public header to compile under Linux. 
   WFL library itself not yet ported to Linux.

   WFL_1_06
   Added getprio() call to eWflSendPulse* functions.

   WFL_1_05
   Added eWflFileTruncate().

   WFL_1_04
   Tidied to compile under VMS.

   WFL_1_03
   Add GET/SET type to message codes.

   WFL_1_02
   Correction of WflWatchdog routines for use with Pmm.

   WFL_1_01
   Addition of fast-pulse, find-connection and interrupt-masking functions.

   WFL_1_00
   Don't build test programs. No other changes.

   WFL_0_04
   Fix eWflMsgSendToPid so that error is returned on failure to
   ConnectAttach().

   WFL_0_03
   Added build of PhAB routines in WflPhab.c.

   WFL_0_02
   Updated send/receive prototypes to be eWflMsg_t.

   WFL_0_01
   First version.


Files:

   Wfl.c
   Wfl.h
   Wfl.lis
   Wfl.mak
   WflPci.c
   WflRes.c
   WflWatchdog.c
   WflPhab.c
   WflPrivate.h
   WflReadme.txt


Documentation:



Notes:



05-Oct-2007, MJF.

=====
