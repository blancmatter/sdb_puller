##
## File name:
##    Hti.mak
##
## Purpose:
##    To compile the Header-to-ID conversion utilities.
##
## Description:
##    Compilation and link rules to make the Header-to-ID conversion utilities.
##
## Authors:
##    asp: Andrzej S. Piascik
##    mjf: Martyn J. Ford
##    djm: Derek J. McKay
##
## Version:
##    $Id: Hti.mak,v 0.15 2007/08/30 08:34:27 mjf Exp $
##
## History:
##    $Log: Hti.mak,v $
##    Revision 0.15  2007/08/30 08:34:27  mjf
##    Addition of HtiUnits utility.
##
##    Revision 0.14  2007/03/12 15:22:59  sxf
##    Build HtiSqlList process.
##
##    Revision 0.13  2006/10/13 13:34:47  mjf
##    Link against static libraries.
##
##    Revision 0.12  2005/06/17 09:26:34  mjf
##    Addition of HtiList utility.
##
##    Revision 0.11  2004/01/15 16:33:53  sxf
##    Corrected library paths.
##
##    Revision 0.10  2003/02/05 12:59:43  mjf
##    Use the HTI library from the release area.
##
##    Revision 0.9  2003/02/05 11:04:19  dws
##    Added defintion for Bol marker.
##
##    Revision 0.8  2002/10/17 13:50:51  dws
##    Fixed HtiReport to use Hti.lib from /opt/ttl/lib/
##
##    Revision 0.7  2002/10/17 13:46:27  dws
##    Added HtiReport program.
##
##    Revision 0.6  2002/04/18 15:22:45  djm
##    Added the build rules for the HtiGenBinary utility.
##
##    Revision 0.5  2001/09/20 08:35:44  mjf
##    Removal of HtiAllPkg.h as a dependency.
##
##    Revision 0.4  2001/09/19 12:47:57  mjf
##    Publish the utilities to 'util'.
##
##    Revision 0.3  2001/02/14 13:51:39  mjf
##    Addition of HTI library and publish rule for it.
##
##    Revision 0.2  2001/01/02 16:38:14  mjf
##    Use header file "HtiAllPkg.h" to include all package headers.
##
##    Revision 0.1  2001/01/02 14:30:08  mjf
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


# Objects and libraries

OBJS = HtiHdr2Id.o HtiFixRecLen.o HtiGenBinary.o HtiReport.o HtiList.o HtiUnits.o HtiSqlList.o
LIBOBJS = Hti.o


# Rule to build all

all : HtiHdr2Id HtiFixRecLen HtiGenBinary Hti.lib HtiReport HtiList HtiUnits HtiSqlList


# Rule to build exectables

HtiHdr2Id : Hti.mak HtiHdr2Id.o
	$(LN) -o HtiHdr2Id HtiHdr2Id.o $(LN_OPT) 

HtiFixRecLen : Hti.mak HtiFixRecLen.o
	$(LN) -o HtiFixRecLen HtiFixRecLen.o $(LN_OPT) 

HtiGenBinary : Hti.mak HtiGenBinary.o
	$(LN) -o HtiGenBinary HtiGenBinary.o \
		$(TTL_LIB)/Cil.lib \
		$(TTL_LIB)/Cfu.lib \
		$(LN_OPT)

HtiReport : Hti.mak HtiReport.o
	$(LN) -o HtiReport HtiReport.o \
		$(TTL_LIB)/Cil.lib \
		$(TTL_LIB)/Clu.lib \
		$(TTL_LIB)/Log.lib \
		$(TTL_LIB)/Cfu.lib  \
		$(TTL_LIB)/Tim.lib  \
		Hti.lib \
		$(LN_OPT)

HtiList : Hti.mak HtiList.o
	$(LN) -o HtiList HtiList.o \
		$(TTL_LIB)/Cil.lib \
		$(TTL_LIB)/Clu.lib \
		$(TTL_LIB)/Log.lib \
		$(TTL_LIB)/Cfu.lib  \
		$(TTL_LIB)/Tim.lib  \
		Hti.lib \
		$(LN_OPT)

HtiUnits : Hti.mak HtiUnits.o
	$(LN) -o HtiUnits HtiUnits.o \
		$(TTL_LIB)/Cil.lib \
		$(TTL_LIB)/Clu.lib \
		$(TTL_LIB)/Log.lib \
		$(TTL_LIB)/Cfu.lib  \
		$(TTL_LIB)/Tim.lib  \
		Hti.lib \
		$(LN_OPT)

HtiSqlList : Hti.mak HtiSqlList.o
	$(LN) -o HtiSqlList HtiSqlList.o \
		$(TTL_LIB)/Cil.lib \
		$(TTL_LIB)/Clu.lib \
		$(TTL_LIB)/Log.lib \
		$(TTL_LIB)/Cfu.lib  \
		$(TTL_LIB)/Tim.lib  \
		Hti.lib \
		$(LN_OPT)

# Rule to build library

Hti.lib : Hti.mak $(LIBOBJS)
	$(LB) $(LB_OPT) $@ $(LB_DIV) $(LIBOBJS)


# Rule to build objects

HtiHdr2Id.o      : HtiHdr2Id.c

HtiFixRecLen.o   : HtiFixRecLen.c 

HtiGenBinary.o   : HtiGenBinary.c 

HtiReport.o   : HtiReport.c Hti.h HtiPrivate.h

HtiList.o        : HtiList.c 

HtiUnits.o       : HtiUnits.c 

Hti.o            : Hti.c Hti.h HtiPrivate.h


# Clean rule

clean : 
	$(RM) $(OBJS) $(LIBOBJS)
	$(RM) HtiHdr2Id HtiFixRecLen HtiGenBinary HtiReport HtiList HtiUnits Hti.lib


# Publish rule

publish :
	  $(CP) Hti.h        $(TTL_INCLUDE)
	  $(CP) Hti.lib      $(TTL_LIB)
	  $(CP) HtiHdr2Id    $(TTL_UTIL)
	  $(CP) HtiFixRecLen $(TTL_UTIL)
	  $(CP) HtiGenBinary $(TTL_UTIL)
	  $(CP) HtiReport $(TTL_UTIL)
	  $(CP) HtiList      $(TTL_UTIL)
	  $(CP) HtiUnits     $(TTL_UTIL)


##  
## End of file
##
