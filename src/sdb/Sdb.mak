##
## File name:
##    Sdb.mak
##
## Purpose:
##    To compile the Status Database (SDB) software.
##
## Description:
##    ...
##
## Authors:
##    djm: Derek J. McKay (TTL)
##    mjf: Martyn J. Ford (TTL)
##
## Version:
##    $Id: Sdb.mak,v 0.48 2007/09/14 15:55:00 mjf Exp $
##
## History:
##    $Log: Sdb.mak,v $
##    Revision 0.48  2007/09/14 15:55:00  mjf
##    SdbRecv moved into separate package 'SMU'.
##
##    Revision 0.47  2007/07/17 10:20:02  mjf
##    Added functionality to pass raw UDP packets from Sdb
##    to a MySql database.
##
##    Revision 0.46  2007/03/12 15:37:41  sxf
##    Correct compile options for SdbRecv
##
##    Revision 0.45  2007/03/07 11:40:49  sxf
##    Added build for SdbRecv process on linux.
##
##    Revision 0.44  2006/10/03 10:47:10  sxf
##    Baseline SDB_1_07.
##
##    Revision 0.43  2005/06/20 13:35:31  mjf
##    Minor changes to aid porting to Linux - no functional changes.
##
##    Revision 0.42  2004/01/16 08:55:04  sxf
##    Minor changes to port to QNX6.
##
##    Revision 0.41  2002/04/18 14:49:36  djm
##    Added the HTI library to the testdump utility.
##
##    Revision 0.40  2001/09/14 11:08:01  djm
##    Added build rules for SdbClear.c
##
##    Revision 0.39  2001/02/20 14:29:46  mjf
##    Publish testlist, testdump and testserver into the 'util' directory.
##
##    Revision 0.38  2001/02/14 13:53:02  mjf
##    Use HTI library for Header-to-ID conversions in testlist and testinject.
##
##    Revision 0.37  2001/01/25 17:26:42  mjf
##    Don't copy test programs in publish rule.
##
##    Revision 0.36  2001/01/25 17:22:23  mjf
##    Addition of publish rule.
##
##    Revision 0.35  2001/01/23 15:06:09  mjf
##    Link in CLU and LOG libraries with test programs.
##
##    Revision 0.34  2001/01/18 13:52:44  mjf
##    Link in CFU with test applications where CIL is used.
##
##    Revision 0.33  2001/01/03 11:41:24  mjf
##    Also link in CFU library to pre-empt pending LOG library change.
##
##    Revision 0.32  2000/11/08 13:50:35  mjf
##    Import tool declarations from default labels.
##
##    Revision 0.31  2000/11/08 12:14:23  mjf
##    Makefile enhanced to use default labels. Supports both QNX and Linux.
##
##    Revision 0.30  2000/10/09 14:49:53  mjf
##    Force removal of files on a clean.
##
##    Revision 0.29  2000/10/09 12:54:12  mjf
##    Replacing of dependency 'Makefile' with 'Sdb.mak'. Link with system
##    libraries rather than system objects.
##
##    Revision 0.28  2000/10/09 10:46:36  mjf
##    Renaming of Makefile to be Sdb.h. Include the files Sdb.h and
##    SdbPrivate.h from local directory.
##
##    Revision 0.27  2000/09/27 11:53:18  djm
##    Added build rule for the SdbUnits.c module.
##
##    Revision 0.26  2000/09/20 14:21:52  djm
##    Added build rules for testflood (SDB test program).
##
##    Revision 0.25  2000/08/30 11:11:48  djm
##    Minor corrections for command line parsing. Fixed incorrect listing
##    of external object files in OBJS list.
##
##    Revision 0.24  2000/08/24 11:48:13  djm
##    Reorganised flags. Added the build procedures for Sdb.lib.
##
##    Revision 0.23  2000/08/22 16:26:27  djm
##    Added build rules for testinject.
##
##    Revision 0.22  2000/08/17 10:28:05  djm
##    Removed build rules for SFR (now in its own development area).
##
##    Revision 0.21  2000/08/07 15:54:52  djm
##    Changed some nomenclature and corrected some non-standard tabs.
##
##    Revision 0.20  2000/07/21 14:43:44  djm
##    Added strict-ANSI checking for all modules other than the
##    process spawning module. Added some build rules for the
##    file storage recovery task.
##
##    Revision 0.19  2000/07/14 16:31:46  djm
##    Added testdump program and a number of changes to improve
##    building of other modules (removed unecessary linking, etc.).
##
##    Revision 0.18  2000/07/13 11:45:19  djm
##    Added build rule for SdbCode.c.
##
##    Revision 0.17  2000/07/10 16:55:31  djm
##    Added storage file build rules.
##
##    Revision 0.16  2000/06/26 14:35:29  djm
##    Added build rules for multiple-request modules and test programs.
##
##    Revision 0.15  2000/06/23 10:32:40  djm
##    Removed obsolete build rules for SdbByteOrder.c.
##
##    Revision 0.14  2000/06/22 14:00:11  djm
##    Added build rules for SdbByteOrder.c.
##
##    Revision 0.13  2000/06/22 07:57:18  djm
##    Added build rules for SdbCount.c and testcount.c
##
##    Revision 0.12  2000/06/07 11:50:46  djm
##    Included Log.o in object list with which to be linked.
##
##    Revision 0.11  2000/05/31 10:48:44  djm
##    Added links to Tim.o.
##
##    Revision 0.10  2000/05/31 08:47:51  djm
##    Added build rules for SdbLnkLst.c.
##
##    Revision 0.9  2000/05/21 15:27:49  djm
##    Added build rules for testlist.c.
##
##    Revision 0.8  2000/05/21 09:15:44  djm
##    Added build rules for SdbList.c.
##
##    Revision 0.7  2000/05/18 14:40:47  djm
##    Added build rules for SdbAutoSubmit.c.
##
##    Revision 0.6  2000/05/18 10:17:10  djm
##    Altered search path for Cil.o.
##
##    Revision 0.5  2000/05/17 15:03:30  djm
##    Added build rules for SdbReply.c
##
##    Revision 0.4  2000/05/17 09:37:56  djm
##    Added build rules for SdbRetrieve.c
##
##    Revision 0.3  2000/05/12 08:12:00  djm
##    Added build rules for SdbHash.c and SdbSubmit.c.
##
##    Revision 0.2  2000/05/09 11:13:41  djm
##    Added SdbHeartbeat and testclient modules to build procedure.
##
##    Revision 0.1  2000/05/09 08:18:24  djm
##    Initial creation.
##
##
##


