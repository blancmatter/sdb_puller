##
## Filename:    
##    Clu.mak
##
## Purpose:
##    Makefile for Command Line Utilities (CLU) package.
##
## Description:
##    N/A
##
## Authors:
##    mjf : Martyn J. Ford (TTL)
##
## Version:
##    $Id: Clu.mak,v 0.15 2001/02/06 10:32:55 mjf Exp $
##
## History:
##    $Log: Clu.mak,v $
##    Revision 0.15  2001/02/06 10:32:55  mjf
##    To build test application use target 'test'. Also libraries removed
##    from being a dependency of Clu.lib, and moved to test application.
##
##    Revision 0.14  2001/01/25 16:57:30  mjf
##    Addition of publish rule.
##
##    Revision 0.13  2001/01/04 17:48:56  mjf
##    Link in CFU library with example for new LOG library.
##
##    Revision 0.12  2000/12/12 18:45:51  mjf
##    CLU library now split into more than one source file.
##
##    Revision 0.11  2000/12/11 14:51:04  mjf
##    Addition of function to submit the application's version into the SDB.
##    This function will also perform a CIL setup if one has not already been
##    done.
##
##    Revision 0.10  2000/11/08 13:43:34  mjf
##    Import tool declarations from default labels.
##
##    Revision 0.9  2000/11/08 11:38:51  mjf
##    Makefile enhanced to use default labels. Supports both QNX and Linux.
##
##    Revision 0.8  2000/11/03 13:18:19  asp
##    Bugfix CluExample now builds from clean.
##
##    Revision 0.7  2000/10/31 11:34:01  djm
##    Changes to facilitate port to Linux.
##
##    Revision 0.6  2000/10/09 14:37:21  mjf
##    Force removal of files on a clean.
##
##    Revision 0.5  2000/08/17 16:57:24  mjf
##    Renaming of Makefile to Clu.mak.
##
##    Revision 0.4  2000/08/11 15:23:24  mjf
##    Addition of library and public header as dependencies for example.
##
##    Revision 0.3  2000/08/09 14:55:37  mjf
##    Make source dependent on Clu.h in this directory. Also clean example.
##
##    Revision 0.2  2000/07/27 10:38:00  mjf
##    Get Clu.h from /opt/ttl/include, and build CluExample.
##
##    Revision 0.1  2000/07/25 13:21:09  mjf
##    Initial revision.
##
##
##


# Include default labels for build

include ../TtlLabels.mak


# Declare phony targets

.PHONY : all
.PHONY : clean
.PHONY : publish


# Implicit rules

.c.o : 
	$(CC) $(CC_OPT) $< -o $@


# Objects

OBJS = Clu.o CluVersion.o
LIBS = $(TTL_LIB)/Log.lib $(TTL_LIB)/Cil.lib \
       $(TTL_LIB)/Tim.lib $(TTL_LIB)/Cfu.lib
 

# Include files

INCS =	$(TTL_INCLUDE)/TtlSystem.h \
   		$(TTL_INCLUDE)/Cil.h \
   		$(TTL_INCLUDE)/Log.h \
   		$(TTL_INCLUDE)/Sdb.h \
   		$(TTL_INCLUDE)/Tim.h


# Build rules (general)

all:	Clu.lib

test:	CluExample

clean: 
		$(RM) $(OBJS)
		$(RM) Clu.lib
		$(RM) CluExample CluExample.o


# Rule to build library

Clu.lib:	Clu.mak $(OBJS)
			$(LB) $(LB_OPT) $@ $(LB_DIV) $(OBJS)

# Rule to build example

CluExample:	CluExample.o Clu.lib Clu.h $(INCS) $(LIBS)
			$(LN) $(LN_OPT) -o CluExample CluExample.o $(OBJS) $(LIBS)

# Rule to build object

Clu.o:	Clu.mak $(INCS) Clu.c
		$(CC) $(CC_OPT_NON_ANSI) Clu.c

CluVersion.o:	Clu.mak $(INCS) CluVersion.c
		$(CC) $(CC_OPT) CluVersion.c

CluExample.o:	Clu.mak Clu.h CluExample.c
		$(CC) $(CC_OPT) CluExample.c


# Publish rule

publish :
	  $(CP) Clu.lib $(TTL_LIB)
	  $(CP) Clu.h   $(TTL_INCLUDE)


## EOF

