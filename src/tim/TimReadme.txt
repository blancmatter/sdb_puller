=====
 TIM
=====

This package is the TTL system time library


History:

   TIM_1_00
   Ported to QNX v6.

   TIM_0_07
   Addition of function 'eTimUpdateCache', to enable the cached time to be 
   sourced other than by the real-time clock (e.g. IRIG-B).

   TIM_0_06
   Changes to 'eTimGetTime', and additional functions :-
   - eTimCacheTime   : reads the system time and makes a copy of it, and 
     ensures that all subsequent calls to 'eTimGetTime' return the stored time
     without actually reading the system time (to increase execution speed)
   - eTimUnCacheTime : reads the system time and ensures that all subsequent 
     calls to 'eTimGetTime' read the system time
   The default operation of 'eTimGetTime' is unchanged.

   TIM_0_05
   Added eTimToDouble() function.

   TIM_0_04
   Corrected 'const' argument type in eTimToString.

   TIM_0_03
   Baselined following adjustment to correctly handle negative
   durations.

   TIM_0_02

   TIM_0_01


Files:

   Tim.h                <-- RCS'd in this directory
   TimPrivate.h         <-- RCS'd in this directory
   Tim.c                <-- RCS'd in this directory
   Tim.mak              <-- RCS'd in this directory
   TimReadme.txt        <-- RCS'd in this directory
   Tim.lis              <-- RCS'd in this directory
   testprog.c           <-- RCS'd in this directory

Documentation:

   \\Files\97-001\Development\MCS Mount Control System\Detailed Design\Tim
   TIM Users Guide v0.09.doc



06-Apr-2004, MJF.

=====