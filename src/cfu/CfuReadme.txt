=====
 CFU
=====

This package is the Configuration File Utilities (CFU).


History:

   CFU_1_01
   Made 'eCfuSetup' robust to a NULL-pointer.

   CFU_1_00
   First QNX v6 baseline - identical to CFU_0_07.

   CFU_0_07
   Increase of maximum line length to 512 characters.

   CFU_0_06
   Filename passed to setup function now 'const'.

   CFU_0_05
   Allow up to 256 configuration files to be open simultaneously, the use of 
   which is nested. Note that an unsuccessful 'setup' of a configuration file
   still needs a corresponding 'complete'.

   CFU_0_04
   Fix to resetting of setup level when completing use of a configuration file.

   CFU_0_03
   Trim whitespace at start of line correctly in all cases.

   CFU_0_02
   Functions now return Status_t and SYS_NOMINAL.

   CFU_0_01
   First version.


Files:

   Cfu.h        
   Cfu.c        
   CfuPrivate.h 
   CfuReadme.txt
   Cfu.lis      
   Makefile     
   CfuExample.c 


Documentation:

   \\Files\97-001\Development\MCS Mount Control System\User Guide\Cfu

   CFU User Guide v0.04.doc


Notes:

   There is some example source provided. This is RCS'd in the /opt/ttl/src/cfu
area. This is an example of using CFU.


06-May-2004, MJF.

=====