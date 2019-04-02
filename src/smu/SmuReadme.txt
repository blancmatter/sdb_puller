=====
 SMU
=====

This package contains the SBD MySQL Utilities (SMU).


Notes:
- 'libmysqlclient' must be installed to link against
- ensure the path of 'libmysqlclient.a' is specified correctly in the makefile


History:

   SMU_1_02
   Link in /usr/lib/mysql/libmysqlclient.a rather than /usr/lib64/..., and libz
   rather than libgzz, plus libssl.

   SMU_1_01
   SmuRecv.c brought in line with the /Lco/sys_telemetry/trunk in Subversion.

   SMU_1_00
   First baseline as stand-alone package, functionally consistent with SDB_1_10
   but with 'Sdb' components renamed to be 'Smu', including 'SmuRecv'.


Files:

   See Smu.lis


04-Oct-2007, MJF.

=====