# Include default labels for build

include ../TtlLabels.mak


# Declare phony targets

.PHONY : all
.PHONY : clean
.PHONY : publish


# List of object files.
OBJS =	Sdb.o \
		SdbAutoSubmit.o \
		SdbCleanup.o \
		SdbClear.o \
		SdbCount.o \
		SdbFileRetr.o \
		SdbHash.o \
		SdbHeartbeat.o \
		SdbList.o \
		SdbLnkLst.o \
		SdbMulRetr.o \
		SdbProcess.o \
		SdbReply.o \
		SdbReport.o \
		SdbRetrieve.o \
		SdbSetup.o \
		SdbState.o \
		SdbStore.o \
		SdbSubmit.o \
		SdbUnits.o

# List of libraries
LIBS =	$(TTL_LIB)/Clu.lib \
		Sdb.lib \
		$(TTL_LIB)/Cil.lib \
		$(TTL_LIB)/Log.lib \
		$(TTL_LIB)/Tim.lib \
		$(TTL_LIB)/Cfu.lib

# List of include files.
INCS =	Sdb.h \
		SdbPrivate.h \
		$(TTL_INCLUDE)/Cil.h \
		$(TTL_INCLUDE)/TtlSystem.h

# Build rules (general).

all:	Sdb.lib \
		Sdb \
		testclient testcount testdump testfilereq testflood \
		testinject testlist testmulreq teststore

clean:
	$(RM) $(OBJS)
	$(RM) Sdb
	$(RM) testclient testcount testdump testfilereq testflood
	$(RM) testinject testlist testmulreq teststore
	$(RM) Sdb.lib


# Executable rules.

Sdb:	Sdb.mak $(OBJS) $(LIBS)
	$(LN) -o Sdb $(OBJS) $(LIBS)  $(LN_OPT)

testclient:	Sdb.mak testclient.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib
	$(LN) -o testclient testclient.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(LN_OPT)

testcount:	Sdb.mak testcount.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib
	$(LN) -o testcount testcount.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(LN_OPT)

testdump:	Sdb.mak testdump.o SdbCode.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Hti.lib
	$(LN) -o testdump testdump.o SdbCode.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Hti.lib $(LN_OPT)

testfilereq:	Sdb.mak testfilereq.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib
	$(LN) -o testfilereq testfilereq.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(LN_OPT)

testflood:	Sdb.mak testflood.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib
	$(LN) -o testflood testflood.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(LN_OPT)

testinject:	Sdb.mak testinject.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Hti.lib
	$(LN) -o testinject testinject.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Hti.lib $(LN_OPT)

testlist:	Sdb.mak testlist.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Hti.lib
	$(LN) -o testlist testlist.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Hti.lib  $(LN_OPT)

testmulreq:	Sdb.mak testmulreq.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib
	$(LN) -o testmulreq testmulreq.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(LN_OPT)

teststore:	Sdb.mak teststore.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib
	$(LN) -o teststore teststore.o $(TTL_LIB)/Cil.lib $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib $(LN_OPT)


