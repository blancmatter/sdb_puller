##
## Filename:    
##    Tdl.mak
##
## Purpose:
##    Makefile for TTL Development Library (TDL) package.
##
## Description:
##    N/A
##
## Authors:
##    mjf : Martyn J. Ford (TTL)
##
## Version:
##    $Id: Tdl.mak,v 0.2 2007/09/24 14:02:03 mjf Exp $
##
## History:
##    $Log: Tdl.mak,v $
##    Revision 0.2  2007/09/24 14:02:03  mjf
##    Build all the source for the TDL library and two variants of examples
##    from the current directory, to be built stand-alone on another machine.
##
##    Revision 0.1  2001/05/21 08:42:38  mjf
##    Initial revision.
##
##
##


# Define default labels for build

CC              = cc -c
LN              = cc
LB              = ar
CC_OPT          = -pedantic -ansi 
CC_OPT_NON_ANSI = -pedantic
LN_OPT          = -pedantic -ansi -lm
LB_OPT          = -r
LB_DIV          = 
RM              = rm -f
CP              = cp -f

# Overwrite any default labels for QNX v6

ifeq ($(SYSNAME),nto)

CC_OPT          = -ansi -D_POSIX_VERSION=200112L
CC_OPT_NON_ANSI = -D_POSIX_VERSION=200112L
LN_OPT          = -static -ansi -lsocket -lm
LB_OPT          = -rs

endif



# Declare phony targets

.PHONY : all
.PHONY : clean
.PHONY : publish


# Implicit rules

.c.o : 
		$(CC) $(CC_OPT) $< -o '$(@F)'


# Target library, public header and examples

LIBRARY = Tdl.lib
HEADER  = Tdl.h
EXAMPLE = TdlTx TdlRx


# Object members of library

OBJECTS = Cil.o CilUtil.o Tim.o Cfu.o
 

# Build rule

all :	$(LIBRARY) $(EXAMPLE)


# Rule to build library

$(LIBRARY) :	Tdl.mak $(OBJECTS)
		$(LB) $(LB_OPT) $@ $(LB_DIV) $(OBJECTS)


# Rules to build examples

TdlTx :	TdlExample.c $(LIBRARY)
		$(LN) -o $@ TdlExample.c $(LIBRARY) $(CC_OPT) $(LN_OPT) -DTX

TdlRx :	TdlExample.c $(LIBRARY)
		$(LN) -o $@ TdlExample.c $(LIBRARY) $(CC_OPT) $(LN_OPT) -DRX


# Specific rules for building objects

Tim.o  :		Tim.c Tdl.mak
		$(CC) $(CC_OPT_NON_ANSI) $< -o $@


# Clean rule

clean : 
		$(RM) $(LIBRARY)
		$(RM) $(OBJECTS)
		$(RM) $(EXAMPLE)


# Publish rule

publish :
		$(CP) $(LIBRARY) $(TTL_LIB)
		$(CP) $(HEADER)  $(TTL_INCLUDE)


## EOF

