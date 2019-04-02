##
## File name:
##    Ags.mak
##
## Purpose:
##    To compile the Autoguider Service Task (AGS).
##
## Description:
##    ...
##
## Authors:
##    Stephen Foale (sxf)
##
## Version:
##    $Id: Ags.mak,v 0.5 2006/10/02 13:08:54 sxf Exp $
##
## History:
##    $Log: Ags.mak,v $
##    Revision 0.5  2006/10/02 13:08:54  sxf
##    Baseline AGS_1_04
##    ./
##
##    Revision 0.4  2005/10/04 12:40:53  sxf
##    Brought in line with QNX4 baseline AGS_0_17.
##
##    Revision 0.3  2001/12/05 14:02:49  mjf
##    Minor changes to allow integration into the whole QNX build process.
##
##    Revision 0.2  2001/10/17 15:39:06  sxf
##    First version with basic functionality.
##
##    Revision 0.1  2001/09/18 14:15:51  sxf
##    Initial version.
##    ,
##
##
##

include ../../TtlLabels.mak

# Declare phony targets
.PHONY : all
.PHONY : clean
.PHONY : publish

# Labels
APP = Ags
APPSCRIPT = AgsAgs AgsDgs AgsWgs AgsPgs

# Implicit rules
.c.o : 
	$(CC) $(CC_OPT_ANSI) $< -o $@

# Objects and libraries
OBJS = AgsInit.o \
       AgsMain.o \
       AgsProcess.o \
       AgsReply.o \
       AgsReport.o \
       AgsUtil.o

INCS = Ags.h \
       AgsPrivate.h \
       $(TTL_INCLUDE)/Sdb.h \
       $(TTL_INCLUDE)/Clu.h \
       $(TTL_INCLUDE)/Tim.h \
       $(TTL_INCLUDE)/Cil.h \
       $(TTL_INCLUDE)/Log.h \
       $(TTL_INCLUDE)/TtlSystem.h \
       $(TTL_INCLUDE)/Agg.h

LIBS = $(TTL_LIB)/Cil.lib \
       $(TTL_LIB)/Clu.lib \
       $(TTL_LIB)/Log.lib \
       $(TTL_LIB)/Tim.lib \
       $(TTL_LIB)/Cfu.lib \
       $(TTL_LIB)/Hti.lib 

# Rule to build all
all: $(APP)

# Rule to build exectable
Ags: Ags.mak $(OBJS) $(LIBS)
	 $(LN) -o $@ $(OBJS) $(LIBS) $(LN_OPT)


# Rules to build objects

AgsInit.o:	         Ags.mak $(INCS) AgsInit.c
	                 $(CC) $(CC_OPT) AgsInit.c

AgsMain.o:	         Ags.mak $(INCS) AgsMain.c
	                 $(CC) $(CC_OPT) AgsMain.c

AgsProcess.o:        Ags.mak $(INCS) AgsProcess.c
	                 $(CC) $(CC_OPT) AgsProcess.c

AgsReply.o:	         Ags.mak $(INCS) AgsReply.c
	                 $(CC) $(CC_OPT) AgsReply.c

AgsReport.o:	     Ags.mak $(INCS) AgsReport.c
	                 $(CC) $(CC_OPT) AgsReport.c

AgsUtil.o:	         Ags.mak $(INCS) AgsUtil.c
	                 $(CC) $(CC_OPT) AgsUtil.c

# Clean rule

clean : 
	$(RM) $(OBJS)
	$(RM) $(APP)

# Publish rules

publish : 
	$(CP) Ags $(TTL_BIN)
	$(CP) $(APPSCRIPT) $(TTL_BIN)
	$(CP) Ags.h $(TTL_INCLUDE)

## EOF
