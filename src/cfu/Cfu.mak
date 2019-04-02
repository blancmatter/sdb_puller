##
## File name:
##    Cfu.mak
##
## Purpose:
##    To compile the Configuration File Utility (CFU) code.
##
## Description:
##    ...
##
## Authors:
##    mjf: Martyn J. Ford (TTL)
##    djm: Derek J. McKay (TTL)
##
## Version:
##    $Id: Cfu.mak,v 0.8 2001/01/25 16:45:43 mjf Exp $
##
## History:
##    $Log: Cfu.mak,v $
##    Revision 0.8  2001/01/25 16:45:43  mjf
##    Addition of publish rule.
##
##    Revision 0.7  2000/11/08 13:39:50  mjf
##    Import tool definitions from default labels.
##
##    Revision 0.6  2000/11/08 10:53:59  mjf
##    Makefile enhanced to use default labels. Supports both QNX and Linux.
##
##    Revision 0.5  2000/11/07 12:22:03  mjf
##    Checking in of changes done by DJM for porting to Linux. Note -
##    necessary to comment out line to echo the OS being used for now.
##
##    Revision 0.4  2000/10/09 14:33:58  mjf
##    Force removal of files on a clean.
##
##    Revision 0.3  2000/08/17 16:48:05  mjf
##    Renaming of Makefile to Cfu.mak.
##
##    Revision 0.2  2000/08/11 15:35:51  mjf
##    Addition of public header as dependency for example.
##
##    Revision 0.1  2000/08/09 09:08:45  mjf
##    Initial revision.
##


# Include default labels for build

include ../TtlLabels.mak


# Declare phony targets

.PHONY : all
.PHONY : clean
.PHONY : publish


# List of object files.
OBJS =	Cfu.o

# List of libraries and system objects
LIBS =	


# List of include files.
INCS =	Cfu.h \
		CfuPrivate.h \
		$(TTL_INCLUDE)/Cil.h \
		$(TTL_INCLUDE)/TtlSystem.h


# Build rules (general).

all:	Cfu.lib	CfuExample


clean:
		$(RM) $(OBJS)
		$(RM) Cfu.lib
		$(RM) CfuExample.o CfuExample



# Executable rule.

CfuExample:	Cfu.mak CfuExample.o $(OBJS) $(LIBS)
			$(LN) $(LN_OPT) -o CfuExample CfuExample.o $(OBJS) $(LIBS)

# Rule to build library

Cfu.lib:	Cfu.mak $(OBJS) $(LIBS)
			$(LB) $(LB_OPT) $@ $(LB_DIV) $(OBJS)

# Source code rules (in alphabetical order).

Cfu.o:		Cfu.mak $(INCS) Cfu.c
			$(CC) $(CC_OPT) Cfu.c

CfuExample.o:	Cfu.mak Cfu.h CfuExample.c
			$(CC) $(CC_OPT) CfuExample.c


# Publish rule

publish :
	  $(CP) Cfu.lib $(TTL_LIB)
	  $(CP) Cfu.h   $(TTL_INCLUDE)



### Not sure what this is or where it came from ? Added by DJM. (mjf)
###/usr/local/src/AIDE/aide-0.7/Makefile.in/usr/local/src/AIDE/aide-0.7/Makefile.in



## EOF
