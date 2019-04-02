##
## Filename:    
##    Cmt.mak
##
## Purpose:
##    Makefile for Computer Monitoring Task (CMT)
##
## Description:
##    N/A
##
## Authors:
##    mjf : Martyn J. Ford (TTL)
##    asp : Andrzej S. Piascik (asp)
##
## Version:
##    $Id: Cmt.mak,v 0.12 2007/10/04 10:27:15 mjf Exp $
##
## History:
##    $Log: Cmt.mak,v $
##    Revision 0.12  2007/10/04 10:27:15  mjf
##    Tidy-up.
##
##    Revision 0.11  2005/07/14 09:20:46  sxf
##    Basic monitoring functionality now implemented. Monitoring of:
##       o CPU load
##       o Disk space
##       o Used memory
##
##    Revision 0.10  2004/06/21 17:31:47  mjf
##    Change to ordering of linker options.
##
##    Revision 0.9  2004/04/06 12:47:07  mjf
##    Re-application of SXF's QNX v6 porting to CMT_0_11.
##
##    Revision 0.8  2003/12/02 14:45:36  sxf
##    Added build of CmtLinux.c
##
##    Revision 0.7  2003/02/11 14:32:02  sxf
##    Fetch Cmt.cfg from RCS and copy to /opt/ttl/etc as part of buold.
##
##    Revision 0.6  2002/08/01 10:43:23  sxf
##    No longer build CmtHardware.c but split into CmtQnx.c, CmtVms.c and
##    CmtLinux.c for system specific function calls.
##
##    Revision 0.5  2001/03/16 21:27:51  asp
##    Modified to new format
##
##    Revision 0.4  2001/01/26 08:55:24  mjf
##    Addition of publish rule.
##
##    Revision 0.3  2001/01/04 18:06:14  mjf
##    Correction to use of RM label in 'clean'.
##
##    Revision 0.2  2000/11/14 15:56:56  asp
##    Now includes TtlLabels.mak to make porting to Linux/VMS easier.
##
##    Revision 0.1  2000/11/01 17:33:00  asp
##    *** empty log message ***
##
##
##
##

include ../TtlLabels.mak

# Declare phony targets
.PHONY : all
.PHONY : clean
.PHONY : publish

# Labels
APP = Cmt
DUMMYAPP = a.out

# Implicit rules
.c.o : 
	$(CC) -g $(CC_OPT_NON_ANSI) $< -o $@

#	$(CC) $(CC_OPT_PRIVITY_1) $< -o $@

# Objects and libraries
OBJS = CmtMain.o \
       CmtInit.o \
       CmtInterface.o \
       CmtReport.o \
       CmtUtil.o \
       CmtQnx.o \
       CmtQnx6.o \
       CmtLinux.o
       
LIBS = $(TTL_LIB)/Cil.lib \
       $(TTL_LIB)/Clu.lib \
       $(TTL_LIB)/Log.lib \
       $(TTL_LIB)/Tim.lib \
       $(TTL_LIB)/Cfu.lib

INCS = Cmt.h \
       CmtPrivate.h \
       $(TTL_INCLUDE)/Sdb.h \
       $(TTL_INCLUDE)/Clu.h \
       $(TTL_INCLUDE)/Tim.h \
       $(TTL_INCLUDE)/Cil.h \
       $(TTL_INCLUDE)/Log.h \
       $(TTL_INCLUDE)/TtlSystem.h

# Rule to build all
all: $(DUMMYAPP)


# Rule to build exectable
$(DUMMYAPP): Cmt.mak $(OBJS) $(LIBS)
	 $(LN) -o $@ $(OBJS) $(LIBS) $(LN_OPT_PRIVITY_1) 
	 $(CPPRES) $(DUMMYAPP) $(APP)

# Rules to build objects

CmtInit.o:	         Cmt.mak $(INCS) CmtInit.c
	                 $(CC) $(CC_OPT_PRIVITY_1) CmtInit.c

CmtInterface.o:      Cmt.mak $(INCS) CmtInterface.c
	                 $(CC) $(CC_OPT_PRIVITY_1) CmtInterface.c

CmtMain.o:	         Cmt.mak $(INCS) CmtMain.c
	                 $(CC) $(CC_OPT_PRIVITY_1) CmtMain.c

CmtReport.o:	      Cmt.mak $(INCS) CmtReport.c
	                 $(CC) $(CC_OPT_PRIVITY_1) CmtReport.c

CmtUtil.o:	         Cmt.mak $(INCS) CmtUtil.c
	                 $(CC) $(CC_OPT_PRIVITY_1) CmtUtil.c

CmtQnx.o:	         Cmt.mak $(INCS) CmtQnx.c
	                 $(CC) $(CC_OPT_PRIVITY_1) CmtQnx.c

CmtQnx6.o:	         Cmt.mak $(INCS) CmtQnx6.c
	                 $(CC) $(CC_OPT_PRIVITY_1) CmtQnx6.c

CmtLinux.o:	         Cmt.mak $(INCS) CmtLinux.c
	                 $(CC) $(CC_OPT_PRIVITY_1) CmtLinux.c
# Clean rule

clean : 
	$(RM) $(OBJS)
	$(RM) $(DUMMYAPP)


# Publish rule

publish :
	  $(CPPRES) Cmt     $(TTL_BIN)
	  $(CP)     Cmt.h   $(TTL_INCLUDE)

## EOF
