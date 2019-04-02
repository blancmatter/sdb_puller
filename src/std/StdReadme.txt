=====
 STD
=====

   This package is the Sdb Testdump Device, used to extract
   source and datum pairs from the Sdb within a given time
   range. 


History:

   STD_1_13
   Build against static libraries.

   STD_1_12
   Link against system libz.a rather than home grown one.

   STD_1_11
   Ported to Cygwin.

   STD_1_10
   Ensure data retrieved so far is flushed to the output file if the retrieval
   encounters an error and can't proceed. Order of source/datum names swapped 
   in header of output file.

   STD_1_09
   Allow Std to read gzipped Sdb files (i.e. with extension .sdb.gz)
   using the zlib compression library (see http://www.zlib.net/).
   
   STD_1_08
   Simplification of start-of-hour calculation to aid portability.

   STD_1_07
   Set TZ environment variable to zero to ensure calls to time manipulation 
   functions are not affected by daylight saving.

   STD_1_06
   Addition of command-line switch to specify a 'stride' in seconds to ensure 
   extracted measurements are a specified distance apart in time.

   STD_1_05
   Tidied up to aid porting to Linux. No functional changes.

   STD_1_04
   Bug fixes in STD library functions:
   - Improved performance over an hour boundary.
   - Fixed certain exit conditions that could cause failure on subsequent invocation.
   (STD_0_13)
   Column added to output file (third column) to represent time in the form of
   seconds-elapsed-since-1970 to aid the production of quick plots. (STD_0_14)
   Added -matlab command line option that writes the first (header) row 
   as a comment and removes the first two (date & time) columns for 
   export to matlab. Also writes NaN for non existent data.
   Added -gnuplot command line option that writes the first (header) row 
   as a comment for export to gnuplot. Also writes NaN for non existent data.
   (STD_0_15).

   STD_1_03
   Reset FirstTime flag within public library when end of
   search has been reached to allow clients to start a new
   search.
   
   STD_1_02
   Fix problems that make STD enter infinte loops during some
   searches.
   Put search results into linked list so they can be sorted
   chronologically because the SBD does not guarantee the 
   order of records in *.sdb files.

   STD_1_01
   Correct default path for QNX6 and fix bug.

   STD_1_00
   Ported to QNX v6, functionality identical to STD_0_08.

   STD_0_08
   Don't attempt to create a log file by default.

   STD_0_07
   Included a command line switch to allow an alternate
   data path to be specified. Also corrected a few bugs
   which neglected the last hour of data.

   STD_0_06
   Allow up to 100 data items to be extracted at once.
   Allow datum number to be used rather than name.

   STD_0_05
   Minor changes. Tidy-up for full software baseline.

   STD_0_04
   Turned std functions into library calls to allow other
   software to use Std functions.

   STD_0_03
   Updated baseline before attempting to modify code to 
   provide Std library functions.

   STD_0_02
   General tidy up. Included functions to read chunks of 
   Sdb data at a time, making data retrieval more efficient.

   STD_0_01
   First baseline using the TTL package name 'STD'. 

Files:

  StdReadme.txt
  StdMain.c
  StdSdbRead.c
  StdInit.c
  Std.mak
  Std.lis
  Std.cfg
  StdPrivate.h

Documentation:

   ???


Notes:

   Utility to search and extract source and datum pairs,
   specified in the configuration file, from the Sdb.


12-Jul-2006, MJF.

=====

