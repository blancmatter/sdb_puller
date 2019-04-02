=====
 SDB
=====

This package is the System Database (SDB) and associated test processes.


Baselines:

   SDB_1_12
   Tidy-up for porting to Linux - no functional changes.

   SDB_1_11
   'SdbRecv' utility moved into separate package 'SMU' and renamed to be 
   'SmuRecv'. No functional changes.

   SDB_1_10
   Add command line options -host and -port to choose an optional host/port
   combination to send UDP packets containing each datum to an intermediate
   process that can store the data in a MySQL database. (SDB_0_69)
   Change to MySql writes from SdbStore.c to ensure all data are logged to SQL 
   immediately upon change. Latest copy of SdbRecv.c compatible with these 
   changes revision controlled here. Go to Warn state if MySql logging enabled
   but failing, e.g. unable to resolve the hostname. Addition of new units.
   (SDB_0_70)

   SDB_1_09
   Add SdbRecv process for receiving UDP packets and uploading them to a
   MySQL database.

   SDB_1_08
   Increase in number of concurrent open files to 16. (SDB_0_68)

   SDB_1_07
   Build against static libraries.

   SDB_1_06
   Code portability improved.

   SDB_1_05
   Brought in line with SDB_0_67 from QNX v4 :-
   Remove redundant checks from Heartbeat response.
   Use time caching to reduce system calls.
   Remove duplicate attempts to submit software version number.
   Improve SDB's ability to handle high levels of submit and retreival 
   requests during Operational On/Off by increasing process priority from 
   10 to 19.
   Tidy-up of logging - no functional changes.
   NB - SDB_0_65 on QNX v4 changed scheduling policy to FIFO, which has not
   yet been done on QNX v6.

   SDB_1_04
   Minor changes to aid porting to Linux - no functional changes.

   SDB_1_03
   Milliarcseconds per second units added (SDB_0_63).

   SDB_1_02
   'testlist' fixed for large datum lists (e.g. SPI).

   SDB_1_01
   Addition of new units for T2-408 Services PLC Interface (SPI).

   SDB_1_00
   Ported to QNX v6.

   SDB_0_60
   Submit SDB version number every heartbeat.

   SDB_0_59
   Addition of units for PLC System Status.

   SDB_0_58
   Fix of TTL Defect Tool Bug #49.

   SDB_0_57
   Addition of units - major/minor version packed into 16 bits.

   SDB_0_56
   Addition of units - baud rate.

   SDB_0_55
   Addition of units - millWatts per metre squared, autoguider state.

   SDB_0_54
   Addition of an additional unit for inclinometer calibration 
   (milli-degrees per volt).

   SDB_0_53
   Addition of units for PLC device types. Removal of faulty debug message at
   change of hour. Addition of functionality into 'testlist' to decode and 
   display new SPT data with the '-names' switch.

   SDB_0_52
   Addition of two command-line parameters. The '-datapath' allows the path for 
   datafiles to be altered from the default. The '-cleanup' flag allows the 
   specification of the number of days after which database files are removed.

   SDB_0_51
   Minor changes to testdump and testlist.

   SDB_0_50
   Added new unit "E_SDB_UAS_UNTIS" for representing microarcseconds.

   SDB_0_49
   The 'testlist' and 'testutil' utilities now run at priority 9 (not 10). Some
   units added.
   
   SDB_0_48
   Changed the pathname of the Sfr to /opt/ttl/bin from ./

   SDB_0_47
   Addition of new SDB units : telescope, litres, milli-litres and mlitres/min.
   The SDB now ensures that the last un-archived submission is flushed to disk,
   even if the datum value is the same as the already-archived submission, in 
   the following cases :-
   a) the SDB is taken to safe-state
   b) a datum is cleared from the SDB
   c) the relevant SDB data file is closed

   SDB_0_46
   Addition of new SDB units - package ID and IET data state.

   SDB_0_45
   Addition of new SDB units - EPT and SPT data states.

   SDB_0_44
   Added new units to the enumerated list in Sdb.h.

   SDB_0_43
   Addition of a 'RETRIEVE_L' service for retrieving data from disk. This works
   in a similar manner to the 'RETRIEVE_F' service, except if more than the 
   specified number of measurements exist between the two time-stamps, then the
   last 'n' entries (rather than the first 'n') are retrieved.
   The 'testlist' utility now features an additional command-line option to 
   additionally retrieve the last 'n' measurements for each datum, in the form
   '-lastdata [n]'.

   SDB_0_42
   Addition of a 'CLEAR_1' service, which will allow the originator of the 
   message to selectively clear their own datum entries. The message takes the 
   same form as the 'CLEAR_S' service, but the data to clear are specified 
   rather than the data to be perserved.

   SDB_0_41
   Increased the maximum number of SDB files that may be open simultaneously.

   SDB_0_40
   Added a new service that allows a process to clear out all entries
   related to a particular "SourceId". It also allows the specification
   of an "exceptions" list of "DatumIDs".
   The creation of files was also changed to remove "world-write" permission.

   SDB_0_39
   No functional changes to the SDB itself, but utilities 'testlist' and
   'testdump' improved so that a hexadecimal datum may be specified on the
   command-line.

   SDB_0_38
   New unit type of 'UNSUPPORTED_UNITS' added where a datum is not valid in a
   particular context. SDB files now created with read/write access of 'user',
   'group' and 'other'.  Note that this will need reverting back to 'user' and
   'group' only for production.

   SDB_0_37
   Inclinometer units changed to be milli-degrees.

   SDB_0_36
   Removed fflush() functions from data writing in order to increase the 
   performance of the system.


