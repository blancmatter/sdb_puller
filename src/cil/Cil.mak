##
## File name:
##    Cil.mak
##
## Purpose:
##    To build the Communications Interface Library (CIL) code.
##
## Description:
##    ...
##
## Authors:
##    plw: Paul L. Wright (TTL)
##    djm: Derek J. McKay (TTL)
##    mjf: Martyn J. Ford (TTL)
##
## Version:
##    $Id: Cil.mak,v 1.23 2006/10/02 10:39:57 sxf Exp $
##
## History:
##    $Log: Cil.mak,v $
##    Revision 1.23  2006/10/02 10:39:57  sxf
##    Build against static libraries.
##
##    Revision 1.22  2005/06/20 09:50:44  mjf
##    Removal of dependency on Wfl.lib.
##
##    Revision 1.21  2004/06/10 15:59:18  mjf
##    No need for Rep.lib.
##
##    Revision 1.20  2004/06/10 14:23:38  mjf
##    No need for TFP library.
##
##    Revision 1.19  2004/05/19 15:29:11  mkb
##    Add support for ClockCycle call.
##
##    Revision 1.18  2001/03/05 10:31:45  mjf
##    Copy testclient and testserver to the 'util' directory as part of the
##    publish rule.
##
##    Revision 1.17  2001/03/02 16:39:05  mjf
##    CIL library now consists of two libraries.
##
##    Revision 1.16  2001/01/25 16:52:28  mjf
##    Addition of publish rule.
##
##    Revision 1.15  2001/01/23 15:06:59  mjf
##    Link in CLU and LOG with test programs.
##
##    Revision 1.14  2001/01/17 10:12:23  mjf
##    Changed to use CFU for CIL map parsing.
##
##    Revision 1.13  2000/12/04 11:25:15  mjf
##    Link test programs with Tim.lib for time-stamp in eSclSend.
##
##    Revision 1.12  2000/11/08 13:41:58  mjf
##    Import tool declarations from default labels.
##
##    Revision 1.11  2000/11/08 11:16:33  mjf
##    Makefile enhanced to use default labels. Supports both QNX and Linux.
##
##    Revision 1.10  2000/10/26 15:59:25  djm
##    Minor changes for porting to Linux
##
##    Revision 1.9  2000/10/09 14:35:42  mjf
##    Force removal of files on a clean.
##
##    Revision 1.8  2000/10/09 11:36:25  mjf
##    Removal of 'Makefile' as a dependency - replaced with 'Cil.mak',
##
##    Revision 1.7  2000/10/09 08:16:17  mjf
##    Include header files from local directory, and build an output library.
##
##    Revision 1.6  2000/10/06 16:00:32  mjf
##    Makefile renamed to be 'Cil.mak'.
##
##    Revision 1.5  2000/07/14 16:37:18  djm
##    Major overhaul to Makefile.
##    Removed a number of obsolete build rules, added definitions for
##    Linux/GNU-C builds, reorganised file layout, fixed some incorrect
##    or missing dependancies, etc.
##
##
##    02-Jun-2000 djm  Added build rules for testraw.c.
##    27-Mar-2000 djm  Build rules for testbroker.c added.
##                     A few additional formatting changes made.
##    29-Feb-2000 plw  Version used to test new CIL
##    10-Feb-2000 plw  Initial revision
##


# Include default labels for build

include ../TtlLabels.mak


# Declare phony targets

.PHONY : all
.PHONY : clean
.PHONY : publish


# List of object files.
OBJS = Cil.o CilUtil.o


# List of include files.
INCS =	Cil.h \
		CilPrivate \
		$(TTL_INCLUDE)/TtlSystem.h

LIBS =     $(TTL_LIB)/Log.lib \
           $(TTL_LIB)/Clu.lib \
           $(TTL_LIB)/Tim.lib \
           $(TTL_LIB)/Cfu.lib

# Build rules (general).

all:	Cil.lib testbroker testclient testraw testserver

clean:
	$(RM) $(OBJS)
	$(RM) Cil.lib
	$(RM) testbroker testclient testraw testserver


# Executable rules.

BROKEROBJS = Cil.o CilUtil.o testbroker.o
CLIENTOBJS = Cil.o CilUtil.o testclient.o
RAWOBJS    = Cil.o CilUtil.o testraw.o
SERVEROBJS = Cil.o CilUtil.o testserver.o


testbroker:		Cil.mak $(BROKEROBJS) $(LIB)
	$(LN)  -o $@ $(BROKEROBJS) $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Tim.lib $(LN_OPT)


tester:			Cil.mak $(TESTEROBJS) $(LIB)
	$(LN) -o $@ $(TESTEROBJS) $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Tim.lib $(LN_OPT)

testraw:		Cil.mak $(RAWOBJS) $(LIB)
	$(LN) -o $@ $(RAWOBJS) $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Tim.lib $(LN_OPT)

testserver:		Cil.mak $(SERVEROBJS) $(LIB)
	$(LN) -o $@ $(SERVEROBJS) $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Tim.lib $(LN_OPT)

testclient:		Cil.mak $(CLIENTOBJS) $(LIB)
	$(LN) -o $@ $(CLIENTOBJS) $(TTL_LIB)/Clu.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Tim.lib $(LN_OPT)

# Source code rules (in alphabetical order).

#Cil.o:	Cil.mak $(INCS) Cil.c
#	$(CC) $(CC_OPT) Cil.c

.c.o:
	$(CC) $(CC_OPT_NON_ANSI) $<


# Rule to build library

Cil.lib : Cil.mak $(OBJS) $(LIBS)
	$(LB) $(LB_OPT) $@ $(LB_DIV) $(OBJS)


Cil.o:			$(TTL_INCLUDE)/TtlSystem.h Cil.h CilPrivate.h Cil.mak
CilUtil.o:		$(TTL_INCLUDE)/TtlSystem.h Cil.h CilPrivate.h Cil.mak

testbroker.o:	$(TTL_INCLUDE)/TtlSystem.h Cil.h Cil.mak
testclient.o:	$(TTL_INCLUDE)/TtlSystem.h Cil.h Cil.mak
testserver.o:	$(TTL_INCLUDE)/TtlSystem.h Cil.h Cil.mak
testraw.o:		$(TTL_INCLUDE)/TtlSystem.h Cil.h testraw.c Cil.mak


# Publish rule

publish :
	  $(CP) Cil.lib $(TTL_LIB)
	  $(CP) Cil.h   $(TTL_INCLUDE)
	  $(CP) testserver $(TTL_UTIL)
	  $(CP) testclient $(TTL_UTIL)



## EOF

