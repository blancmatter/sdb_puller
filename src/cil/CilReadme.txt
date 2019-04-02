=====
 CIL
=====

This package is the Communications Interface Library (CIL).


Baselines:

   CIL_1_09
   Change of enumeration constructions for automatic lookup table generation.
   No functional changes. (CIL_0_16)
 
   CIL_1_08
   Minor modifications to build under linux.

   CIL_1_07
   Build against static libraries.

   CIL_1_06
   Code portability tweak to make VMS the exception calling 'recvfrom' function.

   CIL_1_05
   Code portability enhanced.

   CIL_1_04
   Set size of UDP receive and send buffers during CilSetup. Receive buffer set
   above default to help balance communications load during OPERATIONAL ON/OFF.

   CIL_1_03
   Tidied up to aid porting to Linux. No functional changes.

   CIL_1_02
   Further porting of test utilities.

   CIL_1_01
   Don't return a send error if the destination host is down (errno EHOSTDOWN).

   CIL_1_00
   Ported to QNX v6.

   CIL_0_14
   Addition of Nasmyth X+, Nasmyth X- and Prime focal stations, in terms of an
   axis derotator, a suite of auxiliary mechanisms and an autoguider.
   
   CIL_0_13
   Change to CIL map for Autoguider processes.
   
   CIL_0_12
   Update to testclient to output times.
   
   CIL_0_11
   Addition of AGG for Autoguider Guide Packets.

   CIL_0_10
   Addition of CIL identifiers for Primary Mirror Support.

   CIL_0_09
   CIL library now comprises of two objects, to provide access to eCilNameString
   to functions who are not actually using CIL for comms. Also CIL entries added
   for Computer Control Tasks, and names of AMN entries changed.

   CIL_0_08
   Node in the CIL map may be specified using the host name, and if not 
   matched then the IP address. Also use CFU to process the CIL map.

   CIL_0_07
   Addition of function to return whether or not a CIL setup has already been 
   performed.

   CIL_0_06
   Addition of function to return the CIL ID of the calling process.

   CIL_0_05
   Write time-stamp into CIL header when sending a CIL message.

   CIL_0_04
   Version that works under VMS, QNX & Linux

   CIL_0_03
   Addition of function to flush the incoming message queue.

   CIL_0_02
   Addition of function to return a CIL name using a CIL ID, without accessing
   the CIL map on disk.

   CIL_0_01
   First baseline created.


Files:

  Cil.c                
  Cil.h                
  CilPrivate.h         
  Cil.mak              
  CilReadme.txt        
  Cil.lis              


Documentation:

  \\Files\97-001\Development\MCS Mount Control System\User Guide\Cil

  UDP only CIL User Guide v0.14.doc

-------
 NOTES
-------

  There are some example test programs provided. These are RCS'd in the
/opt/ttl/src/cil area. They are an example client, broker and server.


12-Jul-2007, MJF.

=====
