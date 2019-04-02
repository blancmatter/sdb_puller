=====
 CLU
=====

This package is the Command Line Utilities (CLU).


History:

   CLU_1_01
   Use 'stderr' rather than 'stdout' as default stream. This change solved a
   strange problem where '-h' output was truncated, without understanding why 
   this was actually a problem with 'stdout' (CLU_0_15).

   CLU_1_00
   Ported to QNX v6.

   CLU_0_14
   Ported to VMS. Note that the resetting of the priority of VMS processes has 
   not been implemented.

   CLU_0_13
   Ensure that SYS_NOMINAL is returned from a call to eCluSignOn when help 
   has been specified on the command-line.

   CLU_0_12
   Allow arguments supplied with switches to start with a '-'. Hence, negative
   numerical arguments can be specified.

   CLU_0_11
   Improvement to 'submit version' function when CIL setup has already been
   performed.

   CLU_0_10
   Splitting up of library into two objects, so that applications not having a 
   CIL address do not need to link in CIL if they do not use the 'submit 
   version' function.

   CLU_0_09
   Addition of function to submit the version of the application into the SDB.
   This function also performs a CIL setup if one has not already been done.

   CLU_0_08
   Addition of automatic instance detection based on CIL address, which is used
   in the call to eLogSetup. Function eCluSignOn now returns a log error if the
   call to eLogSetup has failed.

   CLU_0_07
   Additional work on Linux porting.

   CLU_0_06
   Ported to Linux. Changes are required only in the Clu.mak makefile in
   order to build under QNX/Linux now.

   CLU_0_05
   Changing of number of significant letters required to match some common 
   arguments. Enables better re-use of initial letters.

   CLU_0_04
   Correction to small problem when specifying an alternate CIL name.   

   CLU_0_03
   Functions return Status_t type and SYS_NOMINAL, not eCluStatus_t and 
   E_CLU_NOMINAL. Name of 'CIL' common switch now lower-case. Program name 
   gets checked without the path. eCluParseArgs now accepts string pointer, 
   and any problematic switches get written in. Argument clashes for verbose 
   and quiet removed. 

   CLU_0_02          
   Addition of 'starting application message'. Change of log priority for 
   reporting of parsed arguments. 

   CLU_0_1
   First version.


Files:

   Clu.h                <-- RCS'd in this directory
   Clu.c                <-- RCS'd in this directory
   CluVersion.c         <-- RCS'd in this directory
   CluPrivate.h         <-- RCS'd in this directory
   CluReadme.txt        <-- RCS'd in this directory
   Clu.lis              <-- RCS'd in this directory
   Makefile             <-- RCS'd in this directory
   CluExample.c         <-- RCS'd in this directory


Documentation:

   \\Files\97-001\Development\MCS Mount Control System\Detailed Design\Clu

   CLU User Guide v0.05.doc


Notes:

   There is some example source provided. This is RCS'd in the /opt/ttl/src/clu
area. This is an example of using CLU.


17-Jun-2005, MJF.

=====