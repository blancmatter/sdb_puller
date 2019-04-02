################################################################################
## Filename:    
##    Cct.mak
##
## Purpose:
##    Makefile for Computer Control Task (CCT).
##
## Description:
##    Use 'make' to build the CCT software. Make the 'clean' target to remove 
##    all intermediate files to then force a full rebuild. Make the 'publish'
##    target to copy the executable and public header to the release area.
##
## Traceability to Software Design:
##    -
##
## Author(s):
##    Martyn J. Ford (mjf)
##
## Copyright (c) Telescope Technologies Limited (TTL), 2001
##
## Version:
##    $Id: Cct.mak,v 0.6 2007/10/04 10:26:27 mjf Exp $
##
## History:
##    $Log: Cct.mak,v $
##    Revision 0.6  2007/10/04 10:26:27  mjf
##    CCT_1_03 - fix to typo in makefile, no functional changes.
##
##    Revision 0.5  2006/10/02 10:27:11  sxf
##    Baseline CCT_1_01.
##
##    Revision 0.4  2001/08/07 08:31:56  mjf
##    Addition of delay following the execution of a process. Set to 1000 ms
##    by default, may be altered on the command-line.
##
##    Revision 0.3  2001/03/08 16:57:29  mjf
##    Copy application without unlinking to preserve the attributes.
##
##    Revision 0.2  2001/03/08 16:54:44  mjf
##    Build with non-ansi compile options, link in CFU.
##
##    Revision 0.1  2001/03/07 12:00:48  mjf
##    Initial revision.
##
##
################################################################################


# Include default labels for build

include ../TtlLabels.mak


# Declare phony targets

.PHONY : all
.PHONY : clean
.PHONY : publish


# Labels

APP = Cct
MAK = Cct.mak
HDR = Cct.h


# Implicit rules

.c.o : 
	$(CC) $(CC_OPT_NON_ANSI) $< -o $@


# Objects and libraries

OBJS = CctMain.o CctInit.o CctOsInterface.o CctUtil.o
LIBS = $(TTL_LIB)/Cil.lib $(TTL_LIB)/Log.lib $(TTL_LIB)/Tim.lib \
       $(TTL_LIB)/Cfu.lib $(TTL_LIB)/Clu.lib 


# Rule to build all

all : $(APP)


# Rule to build exectable

$(APP) : $(MAK) $(OBJS) $(LIBS)
	$(LN) -o $(APP) $(OBJS) $(LIBS) $(LN_OPT)


# Rule to build objects

CctMain.o        : CctMain.c \
                   Cct.h CctPrivate.h \
                   $(TTL_INCLUDE)/TtlSystem.h $(TTL_INCLUDE)/TtlConstants.h \
                   $(TTL_INCLUDE)/Mcp.h $(TTL_INCLUDE)/Cil.h \
                   $(TTL_INCLUDE)/Clu.h $(TTL_INCLUDE)/Sdb.h \
                   $(TTL_INCLUDE)/Log.h $(TTL_INCLUDE)/Tim.h

CctInit.o        : CctInit.c \
                   Cct.h CctPrivate.h \
                   $(TTL_INCLUDE)/TtlSystem.h $(TTL_INCLUDE)/TtlConstants.h \
                   $(TTL_INCLUDE)/Mcp.h $(TTL_INCLUDE)/Cil.h \
                   $(TTL_INCLUDE)/Clu.h $(TTL_INCLUDE)/Sdb.h \
                   $(TTL_INCLUDE)/Log.h $(TTL_INCLUDE)/Tim.h

CctOsInterface.o : CctOsInterface.c \
                   Cct.h CctPrivate.h \
                   $(TTL_INCLUDE)/TtlSystem.h $(TTL_INCLUDE)/TtlConstants.h \
                   $(TTL_INCLUDE)/Mcp.h $(TTL_INCLUDE)/Cil.h \
                   $(TTL_INCLUDE)/Clu.h $(TTL_INCLUDE)/Sdb.h \
                   $(TTL_INCLUDE)/Log.h $(TTL_INCLUDE)/Tim.h \
                   $(TTL_INCLUDE)/Cfu.h

CctUtil.o        : CctUtil.c \
                   Cct.h CctPrivate.h \
                   $(TTL_INCLUDE)/TtlSystem.h $(TTL_INCLUDE)/TtlConstants.h \
                   $(TTL_INCLUDE)/Mcp.h $(TTL_INCLUDE)/Cil.h \
                   $(TTL_INCLUDE)/Clu.h $(TTL_INCLUDE)/Sdb.h \
                   $(TTL_INCLUDE)/Log.h $(TTL_INCLUDE)/Tim.h


# Clean rule

clean : 
	$(RM) $(OBJS)
	$(RM) $(APP)

   
# Publish rule

publish :
	  $(CPPRES) $(APP)   $(TTL_BIN)
	  $(CP) $(HDR)   $(TTL_INCLUDE)


### EOF

