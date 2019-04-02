=====
 CCT
=====

This application is the Computer Control Task (CCT).


History:

   CCT_1_05
   Improvements to handling of the case where a spawned application cannot be
   executed.

   CCT_1_04
   Fix to log message on inability to open the shutdown configuration file.

   CCT_1_03
   Fix to typo in makefile. Tidy-up to definitions for Linux.

   CCT_1_02
   Shutdown more elegantly than "shutdown -f", use -v (verbose) instead.

   CCT_1_01
   Build against static libraries.

   CCT_1_00
   Ported onto QNX v6.

   CCT_0_19
   Allow commands to be issued with NO_UNITS as well as TRUEFALSE_UNITS.

   CCT_0_18
   Minor change to allow compilation on Linux.

   CCT_0_17
   Increase Cil receive timeout to 100ms from 1ms.
   Reported that Cct was hogging resources on Tcc (bug #58).

   CCT_0_16
   The VMS implementation of stop/start application has been simplified by
   switching from the VMS RTL call SYS$CREPRC() to the ANSI call system().
   NOTE: the VMS stop/start application configuration files should be a single
   line of the form @path:application.com

   CCT_0_15
   Don't check for the existence of the configuration files on every heartbeat, 
   so hence don't assume 'warn' state in their absence.

   CCT_0_14
   Change to QNX variant only - before forking a new process ignore the child 
   signal, which avoids leaving zombies after a process has been killed.

   CCT_0_13
   Change of log message levels, especially in the case where the CCT is to kill
   a process which has already been correctly shut down (this is not an error).

   CCT_0_12
   Fix to response to commands, in the case where a heartbeat has been received
   between the command being received and the response sent.

   CCT_0_11
   Addition of delay following the execution of each process. This is 1000 ms 
   by default, but may be altered on the command-line with the '-delay' switch.

   CCT_0_10
   Implementation of CCT on VMS to start and stop software using scripts. Note 
   that a poll of 'application running' in this case will check whether the 
   script used to run the application is still running. It will be 
   implementation-defined whether the script to run the application will remain 
   executing, hence the 'application running' flag must be used with caution in 
   such cases. The CCT will now accept commands from the MCP or from the TST, 
   to be during integration only.

   CCT_0_09
   Addition of a '-shutdown' switch to specify a configuration file containing 
   commands to run on the shutdown of the application software. If this file is 
   specified, then this file is invoked instead of killing processes using pids.
   This is the default when built for VMS. Additionally, the CCT process state
   will be 'warn' if it is unable to locate any of its configuration files.

   CCT_0_08
   Change of default VMS CIL ID to be CC8.

   CCT_0_07
   Changes to VMS logical names. The default CIL map is now "CIL_MAP", and the 
   default configuration file and reboot/shutdown scripts are now to be in 
   directory "COMPTASK_DIR".

   CCT_0_06
   Submit status parameters into the SDB on each heartbeat, rather than only
   on queries.

   CCT_0_05
   Send the reply message to an operating system shutdown/restart command
   before performing the action.

   CCT_0_04
   Full functionality for VMS operating system.

   CCT_0_03
   Increased level of POSIX compliance, and full functionality for Linux.

   CCT_0_02
   Fully functional for the QNX operating system.

   CCT_0_01
   Initial baseline. Responds to heartbeats, set/get commands, 
   safe-state/activate commands, and submits data to the SDB.


Files:

   Cct.h           
   CctPrivate.h    
   CctMain.c       
   CctInit.c       
   CctOsInterface.c
   CctUtil.c       
   CctOsInterface.h
   Cct.mak         
   CctVmsBuild.com 
   CctReadme.txt   
   Cct.lis         


Documentation:

   -


Notes:

   The following operating system commands are used :-

             Restart OS                       Close OS
   QNX v4    shutdown -f                      shutdown -fb
   QNX v6    shutdown -v                      shutdown -vb
   Linux     /sbin/shutdown -r now            /sbin/shutdown -h now
   VMS       @COMPTASK_DIR:TTL_VMS_REBOOT.COM @COMPTASK_DIR:TTL_VMS_SHUTDOWN.COM

   The various CCTs are compiled with the following defaults for CIL :-

             CIL ID                           CIL Map              
   QNX v4    CC0                              /opt/ttl/etc/Cil.map 
   QNX v6    CC0                              /ttl/sw/etc/Cil.map 
   Linux     CC0                              /ttl/sw/etc/Cil.map 
   VMS       CC8                              CIL_MAP              

   The various CCTs are compiled with the following configuration file defaults
   (noting that the absence of a Shutdown File will use the spawned pids to 
   close the application software) :-

             Config File                      Shutdown File
   QNX v4    /opt/ttl/etc/Cct.cfg             -
   QNX v6    /ttl/sw/etc/Cct.cfg              -
   Linux     /ttl/sw/etc/Cct.cfg              -
   VMS       COMPTASK_DIR:Cct.cfg             COMPTASK_DIR:CctAppStop.cfg

   To build the code on QNX and Linux use the makefile : make -f Cct.mak
   To build the code on VMS use the command-file       : @CctVmsBuild.com

   For test purposes, versions 0.01, 0.02 and 0.03 accept SET commands from the
   TST rather than the MCP, and versions 0.10 upwards accept SET commands from 
   both the MCP and the TST.


04-Dec-2007, MJF.

=====

