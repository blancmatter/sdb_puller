=====
 HTG
=====

HTI Tables Generation - these are the files to use the HTI utilities to 
generate lists of enumerations.


History:

   HTG_1_05
   Addition of process states for on-mount sub-systems. (HTG_0_17)
   New 'EnumTable' created containing the definitions of enumerated data 
   reported to the SDB. (HTG_0_18)

   HTG_1_04
   './' qualifier added to run utilities in current directory.

   HTG_1_03
   Added VEN Encoder Non-Linearity and Linear Encoder states (HTG_0_16).

   HTG_1_02
   Addition of Nasmyth+, Nasmyth- and Prime Focus.

   HTG_1_01
   Addition of SPT commands new to T2-408 SPLC.

   HTG_1_00
   Ported to QNX v6, and AMT datum IDs associated with the 'T' rather than 'R'.

   HTG_0_15
   Added PLC System Status enumeration.

   HTG_0_14
   Added Status table generation this is a work in progress.

   HTG_0_13
   Modified the makefile to make use of the HtiGenBinary utility to
   generate binary files of the tables. These binary files are used
   for rapid lookup by the HTI library functions.

   HTG_0_12
   Addition of entries for autoguider, and correction of a few typos.

   HTG_0_11
   Addition of entries for EPT, SPT and AMT.

   HTG_0_10
   Addition of support for MODbus server.

   HTG_0_09
   Addition of data IDs for node agents, and the CAN server added into the 
   mirror-support sub-system.

   HTG_0_08
   Following large-scale naming changes to public header files in pursuit of
   conformance. Also, Primary Mirror Support sub-system now has its own 
   independent reporter 'MSR'.


Files:

   Htg.h         
   Htg.mak       
   HtgReadme.txt 
   Htg.lis       


Documentation:


Notes:

Htg.mak is a makefile to automatically generate three lookup table pairs :-

   IdTable          - Table containing only the SDB datum ID enumerations.
   UnitsTable       - Table containing only the SDB unit enumerations.
   FullTable        - Table containing all enumerations.

For each of these, there is a ".idx" file, which is a fixed-length, ASCII,
human readable file. There is also an ".idb" file, which is a binary file.
The latter of these has been generated to allow extremely fast look-up of
entries.

The relevant public header files must all be included by the file Htg.h.


18-Jul-2007, MJF.


=====

