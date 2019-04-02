##
## File name:
##    Wfl.mak
##
## Purpose:
##    To compile the Wrapper Function Library (WFL) code.
##
## Description:
##    ...
##
## Authors:
##    Steve Foale (sxf)
##
## Version:
##    $Id: Wfl.mak,v 0.6 2007/10/05 09:23:33 mjf Exp $
##
## History:
##    $Log: Wfl.mak,v $
##    Revision 0.6  2007/10/05 09:23:33  mjf
##    Addition of new library member WflMsg.c, and test files also added.
##
##    Revision 0.5  2004/04/20 14:52:17  mjf
##    Test programs commented out.
##
##    Revision 0.4  2004/04/19 13:25:03  man
##    Updated to build WflPhab.c
##
##    Revision 0.3  2004/02/20 15:21:27  man
##    Removed WflRes from makefile.
##
##    Revision 0.2  2004/02/12 16:17:30  man
##    Modification to make file.
##
##    Revision 0.1  2004/02/10 11:46:03  sxf
##    Initial version.
##
##


# Include default labels for build

include ../TtlLabels.mak

# Declare phony targets

.PHONY : all
.PHONY : clean
.PHONY : publish

# List of object files.
OBJS =	Wfl.o WflMsg.o WflPci.o WflPhab.o WflWatchdog.o Wfl.o

# List of libraries and system objects
LIBS =	


# List of include files.
INCS =	Wfl.h \
		WflPrivate.h \
		$(TTL_INCLUDE)/TtlSystem.h


# Build rules (general).

all:	Wfl.lib WflTestTim WflSndRcv WflKillPid


clean:
		$(RM) $(OBJS)
		$(RM) Wfl.lib
		$(RM) WflTestTim WflTestTim.o
		$(RM) WflSndRcv WflSndRcv.o
		$(RM) WflKillPid WflKillPid.o
      

# Rule to build library

Wfl.lib:	Wfl.mak $(OBJS) $(LIBS)
			$(LB) $(LB_OPT) $@ $(OBJS)

WflTestTim :   Wfl.mak $(LIBS) Wfl.lib WflTestTim.o
			$(LN) $(LN_OPT) -o WflTestTim WflTestTim.o Wfl.lib

WflSndRcv :    Wfl.mak $(LIBS) Wfl.lib WflSndRcv.o
			$(LN) $(LN_OPT) -o WflSndRcv WflSndRcv.o Wfl.lib

WflKillPid :    Wfl.mak $(LIBS) Wfl.lib WflKillPid.o
			$(LN) $(LN_OPT) -o WflKillPid WflKillPid.o Wfl.lib


# Source code rules (in alphabetical order).

Wfl.o:		Wfl.mak $(INCS) Wfl.c
		$(CC) $(CC_OPT_PRIVITY_1) Wfl.c

WflMsg.o:		Wfl.mak $(INCS) WflMsg.c
		$(CC) $(CC_OPT_PRIVITY_1) WflMsg.c

WflPci.o:	Wfl.mak $(INCS) WflPci.c
		$(CC) $(CC_OPT_PRIVITY_1) WflPci.c

WflPhab.o:	Wfl.mak $(INCS) WflPhab.c
		$(CC) $(CC_OPT_PRIVITY_1) WflPhab.c

WflWatchdog.o:	Wfl.mak $(INCS) WflWatchdog.c
		$(CC) $(CC_OPT_PRIVITY_1) WflWatchdog.c

WflTestTim.o:	Wfl.mak $(INCS) WflTestTim.c
		$(CC) $(CC_OPT_PRIVITY_1) WflTestTim.c

WflSndRcv.o:	Wfl.mak $(INCS) WflSndRcv.c
		$(CC) $(CC_OPT_PRIVITY_1) WflSndRcv.c
    
WflKillPid.o:	Wfl.mak $(INCS) WflKillPid.c
		$(CC) $(CC_OPT_PRIVITY_1) WflKillPid.c


# Publish rule

publish :
	  $(CP) Wfl.lib $(TTL_LIB)
	  $(CP) Wfl.h   $(TTL_INCLUDE)

## EOF
