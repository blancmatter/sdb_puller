=====
 TDL
=====

This package is the TTL Development Library (TDL).


History:

   TDL_1_01
   Distribute source files used to comprise library and ensure makefile will 
   build the library from the distribution. Also build the two variants of the 
   example program (TdlTx and TdlRx).

   TDL_1_00
   First baseline under QNX v6 - identical to TDL_0_01.

   TDL_0_01
   Initial baseline.


Notes:

   There is some example source provided. There is one source file, to be 
   compiled with either 'TX' or 'RX' defined to build a transmitter or receiver.
   To build a transmitter :-
      cc TdlExample.c -DTX -o Tx Tdl.lib
   To build a receiver :-
      cc TdlExample.c -DRX -o Rx Tdl.lib
   Then run 'Rx' and 'Tx' in adjacent windows.
   Note that these test programs use CIL IDs TU1 and TU2 - check that their IP 
   addresses are correct in the file Cil.map.

24-Sep-2007, MJF.

=====

