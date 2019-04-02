=====
 HTI  
=====

This is a utility to generate lists of enumerations, and a library to use them.


History:

   HTI_1_08
   General tidy-up to HtiList and HtiReport, including revision of command-line
   arguments to the latter. Addition of HtiUnits utility to process an SDB units
   file producing a readable text file. (HTI_0_21)

   HTI_1_07
   Addition of '-verbatim' switch to HtiHdr2Id to inhibit any changes made to
   label names based on inherent rules. (HTI_0_20)

   HTI_1_06
   Bugfix to prevent spurious semi-colons when not in MySQL mode.

   HTI_1_05
   Build HtiSqlList process to generate database schema for MySQL.
   Minor change to build under linux.

   HTI_1_04
   Build against static libraries.

   HTI_1_03
   Include Wfl.h in Hti private header file to pick up correct path
   for index files.

   HTI_1_02
   Tidied to aid porting to Linux - no functional changes.
  
   HTI_1_01
   Addition of 'HtiList' utility to list datum names for a process (HTI_0_18).
   Aim to open HTI tables in directory '/ram', and default to '/ttl/sw/etc' if
   not located there (HTI_0_19).

   HTI_1_00
   Ported to QNX v6.
  
   HTI_0_17
   New function to Count number of Oids assocaited with a Cil.
   New function to create a list of Oid Values and Labels.
   New function to cleanup memory allocation after values and
   labels have been created.
   
   HTI_0_16
   Fixed function names for Oid and Status code lookups.
   Changed the commandline options  for HtiReport.

   HTI_0_15
   New Function to lookup a Label associated with an OiD
   New Function to lookup a description associted with a
   status code. New function to lookup a label associted
   with a status code. New utility HtiReport that looks up
   a Description and a label given a Value. theres are 3 
   types of lookup available 'Unit', 'Oid' and 'Status'
   an 'Oid' lookup requires a cilname as well.

   HTI_0_14
   New function To lookup a Comment associated with an OiD 
   new function name: eHtiGetOiDescription

   HTI_0_13
   Fixed compiler error in HtiGenBinary.

   HTI_0_12
   Created a new utility program (HtiGenBinary) to convert the ASCII index
   tables into binary, fixed length, look-up tables. This is used by the
   HTG package.
   Reworked the look-up functions to make use of the new binary index files.

   HTI_0_11
   Pre-pend the package ID onto the following identifiers :-
   - STOP_DEMAND, STOPPED, HOME

   HTI_0_10
   The generated C source code now includes 'Htg.h' rather than 'HtiAllPkg.h'.
   Copy the utilities to the 'util' directories as part of a publish.

   HTI_0_09
   If an identifier name is 'HALT_DEMAND', or starts with 'STATE_', then 
   pre-pend the package name. Note the full set of conditions for pre-prending 
   the package name :-
   - identifier is either :-
     - STATE, AUTH_STATE, PROC_STATE, VERSION, SHUTDOWN, ENABLE, DEMAND, ACTUAL, 
        HALT_DEMAND
   - identifier starts with :-
     - STATE_

   HTI_0_08
   Fix ensuring that the first entry in the ID table may be retrieved correctly.

   HTI_0_07
   Change to the HtiHdr2Id utility, to add the TLA onto the start of the label 
   if the label is STATE, AUTH_STATE, PROC_STATE, ENABLE, DEMAND or ACTUAL, or 
   if the label contains the text VERSION or SHUTDOWN.

   HTI_0_06
   Fix to HTI library, to allow lookup of labels when the description in the 
   table is an empty string.

   HTI_0_05
   Splitting of HTI files into the utility/library code, and the table 
   generation files (now HtiTables.lis).

   HTI_0_04
   Addition to the header-to-ID utility to allow one level of compiler 
   directives.   

   HTI_0_03
   User build script HtiMakeTables changed into a makefile. Makes three 
   tables : 'IdTable', 'UnitsTable' and 'FullTable'. Enumerations added into
   the full table - not yet added ACN agents.

   HTI_0_02
   Addition of creation of 'UnitsTable', containing only the SDB units.

   HTI_0_01
   Initial version.


Files:

   HtiHdr2Id.c          <-- RCS'd in this directory
   HtiFixRecLen.c       <-- RCS'd in this directory
   Hti.c                <-- RCS'd in this directory
   HtiPrivate.h         <-- RCS'd in this directory
   Hti.h                <-- RCS'd in this directory
   Hti.mak              <-- RCS'd in this directory
   HtiReadme.txt        <-- RCS'd in this directory
   Hti.lis              <-- RCS'd in this directory


Documentation:


Notes:

HtiHdr2Id generates source code to print out the enumerations from a header 
file.
   
The program may be called with either 1, 3 or 4 arguments, if one argument is 
supplied it must have one of these values :-

   start            - generates the start of the source code
   end              - generates the end of the source code

If called with 3 arguments they are interpreted as :-

   file-name        - an input package header file
   package          - a 3 character package ID
   enumeration ID   - name of the enumeration to search for in the input file

If called with 4 arguments they are interpreted as :-

   file-name        - an input package header file
   package          - a 3 character package ID
   enumeration ID   - name of the enumeration to search for in the input file
   ifdef-to-include - name of a compilation switch to be defined

The program with then generate source code to print out the enumerated type in a
table format.

   PackageID        - a 3 character package ID 
   Enumerated Value - as a hex value prefixed with 0x
   Short Name       - a double-quote delimited string
   Description      - a double-quote delimited string

HtiFixRecLen converts a file with variable length newline terminated records to
records of fixed length. Spaces are appended to each record to set them to the 
length of the longest record. The fixed length form is used as an index file as
it makes binary search for a particular record very much easier.

Hti.lib is a library containing functions to provide access to the Header-to-ID
lookup tables.


30-Aug-2007, MJF.

=====

