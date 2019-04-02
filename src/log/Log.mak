##
## File name:
##    Log.mak
##
## Purpose:
##    To compile the System logging (LOG) code.
##
## Description:
##    ...
##
## Authors:
##    djm: Derek J. McKay (TTL)
##    mjf: Martyn J. Ford (TTL)
##
## Version:
##    $Id: Log.mak,v 0.14 2002/09/10 12:52:11 sxf Exp $
##
## History:
##    $Log: Log.mak,v $
##    Revision 0.14  2002/09/10 12:52:11  sxf
##    Added LogSnprintf.c and LogSnprintf.h.
##
##    Revision 0.13  2001/02/06 10:31:33  mjf
##    Removal of libraries as a dependency for Log.lib.
##
##    Revision 0.12  2001/02/06 09:55:08  mjf
##    Don't make 'testlog' by default. Call make with the 'test' target to
##    do this. This is because 'testlog' requires the libraries 'Cil' and
##    'Cfu' - when performing a fresh system build 'Cil' will not be built
##    before 'Log'.
##
##    Revision 0.11  2001/01/25 16:54:48  mjf
##    Addition of publish rule.
##
##    Revision 0.10  2000/12/18 11:12:40  mjf
##    Link in CFU library with the application - used by eLogSetup.
##
##    Revision 0.9  2000/11/08 13:45:14  mjf
##    Import tool declarations from default labels.
##
##    Revision 0.8  2000/11/08 11:43:33  mjf
##    Makefile enhanced to used default labels. Supports both QNX and Linux.
##
##    Revision 0.7  2000/10/26 16:26:14  djm
##    Added flags for Linux compilation.
##
##    Revision 0.6  2000/10/09 14:38:32  mjf
##    Force removal of files on a clean.
##
##    Revision 0.5  2000/10/09 12:27:14  mjf
##    Make 'Log.mak' a dependency rather than 'Makefile. Link in CIL
##    library rather the object.
##
##    Revision 0.4  2000/10/09 08:40:59  mjf
##    Renaming of Makefile to be Log.mak. Output from makefile produces a
##    library. Include header files from local directory.
##
##    Revision 0.3  2000/09/19 08:15:06  mjf
##    Do not delete system objects during a 'clean'.
##
##    Revision 0.2  2000/08/11 16:19:20  djm
##    Fixed typo in L_PATH.
##
##    Revision 0.1  2000/06/05 11:32:52  djm
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
OBJS = Log.o LogSnprintf.o

# List of libraries and system objects
LIBS = $(TTL_LIB)/Cil.lib $(TTL_LIB)/Cfu.lib


# List of include files.
INCS = Log.h \
       LogPrivate.h \
       LogSnprintf.h \
       $(TTL_INCLUDE)/Cil.h \
       $(TTL_INCLUDE)/TtlSystem.h


# Build rules (general).

all:	Log.lib

test: testlog

clean:
	$(RM) $(OBJS)
	$(RM) Log.lib
	$(RM) testlog



# Executable rule.

testlog:	Log.mak testlog.o $(OBJS) $(LIBS)
	$(LN) $(LN_OPT) -o testlog testlog.o $(OBJS) $(LIBS)

# Rule to build library

Log.lib : Log.mak $(OBJS)
	$(LB) $(LB_OPT) $@ $(LB_DIV) $(OBJS)

# Source code rules (in alphabetical order).

Log.o:	Log.mak $(INCS) Log.c
	$(CC) $(CC_OPT) Log.c

LogSnprintf.o: Log.mak $(INCS) LogSnprintf.c
	$(CC) $(CC_OPT) LogSnprintf.c

testlog.o:	Log.mak Log.h testlog.c
	$(CC) $(CC_OPT) testlog.c


# Publish rule

publish :
	  $(CP) Log.lib $(TTL_LIB)
	  $(CP) Log.h   $(TTL_INCLUDE)


## EOF