Files:

   Sdb.c                <-- RCS'd in this directory
   SdbAutoSubmit.c      <-- RCS'd in this directory
   SdbByteOrder.c       <-- RCS'd in this directory
   SdbCleanup.c         <-- RCS'd in this directory
   SdbClear.c           <-- RCS'd in this directory
   SdbCode.c            <-- RCS'd in this directory
   SdbCount.c           <-- RCS'd in this directory
   SdbFileRetr.c        <-- RCS'd in this directory
   SdbHash.c            <-- RCS'd in this directory
   SdbHeartbeat.c       <-- RCS'd in this directory
   SdbList.c            <-- RCS'd in this directory
   SdbLnkLst.c          <-- RCS'd in this directory
   SdbMulRetr.c         <-- RCS'd in this directory
   SdbProcess.c         <-- RCS'd in this directory
   SdbReply.c           <-- RCS'd in this directory
   SdbReport.c          <-- RCS'd in this directory
   SdbRetrieve.c        <-- RCS'd in this directory
   SdbSetup.c           <-- RCS'd in this directory
   SdbState.c           <-- RCS'd in this directory
   SdbStore.c           <-- RCS'd in this directory
   SdbSubmit.c          <-- RCS'd in this directory
   SdbUnits.c           <-- RCS'd in this directory
   Sdb.h                <-- RCS'd in this directory
   SdbPrivate.h         <-- RCS'd in this directory

   Sdb.mak              <-- RCS'd in this directory
   SdbReadme.txt        <-- RCS'd in this directory
   Sdb.lis              <-- RCS'd in this directory
   SdbPseudoCode.txt    <-- RCS'd in this directory

   testclient.c         <-- RCS'd in this directory
   testcount.c          <-- RCS'd in this directory
   testdump.c           <-- RCS'd in this directory
   testfilereq.c        <-- RCS'd in this directory
   testflood.c          <-- RCS'd in this directory
   testinject.c         <-- RCS'd in this directory
   testlist.c           <-- RCS'd in this directory
   testmulreq.c         <-- RCS'd in this directory
   teststore.c          <-- RCS'd in this directory


Documentation:

-------
 NOTES
-------

The SdbPseudoCode.txt file is some preliminary design notes for the final SDB
system. It is a single file, containing pseudo-code (C) for identifying and
experimenting with potential code layout. This file has been RCS'd. The 
comment block at the start of the file contains a first pass at a state 
transition diagram for the program, as well.


-------------------
 STATE TRANSITIONS
