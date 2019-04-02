##
## File name:
##    Std.mak
##
## Purpose:
##    To compile the Status Database (SDB) test dump
##    software.
##
## Description:
##    ...
##
## Authors:
##    man: Martin Norbury (man)
##
## Version:
##    $Id: Std.mak,v 0.7 2006/10/03 12:39:04 sxf Exp $
##
## History:
##    $Log: Std.mak,v $
##    Revision 0.7  2006/10/03 12:39:04  sxf
##    Build against static libraries.
##
##    Revision 0.5  2005/09/01 08:01:22  mjf
##    Small correction to dependencies.
##
##    Revision 0.4  2005/08/31 14:18:08  sxf
##    Allow reading of gzipped sdb files.
##
##    Revision 0.3  2003/09/11 14:42:52  sxf
##    Remove -l from link line.
##
##    Revision 0.2  2003/01/15 16:20:52  man
##    Updated Std to use Std library functions.
##
##    Revision 0.1  2002/07/08 10:25:31  man
##    Initial revision.
##
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
OBJS =	StdMain.o \
		StdInit.o \
		StdOutput.o 


ZLIB_OBJ = gzio.o \
           zutil.o \
           crc32.o \
           deflate.o \
           inflate.o \
           adler32.o \
           trees.o \
           infblock.o \
           infcodes.o \
           inftrees.o \
           infutil.o \
           inffast.o

# List of libraries
LIBS = 	Std.lib \
		zlib.lib \
		$(TTL_LIB)/Sdb.lib \
		$(TTL_LIB)/Clu.lib \
		$(TTL_LIB)/Cil.lib \
		$(TTL_LIB)/Log.lib \
		$(TTL_LIB)/Tim.lib \
		$(TTL_LIB)/Cfu.lib \
		$(TTL_LIB)/Hti.lib

# List of include files.
INCS =	Std.h \
		StdPrivate.h \
		$(TTL_INCLUDE)/Cil.h \
		$(TTL_INCLUDE)/TtlSystem.h

ZLIB_INCS = deflate.h \
            infblock.h \
            infcodes.h \
            inffast.h \
            inffixed.h \
            inftrees.h \
            infutil.h \
            trees.h \
            zconf.h \
            zlib.h \
            zutil.h

# Build rules (general).

all:	Std.lib zlib.lib Std 

clean:
	$(RM) $(OBJS) $(ZLIB_OBJ)
	$(RM) Std
	$(RM) Std.lib
	$(RM) zlib.lib


# Executable rules.

Std:	Std.mak $(OBJS) $(LIBS)
	$(LN) -o Std $(OBJS) $(LIBS) $(LN_OPT)



# Library build rules

Std.lib:	Std.mak StdLib.o 
	$(LB) $(LB_OPT) $@ $(LB_DIV) StdLib.o

zlib.lib:	$(ZLIB_OBJ) $(ZLIB_INCS)
	$(LB) $(LB_OPT) $@ $(LB_DIV) $(ZLIB_OBJ)


# Source code rules.

StdMain.o:	Std.mak $(INCS) StdMain.c
	$(CC) $(CC_OPT) StdMain.c

StdInit.o:  Std.mak $(INCS) StdInit.c
	$(CC) $(CC_OPT) StdInit.c

StdOutput.o:  Std.mak $(INCS) StdOutput.c
	$(CC) $(CC_OPT) StdOutput.c

StdLib.o:  Std.mak $(INCS) StdLib.c
	$(CC) $(CC_OPT) StdLib.c

gzio.o:    Std.mak $(ZLIB_INCS) gzio.c
	$(CC)  $(CC_OPT_NON_ANSI) gzio.c

zutil.o:    Std.mak $(ZLIB_INCS) zutil.c
	$(CC)  $(CC_OPT_NON_ANSI) zutil.c

crc32.o:    Std.mak $(ZLIB_INCS) crc32.c
	$(CC)  $(CC_OPT_NON_ANSI) crc32.c

deflate.o:    Std.mak $(ZLIB_INCS) deflate.c
	$(CC)  $(CC_OPT_NON_ANSI) deflate.c

inflate.o:    Std.mak $(ZLIB_INCS) inflate.c
	$(CC)  $(CC_OPT_NON_ANSI) inflate.c

adler32.o:    Std.mak $(ZLIB_INCS) adler32.c
	$(CC)  $(CC_OPT_NON_ANSI) adler32.c

trees.o:    Std.mak $(ZLIB_INCS) trees.c
	$(CC)  $(CC_OPT_NON_ANSI) trees.c

infblock.o:    Std.mak $(ZLIB_INCS) infblock.c
	$(CC)  $(CC_OPT_NON_ANSI) infblock.c

infcodes.o:    Std.mak $(ZLIB_INCS) infcodes.c
	$(CC)  $(CC_OPT_NON_ANSI) infcodes.c

inftrees.o:    Std.mak $(ZLIB_INCS) inftrees.c
	$(CC)  $(CC_OPT_NON_ANSI) inftrees.c

infutil.o:    Std.mak $(ZLIB_INCS) infutil.c
	$(CC)  $(CC_OPT_NON_ANSI) infutil.c

inffast.o:    Std.mak $(ZLIB_INCS) inffast.c
	$(CC)  $(CC_OPT_NON_ANSI) inffast.c

# Publish rule

publish :
	  $(CP) Std        $(TTL_UTIL)
	  $(CP) Std.lib    $(TTL_LIB)
	  $(CP) zlib.lib   $(TTL_LIB)
	  $(CP) zlib.h     $(TTL_INCLUDE)
	  $(CP) zconf.h    $(TTL_INCLUDE)
	  $(CP) Std.h      $(TTL_INCLUDE)

## EOF

