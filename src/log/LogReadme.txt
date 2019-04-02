=====
 LOG
=====

This package is the System Logging Package (LOG).


History:

   LOG_1_02
   Default compile-time host now 'localhost' for portability.

   LOG_1_01
   Minor changes to aid porting to Linux - no functional changes.

   LOG_1_00
   Ported to QNX v6.

   LOG_0_08
   Added "safe" function vsnprintf() for constructing log string without
   a danger of overrunning a fixed size buffer.

   LOG_0_07
   Added a trap to warn if eLogDebug() or eLogInfo() are called with a NULL
   pointer to the "Format". This is common coding error on downgrading the
   priority of an eLogNotice() or higher to either of the the two 
   aforementioned calls, but without removing the "0" status argument.

   LOG_0_06
   Addition of a globally defined "facility code". The full list of "standard"
   facilities are defined in Log.h, along with the selected facility
   (E_LOG_FACILITY) used for the messages generate by the LOG package library. 

   LOG_0_05
   Addition of optional configuration file to specify how the logging is to 
   operate. This uses the CFU library, so it is now necessary to link in the
   CFU library with all applications using LOG.

   LOG_0_04
   Check for destination host for logging unresolved.

   LOG_0_03
   LOG setup enhanced to include an instance name.

   LOG_0_02
   Build process enhanced to use default labels, and to support QNX and Linux.

   LOG_0_01
   First baseline.


Files:

   Log.h        
   Log.c        
   LogSnprintf.c
   LogSnprintf.h
   LogPrivate.h 
   LogReadme.txt
   Log.lis      
   Log.mak      
   testlog.c    


Documentation:

   \\Files\97-001\Development\MCS Mount Control System\User Guide\Log

   LOG Users Guide v0.09.doc


Notes:

   There is some test code provided. This is RCS'd in the /opt/ttl/src/log area.


10-Jul-2006, MJF.

=====
