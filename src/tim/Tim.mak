##
## File name:
##    Tim.mak
##
## Purpose:
##    To compile the TTL System Time (TIM) library.
##
## Description:
##    ...
##
## Authors:
##    djm: Derek J. McKay (TTL)
##    mjf: Martyn J. Ford (TTL)
##
## Version:
##    $Id: Tim.mak,v 0.11 2001/01/25 16:49:56 mjf Exp $
##
## History:
##    $Log: Tim.mak,v $
##    Revision 0.11  2001/01/25 16:49:56  mjf
##    Addition of publish rule.
##
##    Revision 0.10  2000/11/14 14:56:14  djm
##    Added TtlConstants.h dependency.
##
##    Revision 0.9  2000/11/08 13:47:10  mjf
##    Import tool declarations from default labels.
##
##    Revision 0.8  2000/11/08 11:51:12  mjf
##    Makefile enhanced to use default labels. Supports both QNX and Linux.
##
##    Revision 0.7  2000/11/01 17:24:09  asp
##    Restored QNX as default build option
##
##    Revision 0.6  2000/11/01 15:21:36  asp
##    Port makefile to Linux
##
##    Revision 0.5  2000/10/09 12:43:48  mjf
##    Change of dependency 'Makefile' into 'Tim.mak'.
##
##    Revision 0.4  2000/10/09 11:17:03  mjf
##    Renaming of Makefile to Tim.mak. Include Tim.h and TimPrivate.h from
##    local directory. Output from build is now a library.
##
##    Revision 0.3  2000/06/16 15:34:29  rdl
##    Added option to copy Tim.o to /opt/ttl/obj directory.
##
##    Revision 0.2  2000/06/15 14:24:20  rdl
##    Added -g2 option to LINK ARGUMENTS (debug switch).
##
##    Revision 0.1  2000/05/26 08:12:43  djm
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
OBJS = Tim.o testprog.o

# List of libraries and system objects
LIBS =

# List of include files.
INCS = TimPrivate.h \
       Tim.h \
       $(TTL_INCLUDE)/TtlConstants.h \
       $(TTL_INCLUDE)/TtlSystem.h


# Build rules (general).

all:	Tim.lib testprog

clean:
	$(RM) testprog
	$(RM) $(OBJS)
	$(RM) Tim.lib


# Executable rules

testprog:	Tim.mak $(OBJS)
		$(LN) $(LN_OPT) -o testprog $(OBJS)


# Rule to build library

Tim.lib : Tim.mak $(OBJS) $(LIBS)
	$(LB) $(LB_OPT) $@ $(LB_DIV) $(OBJS)


# Object code rules (in alphabetical order).

Tim.o:	Tim.mak $(INCS) Tim.c
	$(CC) $(CC_OPT_NON_ANSI) Tim.c

testprog.o:	Tim.mak Tim.h testprog.c
	$(CC) $(CC_OPT) testprog.c


# Publish rule

publish :
	  $(CP) Tim.lib $(TTL_LIB)
	  $(CP) Tim.h   $(TTL_INCLUDE)


## EOF