-------------------


                      +-------+
       ,--------------|  OFF  |<---(8)
       |              +-------+
       |                  ^
       |                  |
      (1)                (7)
       |                  |
       V                  |
   +-------+          +-------+
   | INIT  |---(2)--->| FAULT |
   +-------+          +-------+
       |                  ^
       |                  |
      (3)                (6)
       |                  |
       |                  |  
       |              +-------+          +------+
       `------------->| OKAY  |---(9)--->| SAFE |
                      +-------+          +------+
                        |   ^
                        |   |
                       (4) (5)
                        |   |
                        V   |
                      +-------+
                      | WARN  |
                      +-------+


 Description of states:

   OFF: The task is not running.
  INIT: The task is performing its initial start-up.
  OKAY: The task is running and messages are being dealt with.
  WARN: The task is running but has not received a message recently.
        It will automatically recover on the receipt of any message.
 FAULT: The task is running, but there is some problem that the
        task is unable to deal with.
  SAFE: The database is in a state suitable for termination.

 Description of transitions:

 1. The task is started.
 2. The initialisation failed.
 3. The task has completed initialisation and is ready to receive messages.
 4. No messages have been received in a certain time.
 5. A message has been received.
 6. An error occurred.
 7. The fault could not be recovered, so the task is aborting.
 8. (FROM ANY STATE), the task crashes or is terminated at a system level.
 9. A request has been made for the SDB to shutdown.

-------
 USAGE
-------

Usage sdb [options]
Options:
              (No option) Normal program operation.
 -help        Print a list of available command line options and exit.
 -quiet       Suppress output (opposite of -verbose).
 -tu#         Use CIL test unit ID TU# (where #=0-9) instead of SDB.
 -cil NAME    Use CIL ID (specified by case-sensitive NAME).
 -verbose     Continuously print task info to console (default).
 -nosyslog    Do not submit message to syslog network log
 -syslog      Send messages to syslog network based log file

The purpose of the -tu# and -cil switches is to make the SDB use a 
different CIL address.


----------------
 VERBOSE OUTPUT
----------------

The following statistics about the program are printed during verbose output 
operating mode.

 State     The process state (as enumerated in TtlSystem.h)
 Dat       "Data" - The number of different data definitions currently stored.
 Tot       "Total" - The total number of data (types & times) stored.
 Hld       "History Limited Data" - The number of data definitions where new 
           data forces the discard of the oldest point of that data 
           definition.
 Sub       "Submitted" - The number of successful data submissions that have
           been made since the SDB was started.
 Rtr       "Retrieved" - The total number of data that have been retrieved
           since the SDB was started.
 Htb       "Heartbeats" - The number of heartbeat polls received.
 Err       "Errors" - The number of comms errors that have occurred.


------------
 TESTINJECT
------------

This program is used to insert data into the SDB. It is not particularly 
efficient, nor very neat, but it will allow for testing of unusual data
values, which is what is needed. The basic operation is just to run it,
and answer the questions that it asks. These will be to enter the following
parameters:

   The source ID
   The datum ID
   The raw value
   The units code (1 = none)

All values should be entered in decimal form. For the units, the value of 1
is needed if no units are applicable. The value of zero is reserved for 
error trapping (invalid units), and thus should not be used. Data is 
time-stamped with the current system time.

After entering the data, the program will attempt to send the data. It will
print the packet content, and then send it. Following this, it'll wait for
the reply message. Note that in interactive mode (with the user typing in
each entry at run-time), multiple heartbeat messages may stack up. This may
result in multiple "Receiving data..." messages as they are cleared. This is
normal, although if left for a _really_ long time, it'll probably overflow
some UDP stack limit.

Having sent the data, it will ask if the user wants to send another point.
The user must type "y" or "n" in response.

It is possible to enter data from a file. This is much more useful, especially
if there is a large collection of data to add. The way this is done is by
piping the data from a file into stdin of the testinject program. Because 
scanf() is used to accept the data, the entries only need to be white-space
separated, and thus may be arranged into single lines for readability. The
file must not have any comment characters in it. There is no provision for
such at this stage. Also, do ensure that the file ends with a carriage return.
And example file, might look like this:

   37   1   -27   1   y
   37   2   512   1   y
   37   3    19   4   y
   38   1     0   1   n

Note the "y/n" at the end of each line. The columns are in the same order as
the questions asked by the program. This file may be run into the testinject
program using something like this:

   $ testinject < data.file

The only command line options are -tu# (where # is a number from 0 to 9) to
select the CIL address to use.

NB: The program injects the data, but the only way to remove an entry is to 
restart the SDB.

------------
 REFERENCES
------------

- Computing Architectural Design (CAD, v0.51, 21-Jan-2000).
- SDB Design & Implementation (v0.04, 15-May-2000).
- SDB Interface Control Document(v0.6 23-May-2000).

---------
 UPDATED
---------

04-Oct-2007, MJF.

=====
