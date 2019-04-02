################################################################################
##
##       Copyright (c) Telescope Technologies Limited (TTL) 2000.
## 
## Filename: 
##    TtlLabels.mak
##
## Purpose:
##    File of labels for generic TTL software development.
##
## Description:
##    N/A
##
## Traceability to Software Design: 
##    -
##
## Authors:
##    mjf : Martyn J. Ford (TTL)
##
## Version:
##    $Id: TtlLabels.mak,v 0.11 2007/10/04 09:42:57 mjf Exp $
##
## History:
##    $Log: TtlLabels.mak,v $
##    Revision 0.11  2007/10/04 09:42:57  mjf
##    Fix to type for QNX v6 'cp' preserving attributes.
##
##    Revision 0.10  2007/10/04 09:02:47  mjf
##    Linux build options revised for Fedora 7.
##
##    Revision 0.9  2006/11/21 10:15:32  sxf
##    Include POSIX version again for non-patched QNX6.3.0 SP3.
##
##    Revision 0.7  2006/09/29 10:53:09  sxf
##    Add POSIX_VERSION definition and ensure all libraries are linked as static.
##
##    Revision 0.6  2006/07/12 11:01:13  mjf
##    Cygwin support added.
##
##    Revision 0.5  2005/08/01 09:46:19  mjf
##    Default to Linux unless otherwise specified, to cope with Linux
##    variants having slightly differing environment variables.
##
##    Revision 0.4  2005/06/20 15:32:16  mjf
##    Addition of base directory for Linux.
##
##    Revision 0.3  2005/06/17 13:10:00  mjf
##    Link in maths library for Linux.
##
##    Revision 0.2  2004/06/21 17:58:18  mjf
##    Link libraries statically for privity level 1.
##
##    Revision 0.1  2004/04/27 10:58:46  mjf
##    Initial revision.
##
##    Revision 0.8  2001/02/20 10:18:22  mjf
##    Addition of label TTL_UTIL for in-house utilities.
##
##    Revision 0.7  2001/01/19 15:32:09  mjf
##    Remove the '-zu' switch from the CC_OPT_HW_ISR.
##
##    Revision 0.6  2001/01/19 14:17:59  mjf
##    Addition of compilation rule for hardware interrupts.
##
##    Revision 0.5  2001/01/05 12:21:24  mjf
##    Addition of default libraries (mqueue), and copy command.
##
##    Revision 0.4  2000/11/27 15:32:59  mjf
##    Removal of debug switch for defaults.
##
##    Revision 0.3  2000/11/14 17:19:02  asp
##    restored -pedantic check to Linux.
##
##    Revision 0.2  2000/11/14 16:09:59  asp
##    Fine tune port to Linux - OSTYPE changed to linux, -pedantic removed from CC
##
##    Revision 0.1  2000/11/08 15:45:37  mjf
##    Initial revision.
##
##
################################################################################

##
## Locations of directories - assume Linux and then change if not.
##

TTL_BASE                   = /ttl/sw

ifeq ($(SYSNAME),nto)
TTL_BASE                   = /ttl/sw
endif

ifeq ($(SYSNAME),qnx4)
TTL_BASE                   = /opt/ttl
endif


TTL_INCLUDE                = $(TTL_BASE)/include
TTL_BIN                    = $(TTL_BASE)/bin
TTL_LIB                    = $(TTL_BASE)/lib
TTL_ETC                    = $(TTL_BASE)/etc
TTL_UTIL                   = $(TTL_BASE)/util


##
## Toolset
##

TTL_QNX_CC                 = cc -c
TTL_QNX_LN                 = cc
TTL_QNX_LB                 = wlib

TTL_LNX_CC                 = cc -c
TTL_LNX_LN                 = cc
TTL_LNX_LB                 = ar

TTL_NTO_CC                 = cc -c
TTL_NTO_LN                 = cc
TTL_NTO_LB                 = ar

TTL_CGW_CC                 = cc -c
TTL_CGW_LN                 = cc
TTL_CGW_LB                 = ar


##
## C Compiler Options
##
## QNX :-
##    -g2          Produce symbolic information for debugger.
##    -I dir       Additionally search for include files in 'dir'.
##    -s           Remove stack overflow checks.
##    -T1          Set privity level 1.
##    -Wc,-ei      Force enums to be of type 'int'.
##    -we          Treat all warnings as errors.
##    -wx          Set the warning level to maximum.
##    -za          Disable language extensions.
##    -ze          Enable language extensions.
##    -zu          Don't assume that SS contains the segment of DGROUP.
## 
## Linux :-
##    