# Library build rules

Sdb.lib:	Sdb.mak SdbCode.o
	$(LB) $(LB_OPT) $@ $(LB_DIV) SdbCode.o


# Source code rules (in alphabetical order).

Sdb.o:	Sdb.mak $(INCS) Sdb.c
	$(CC) $(CC_OPT) Sdb.c

SdbAutoSubmit.o:	Sdb.mak $(INCS) SdbAutoSubmit.c
	$(CC) $(CC_OPT) SdbAutoSubmit.c

SdbCleanup.o:	Sdb.mak $(INCS) SdbCleanup.c
	$(CC) $(CC_OPT) SdbCleanup.c

SdbClear.o:	Sdb.mak $(INCS) SdbClear.c
	$(CC) $(CC_OPT) SdbClear.c

SdbCode.o:	Sdb.mak $(INCS) SdbCode.c
	$(CC) $(CC_OPT) SdbCode.c

SdbCount.o:	Sdb.mak $(INCS) SdbCount.c
	$(CC) $(CC_OPT) SdbCount.c

# --- Note the non-ANSI arguments in the following file.
SdbFileRetr.o:	Sdb.mak $(INCS) SdbFileRetr.c
	$(CC) $(CC_OPT_NON_ANSI) SdbFileRetr.c

SdbHash.o:	Sdb.mak $(INCS) SdbHash.c
	$(CC) $(CC_OPT) SdbHash.c

SdbHeartbeat.o:	Sdb.mak $(INCS) SdbHeartbeat.c
	$(CC) $(CC_OPT) SdbHeartbeat.c

SdbList.o:	Sdb.mak $(INCS) SdbList.c
	$(CC) $(CC_OPT) SdbList.c

SdbLnkLst.o:	Sdb.mak $(INCS) SdbLnkLst.c
	$(CC) $(CC_OPT) SdbLnkLst.c

SdbMulRetr.o:	Sdb.mak $(INCS) SdbMulRetr.c
	$(CC) $(CC_OPT) SdbMulRetr.c

SdbProcess.o:	Sdb.mak $(INCS) SdbProcess.c
	$(CC) $(CC_OPT) SdbProcess.c

SdbReply.o:	Sdb.mak $(INCS) SdbReply.c
	$(CC) $(CC_OPT) SdbReply.c

SdbReport.o:	Sdb.mak $(INCS) SdbReport.c
	$(CC) $(CC_OPT) SdbReport.c

SdbRetrieve.o:	Sdb.mak $(INCS) SdbRetrieve.c
	$(CC) $(CC_OPT) SdbRetrieve.c

SdbSetup.o:	Sdb.mak $(INCS) SdbSetup.c
	$(CC) $(CC_OPT) SdbSetup.c

SdbState.o:	Sdb.mak $(INCS) SdbState.c
	$(CC) $(CC_OPT) SdbState.c

SdbStore.o:	Sdb.mak $(INCS) SdbStore.c
	$(CC) $(CC_OPT) SdbStore.c

SdbSubmit.o:	Sdb.mak $(INCS) SdbSubmit.c
	$(CC) $(CC_OPT) SdbSubmit.c

SdbUnits.o:	Sdb.mak $(INCS) SdbUnits.c
	$(CC) $(CC_OPT) SdbUnits.c


testclient.o:	Sdb.mak Sdb.h testclient.c
	$(CC) $(CC_OPT) testclient.c

testcount.o:	Sdb.mak Sdb.h testcount.c
	$(CC) $(CC_OPT) testcount.c

testdump.o:	Sdb.mak Sdb.h SdbPrivate.h testdump.c
	$(CC) $(CC_OPT) testdump.c

testfilereq.o:	Sdb.mak Sdb.h testfilereq.c
	$(CC) $(CC_OPT) testfilereq.c

testflood.o:	Sdb.mak Sdb.h testflood.c
	$(CC) $(CC_OPT) testflood.c

testlist.o:	Sdb.mak Sdb.h testlist.c
	$(CC) $(CC_OPT) testlist.c

testinject.o:	Sdb.mak Sdb.h testinject.c
	$(CC) $(CC_OPT) testinject.c

testmulreq.o:	Sdb.mak Sdb.h testmulreq.c
	$(CC) $(CC_OPT) testmulreq.c

teststore.o:	Sdb.mak Sdb.h teststore.c
	$(CC) $(CC_OPT) teststore.c

# Publish rule

publish :
	  $(CP) Sdb        $(TTL_BIN)
	  $(CP) Sdb.h      $(TTL_INCLUDE)
	  $(CP) Sdb.lib    $(TTL_LIB)
	  $(CP) testlist   $(TTL_UTIL)
	  $(CP) testdump   $(TTL_UTIL)
	  $(CP) testinject $(TTL_UTIL)



## EOF

