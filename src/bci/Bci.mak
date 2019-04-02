##
## Bci.mak
##
## Makefile for Boltwood Cloud Interface (BCI).
##


# Toolset

RM = rm -f
CP = cp -f
CC = cc -c
LN = cc
#CC_OPT = -pedantic -ansi -D_GNU_SOURCE
#LN_OPT = -pedantic -ansi -lm -lrt
CC_OPT = 
LN_OPT = 


# Declare phony targets

.PHONY : all
.PHONY : clean


# Labels

APP = Bci
MAK = Bci.mak
HDR = BciPrivate.h


# Implicit rules

.c.o : 
	$(CC) $(CC_OPT) $< -o $@


# Objects 

OBJS = BciMain.o BciInterface.o BciSerial.o BciOutput.o


# Rule to build all

all : $(APP)


# Rule to build exectable

$(APP) : $(MAK) $(OBJS)
	$(LN) -o $(APP) $(OBJS) $(LN_OPT)


# Rule to build objects

BciMain.o        : BciMain.c BciPrivate.h

BciInterface.o   : BciInterface.c BciPrivate.h

BciSerial.o      : BciSerial.c BciPrivate.h

BciOutput.o      : BciOutput.c BciPrivate.h


# Clean rule

clean : 
	$(RM) $(OBJS)
	$(RM) $(APP)

   
### EOF