### Default Options ###
TTL_QNX_CC_OPT             = -wx -we -I $(TTL_INCLUDE) -za -Wc,-ei
TTL_LNX_CC_OPT             = -pedantic -I $(TTL_INCLUDE) -ansi -D_GNU_SOURCE
TTL_NTO_CC_OPT             = -I $(TTL_INCLUDE) -ansi -D_POSIX_VERSION=200112L
TTL_CGW_CC_OPT             = -pedantic -I $(TTL_INCLUDE) -ansi

### Non-ANSI C Options ###
TTL_QNX_CC_OPT_NON_ANSI    = -wx -we -I $(TTL_INCLUDE) -ze -Wc,-ei
TTL_LNX_CC_OPT_NON_ANSI    = -pedantic -I $(TTL_INCLUDE) -D_GNU_SOURCE
TTL_NTO_CC_OPT_NON_ANSI    = -I $(TTL_INCLUDE) -D_POSIX_VERSION=200112L
TTL_CGW_CC_OPT_NON_ANSI    = -pedantic -I $(TTL_INCLUDE)

### Privity Level 1 Options - also incorporates non-ANSI ###
TTL_QNX_CC_OPT_PRIVITY_1   = -wx -we -I $(TTL_INCLUDE) -ze -Wc,-ei -T1
TTL_LNX_CC_OPT_PRIVITY_1   = -pedantic -I $(TTL_INCLUDE) -D_GNU_SOURCE
TTL_NTO_CC_OPT_PRIVITY_1   = -I $(TTL_INCLUDE) -D_POSIX_VERSION=200112L
TTL_CGW_CC_OPT_PRIVITY_1   = -pedantic -I $(TTL_INCLUDE)

### Hardware ISR Options - also incorporates non-ANSI & privity level 1 ###
TTL_QNX_CC_OPT_HW_ISR      = -wx -we -I $(TTL_INCLUDE) -ze -Wc,-ei -T1 -s
TTL_LNX_CC_OPT_HW_ISR      = -pedantic -I $(TTL_INCLUDE) -D_GNU_SOURCE
TTL_NTO_CC_OPT_HW_ISR      = -I $(TTL_INCLUDE) -D_POSIX_VERSION=200112L
TTL_CGW_CC_OPT_HW_ISR      = -pedantic -I $(TTL_INCLUDE)


##
## C Linker Options
##
## QNX :-
##    -g2          Produce symbolic information for debugger.
##    -T1          Set privity level 1.
## 
## Linux :-
##    

### Default Options ###
TTL_QNX_LN_OPT           = 
TTL_LNX_LN_OPT           = -pedantic -ansi -lm -lrt
TTL_NTO_LN_OPT           = -static -ansi -lsocket -lm
TTL_CGW_LN_OPT           = -pedantic -ansi -lm

### Privity Level 1 Options ###
TTL_QNX_LN_OPT_PRIVITY_1 = -T1
TTL_LNX_LN_OPT_PRIVITY_1 = -pedantic -ansi -lm -lrt
TTL_NTO_LN_OPT_PRIVITY_1 = -ansi -Bstatic -lsocket -lm 
TTL_CGW_LN_OPT_PRIVITY_1 = -pedantic -ansi -lm


##
## C Librarian Options
##
## QNX :-
##    -n           Always create new library.
## 
## Linux :-
##    

TTL_QNX_LB_OPT            = -n
TTL_LNX_LB_OPT            = -r
TTL_NTO_LB_OPT            = -rs
TTL_CGW_LB_OPT            = -r


##
## Librarian Divider (between library and objects)
##

TTL_QNX_LB_DIV            = +
TTL_LNX_LB_DIV            =
TTL_NTO_LB_DIV            =
TTL_CGW_LB_DIV            = 


##
## C Default Libraries
##

### mqueue ###
TTL_QNX_LIB_MQUEUE       = -l mqueue
TTL_LNX_LIB_MQUEUE       = -l mqueue
TTL_NTO_LIB_MQUEUE       = 
TTL_CGW_LIB_MQUEUE       = -l mqueue


##
## Operating System Utilities
##

### Delete a file (during a 'make clean') ###
TTL_QNX_RM                = rm -f
TTL_LNX_RM                = rm -f
TTL_NTO_RM                = rm -f
TTL_CGW_RM                = rm -f

### Copy a file (force unlink of destination) ###
TTL_QNX_CP                = cp -f
TTL_LNX_CP                = cp -f
TTL_NTO_CP                = cp -f
TTL_CGW_CP                = cp -f

### Copy a file (preserve attributes) ###
TTL_QNX_CP_PRES           = cp -f
TTL_LNX_CP_PRES           = cp --preserve=mode,ownership
TTL_NTO_CP_PRES           = cp -f
TTL_CGW_CP_PRES           = cp --no-preserve=all


