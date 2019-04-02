##
## File name:
##    Smu.mak
##
## Purpose:
##    To compile the SDB MySQL Utility (SMU) software.
##
## Description:
##    ...
##
## Authors:
##    mjf: Martyn J. Ford (TTL)
##
## Version:
##    $Id: Smu.mak,v 0.2 2007/10/04 10:58:47 mjf Exp $
##
## History:
##    $Log: Smu.mak,v $
##    Revision 0.2  2007/10/04 10:58:47  mjf
##    Link in /usr/lib/mysql/libmysqlclient.a rather than /usr/lib64/..., and
##    libz rather than libgzz, plus libssl.
##
##    Revision 0.1  2007/09/14 15:22:15  mjf
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

# Location of library
#
# NB - for 64-bit SuSE use /usr/lib64/mysql/libmysqlclient.a

MYSQLLIB = /usr/lib/mysql/libmysqlclient.a


# List of object files.
OBJS = SmuRecv.o

# List of libraries
LIBS = $(TTL_LIB)/Clu.lib \
  	      $(TTL_LIB)/Log.lib \
	      $(TTL_LIB)/Cfu.lib \
	      $(TTL_LIB)/Std.lib \
	      $(TTL_LIB)/Sdb.lib \
	      $(TTL_LIB)/Tim.lib \
	      $(MYSQLLIB) \
	      -lz -lssl -lm 

# List of include files.
INCS =	$(TTL_INCLUDE)/Sdb.h \
		$(TTL_INCLUDE)/Cil.h \
		$(TTL_INCLUDE)/TtlSystem.h

#ifeq ($(OSTYPE),linux)
# 
#  SDBRECVEXE  = SdbRecv
#
#  SDBRECVOBJS = SdbRecv.o
#
#  SDBRECVLIBS = $(TTL_LIB)/Clu.lib \
#  	      $(TTL_LIB)/Log.lib \
#	      $(TTL_LIB)/Cfu.lib \
#	      $(TTL_LIB)/Std.lib \
#	      $(TTL_LIB)/Sdb.lib \
#	      $(TTL_LIB)/Tim.lib \
#	      $(MYSQLLIB) \
#	      -lgz -lm 
#
#endif

# Build rules (general).

all:	SmuRecv

clean:
	$(RM) $(OBJS)
	$(RM) SmuRecv


# Executable rules.

SmuRecv:	Smu.mak $(OBJS) $(LIBS)
	$(LN) -o SmuRecv $(OBJS) $(LIBS)  $(LN_OPT)


# Source code rules (in alphabetical order).

SmuRecv.o: Smu.mak $(INCS) SmuRecv.c
	$(CC) $(CC_OPT) SmuRecv.c


# Publish rule

publish :
	  $(CP) SmuRecv    $(TTL_BIN)


## EOF

