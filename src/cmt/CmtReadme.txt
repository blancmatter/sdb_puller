=====
 CMT
=====

This application is the Computer Mointoring Task (CMT).


History:

   CMT_1_09
   Functionaliy proven for Linux - CPU loading, free disk space, free memory, 
   and NTP-locking. Note NTP took ~8 hours to unlock following an 'ntpd stop'
   but only several minutes to re-lock following an 'ntpd start'.

   CMT_1_08
   Tidy-up to Linux port - no functional changes.

   CMT_1_07
   Check NTP lock status, cpu load and memory free less often.
   Report changes of state of NTP lock at WARNING level.

   CMT_1_06
   Minor change for QNX6.3.

   CMT_1_05
   Minor improvements for Linux.

   CMT_1_04
   Added functionality for check of NTP lock for QNX6, VMS and Linux.

   CMT_1_03
   Basic functionality now implemented. Monitoring of
     o CPU load
     o Disk space
     o Used memory
   
   CMT_1_02
   Put linker options at end of linker command-line - no functional changes.

   CMT_1_01
   Fixed do it doesn't crash under QNX v6 (previously used QNX v4 calls which 
   compiled but seemed to crash without the migration manager). Whilst the 
   QNX v6 Computer Monitoring Task doesn't do any Computer Monitoring it's still
   a Task.

   CMT_1_00
   Ported to QNX v6.

   CMT_0_11
   Fix to version number. No functional changes.

   CMT_0_10
   Check for terminal communications failure (e.g. unable to allocate memory)
   during comms in, comms out and command response. Shutdown CMT on failure. 

   CMT_0_09
   Implemented functionality for basic monitoring on Linux systems.
   Corrected reporting of disk 4 free space.

   CMT_0_08
   Submit process version as part of initialisation.

   CMT_0_07
   Change path to VMS config file from TCSLIB_DIR to 
   UTILS_DIR to be consistent with CCT.

   CMT_0_06
   Increase Cil receive timeout to 100ms from 1ms.
   Cmt was reported to hog resources on Tcc.

   CMT_0_05
   Monitor disk usage and change state if a disk falls below a 
   defined threshold.
   Remove spurious command line argument.

   CMT_0_04
   Major reworking. 
   Now uses configuration files to decide what to monitor.
   Disk usage only monitored if requested so that all processes do not
   wastefully monitor the same disk.
   Also added monitoring of DSP PC104 board.

   UIT_0_01
   Contains no application-specific functionality, but handles and responds to 
   MCP messages, times out heartbeats, gets 'test' status data and submits it 
   to the SDB. Also has infrastructure to respond to an incoming command and 
   reply with status data, and to process incoming commands to perform actions
   (both of these untested).


Files:

   Cmt.h                <-- RCS'd in this directory
   CmPrivate.h          <-- RCS'd in this directory
   CmtMain.c            <-- RCS'd in this directory
   CmtInit.c            <-- RCS'd in this directory
   CmtInterface.c       <-- RCS'd in this directory
   CmtReport.c          <-- RCS'd in this directory
   CmtUtil.c            <-- RCS'd in this directory
   Cmt.mak              <-- RCS'd in this directory
   CmtReadme.txt        <-- RCS'd in this directory
   Cmt.lis              <-- RCS'd in this directory


Documentation:

   \\Files\97-001\Development\MCS Mount Control System\Detailed Design\Mcp

   MCP Detailed Design v0.05.doc

Notes:



16-Oct-2007, MJF.

=====