##
## Operating System Specific Definitions - assume Linux and then change if not.
##

# Default definitions for Linux operating system

   CC = $(TTL_LNX_CC)
   LN = $(TTL_LNX_LN)
   LB = $(TTL_LNX_LB)
   CC_OPT = $(TTL_LNX_CC_OPT)
   CC_OPT_NON_ANSI =  $(TTL_LNX_CC_OPT_NON_ANSI)
   CC_OPT_PRIVITY_1 = $(TTL_LNX_CC_OPT_PRIVITY_1)
   CC_OPT_HW_ISR = $(TTL_LNX_CC_OPT_HW_ISR)
   LN_OPT = $(TTL_LNX_LN_OPT)
   LN_OPT_PRIVITY_1 = $(TTL_LNX_LN_OPT_PRIVITY_1)
   LB_OPT = $(TTL_LNX_LB_OPT)
   LB_DIV = $(TTL_LNX_LB_DIV)
   LIB_MQ = $(TTL_LNX_LIB_MQUEUE)
   RM = $(TTL_LNX_RM)
   CP = $(TTL_LNX_CP)
   CPPRES = $(TTL_LNX_CP_PRES)

# Definitions for Neutrino operating system

ifeq ($(SYSNAME),nto)
   CC = $(TTL_NTO_CC)
   LN = $(TTL_NTO_LN)
   LB = $(TTL_NTO_LB)
   CC_OPT = $(TTL_NTO_CC_OPT)
   CC_OPT_NON_ANSI =  $(TTL_NTO_CC_OPT_NON_ANSI)
   CC_OPT_PRIVITY_1 = $(TTL_NTO_CC_OPT_PRIVITY_1)
   CC_OPT_HW_ISR = $(TTL_NTO_CC_OPT_HW_ISR)
   LN_OPT = $(TTL_NTO_LN_OPT)
   LN_OPT_PRIVITY_1 = $(TTL_NTO_LN_OPT_PRIVITY_1)
   LB_OPT = $(TTL_NTO_LB_OPT)
   LB_DIV = $(TTL_NTO_LB_DIV)
   LIB_MQ = $(TTL_NTO_LIB_MQUEUE)
   RM = $(TTL_NTO_RM)
   CP = $(TTL_NTO_CP)
   CPPRES = $(TTL_NTO_CP_PRES)
endif 

# Definitions for QNX operating system

ifeq ($(SYSNAME),qnx4)
   CC = $(TTL_QNX_CC)
   LN = $(TTL_QNX_LN)
   LB = $(TTL_QNX_LB)
   CC_OPT = $(TTL_QNX_CC_OPT)
   CC_OPT_NON_ANSI =  $(TTL_QNX_CC_OPT_NON_ANSI)
   CC_OPT_PRIVITY_1 = $(TTL_QNX_CC_OPT_PRIVITY_1)
   CC_OPT_HW_ISR = $(TTL_QNX_CC_OPT_HW_ISR)
   LN_OPT = $(TTL_QNX_LN_OPT)
   LN_OPT_PRIVITY_1 = $(TTL_QNX_LN_OPT_PRIVITY_1)
   LB_OPT = $(TTL_QNX_LB_OPT)
   LB_DIV = $(TTL_QNX_LB_DIV)
   LIB_MQ = $(TTL_QNX_LIB_MQUEUE)
   RM = $(TTL_QNX_RM)
   CP = $(TTL_QNX_CP)
   CPPRES = $(TTL_QNX_CP_PRES)
endif 

# Definitions for Cygwin

ifeq ($(TERM),cygwin)
   CC = $(TTL_CGW_CC)
   LN = $(TTL_CGW_LN)
   LB = $(TTL_CGW_LB)
   CC_OPT = $(TTL_CGW_CC_OPT)
   CC_OPT_NON_ANSI =  $(TTL_CGW_CC_OPT_NON_ANSI)
   CC_OPT_PRIVITY_1 = $(TTL_CGW_CC_OPT_PRIVITY_1)
   CC_OPT_HW_ISR = $(TTL_CGW_CC_OPT_HW_ISR)
   LN_OPT = $(TTL_CGW_LN_OPT)
   LN_OPT_PRIVITY_1 = $(TTL_CGW_LN_OPT_PRIVITY_1)
   LB_OPT = $(TTL_CGW_LB_OPT)
   LB_DIV = $(TTL_CGW_LB_DIV)
   LIB_MQ = $(TTL_CGW_LIB_MQUEUE)
   RM = $(TTL_CGW_RM)
   CP = $(TTL_CGW_CP)
   CPPRES = $(TTL_CGW_CP_PRES)
endif 


### End of file ###

