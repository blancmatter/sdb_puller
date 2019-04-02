##
## File name:
##    Htg.mak
##
## Purpose:
##    To generate look-up tables for SDB datum IDs and all other enumerations.
##
## Description:
##    Create, run and then delete executables to print out the values of both 
##    the enumerated SDB datum IDs, and all enumerations. These tables are then
##    sorted to produce a user readable form and space-padded to produce fixed-
##    length index versions. The two look-up tables created are 'IdTable' and 
##    'FullTable'.
##
## Authors:
##    asp: Andrzej S. Piascik
##    mjf: Martyn J. Ford
##
## Version:
##    $Id: Htg.mak,v 0.19 2007/07/18 14:28:57 mjf Exp $
##
## History:
##    $Log: Htg.mak,v $
##    Revision 0.19  2007/07/18 14:28:57  mjf
##    Addition of process states for on-mount sub-systems.
##    New 'EnumTable' created containing the definitions of enumerated data
##    reported to the SDB.
##
##    Revision 0.18  2006/07/10 15:32:14  mjf
##    Run utilities in current directory by using './'.
##
##    Revision 0.17  2005/06/17 09:01:21  mjf
##    Addition of VEN ENL and linear encoder states.
##
##    Revision 0.16  2004/06/21 09:41:57  mjf
##    Addition of Nasmyth+, Nasmyth- and Prime Focus.
##
##    Revision 0.15  2004/05/20 10:01:24  mjf
##    Include the new SPT commands in FullTable as well as IdTable.
##
##    Revision 0.14  2004/05/20 09:23:57  mjf
##    Addition of new T2-408 SPLC commands.
##
##    Revision 0.13  2004/04/19 14:26:37  mjf
##    Removal of 'Inc.h'.
##
##    Revision 0.12  2004/04/06 15:36:20  sxf
##    AMT datum IDs use the 'T' rather than the 'R'.
##
##    Revision 0.11  2003/12/12 12:01:08  mjf
##    Publish the StatusTable.idx file.
##
##    Revision 0.10  2003/12/12 10:57:39  mjf
##    Addition of PLC System Status to FullTable.
##
##    Revision 0.9  2003/02/04 15:24:26  dws
##    Added Status table generation.
##
##    Revision 0.8  2002/04/17 16:53:04  djm
##    Added the HtiGenBinary utility in order to compile the
##    binary tables.
##
##    Revision 0.7  2002/03/22 16:44:03  mjf
##    Addition of autoguider.
##
##    Revision 0.6  2002/01/07 11:13:55  mjf
##    Telescope definitions now to be held in TtlSystem.h.
##
##    Revision 0.5  2001/12/18 14:10:04  rdl
##    Added lines to build entries for Axis Motion Test tool (AMT).
##
##    Revision 0.4  2001/12/14 11:28:17  mjf
##    Change of EPT ShutterState to DataState.
##
##    Revision 0.3  2001/12/13 16:00:47  mkb
##    Added ModBus server public header file.
##
##    Revision 0.2  2001/10/10 12:29:07  mjf
##    Addition of data IDs for node agents, and CAN added into mirror support.
##
##    Revision 0.1  2001/09/19 12:41:49  mjf
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


# Define names of utilities

HDR2ID = $(TTL_UTIL)/HtiHdr2Id
FIXRECLEN = $(TTL_UTIL)/HtiFixRecLen
GENBINARY = $(TTL_UTIL)/HtiGenBinary


# Define names of OS utilities

SORT = sort


# Rule to build all

all : IdTable UnitsTable FullTable StatusTable EnumTable


# Rule to build ID-only table

IdTable :
	# Create a C module and function header
	$(HDR2ID) start > $@.c
	### Parse each header file to extract the SDB datum ID enumerations
	# MCS applications
	$(HDR2ID) $(TTL_INCLUDE)/Mcb.h       MCB eMcbDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       MCP ePmmDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       CHB eChbDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       UI1 eUitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       UI2 eUitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       AI1 eAitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       AI2 eAitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       AI3 eAitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       MIT eMitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sdb.h       SDB eSdbDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM0 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM1 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM2 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM3 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM4 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM5 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM6 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM7 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM8 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM9 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC0 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC1 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC2 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC3 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC4 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC5 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC6 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC7 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC8 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC9 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ept.h       EPT eEptDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Wms.h       WMS eWmsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       SPT eSptDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       SPT eSpiDataId_e        >> $@.c
	# TCS
	$(HDR2ID) $(TTL_INCLUDE)/Tcs.h       TCS eTcsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tcs.h       RCS eRcsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tcs.h       AGS eAgsDataId_e        >> $@.c
	# ACN : AZR
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h AZR eMcpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       AZR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       AZR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       AZR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       AZR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       AZR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       AZR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       AZR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       AZR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       AZR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       AZR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       AZR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       AZR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       AZR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       AZR eCflDataId_e        >> $@.c
	# ACN : ELR
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h ELR eMcpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       ELR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       ELR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       ELR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       ELR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       ELR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       ELR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       ELR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       ELR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       ELR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       ELR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       ELR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       ELR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       ELR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       ELR eCflDataId_e        >> $@.c
	# ACN : CSR
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h CSR eMcpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       CSR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       CSR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       CSR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       CSR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       CSR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       CSR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       CSR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       CSR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       CSR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       CSR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       CSR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       CSR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       CSR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       CSR eCflDataId_e        >> $@.c
	# ACN : NDR
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h NDR eMcpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       NDR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       NDR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       NDR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       NDR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       NDR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       NDR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       NDR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       NDR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       NDR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NDR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       NDR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       NDR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       NDR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       NDR eCflDataId_e        >> $@.c
	# ACN : NWR
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h NWR eMcpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       NWR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       NWR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       NWR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       NWR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       NWR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       NWR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       NWR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       NWR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       NWR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NWR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       NWR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       NWR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       NWR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       NWR eCflDataId_e        >> $@.c
	# ACN : PFR
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h PFR eMcpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       PFR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       PFR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       PFR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       PFR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       PFR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       PFR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       PFR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       PFR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       PFR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       PFR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       PFR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       PFR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       PFR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       PFR eCflDataId_e        >> $@.c
	# AMN : Auxiliary Mechanisms
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h OMR eMcpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       OMR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       OMR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       OMR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       OMR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       OMR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h       OMR eAgdDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agf.h       OMR eAgfDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ams.h       OMR eAmsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Can.h       OMR eCanDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfd.h       OMR eSfdDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfp.h       OMR eSfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Smf.h       OMR eSmfDataId_e        >> $@.c   
	$(HDR2ID) $(TTL_INCLUDE)/Mbs.h       OMR eMbsDataId_e        >> $@.c   
	# AMN : Primary Mirror Support (reported by OMR)
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h MSR eMcpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       MSR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       MSR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       MSR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       MSR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       MSR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Pms.h       MSR ePmsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sif.h       MSR esifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Can.h       MSR eCanDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Acm.h       MSR eAcmDataId_e        >> $@.c
	# Autoguider
	$(HDR2ID) $(TTL_INCLUDE)/Ags.h       AGS eAgsDataId_e        >> $@.c
	# AMT : Axis Motion Tool (test tool uses AZR, ELR & CSR for Cil Address)
	$(HDR2ID) $(TTL_INCLUDE)/Amt.h       AZT eAmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amt.h       ELT eAmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amt.h       CST eAmtDataId_e        >> $@.c
	# Add the end code to the automatically generated C source code
	$(HDR2ID) end >> $@.c
	# Build the generated C source code
	$(CC) $(CC_OPT_NON_ANSI) $@.c -o $@.o
	$(LN) $(LN_OPT) -o $@.out $@.o
	# Run the executable to generate the user readable file
	./$@.out > $@.tmp
	# Sort the user readable file into alphabetical order
	$(SORT) -o $@.txt $@.tmp
	# Generate a lookup file by setting each record to be the same length
	$(FIXRECLEN) $@.txt > $@.idx
	# Generate the binary file
	$(GENBINARY) $@.idx $@.idb
	# Clean up working files
	$(RM) $@.c $@.o $@.out 


# Rule to build SDB units-only table

UnitsTable :
	# Create a C module and function header
	$(HDR2ID) start > $@.c
	### Parse SDB header file to extract the SDB units enumeration
	$(HDR2ID) $(TTL_INCLUDE)/Sdb.h       SDB eSdbUnit_e   >> $@.c
	# Add the end code to the automatically generated C source code
	$(HDR2ID) end >> $@.c
	# Build the generated C source code
	$(CC) $(CC_OPT_NON_ANSI) $@.c -o $@.o
	$(LN) $(LN_OPT) -o $@.out $@.o
	# Run the executable to generate the user readable file
	./$@.out > $@.tmp
	# Sort the user readable file into alphabetical order
	$(SORT) -o $@.txt $@.tmp
	# Generate a lookup file by setting each record to be the same length
	$(FIXRECLEN) $@.txt > $@.idx
	# Generate the binary file
	$(GENBINARY) $@.idx $@.idb
	# Clean up working files
	$(RM) $@.c $@.o $@.out 


# Rule to build full table

FullTable :
	# Create a C module and function header
	$(HDR2ID) start > $@.c
	### Parse each header file to extract the SDB datum ID enumerations
	# MCS applications
	$(HDR2ID) $(TTL_INCLUDE)/Mcb.h       MCB eMcbDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       MCP ePmmDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       CHB eChbDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       UI1 eUitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       UI2 eUitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       AI1 eAitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       AI2 eAitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       AI3 eAitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       MIT eMitDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sdb.h       SDB eSdbDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM0 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM1 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM2 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM3 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM4 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM5 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM6 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM7 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM8 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h       CM9 eCmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC0 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC1 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC2 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC3 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC4 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC5 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC6 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC7 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC8 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h       CC9 eCctDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ept.h       EPT eEptDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Wms.h       WMS eWmsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       SPT eSptDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       SPT eSpiDataId_e        >> $@.c
	# TCS
	$(HDR2ID) $(TTL_INCLUDE)/Tcs.h       TCS eTcsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tcs.h       RCS eRcsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tcs.h       AGS eAgsDataId_e        >> $@.c
	# ACN : AZR
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       AZR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       AZR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       AZR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       AZR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       AZR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       AZR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       AZR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       AZR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       AZR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       AZR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       AZR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       AZR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       AZR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       AZR eCflDataId_e        >> $@.c
	# ACN : ELR
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       ELR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       ELR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       ELR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       ELR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       ELR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       ELR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       ELR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       ELR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       ELR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       ELR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       ELR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       ELR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       ELR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       ELR eCflDataId_e        >> $@.c
	# ACN : CSR
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       CSR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       CSR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       CSR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       CSR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       CSR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       CSR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       CSR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       CSR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       CSR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       CSR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       CSR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       CSR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       CSR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       CSR eCflDataId_e        >> $@.c
	# ACN : NDR
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       NDR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       NDR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       NDR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       NDR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       NDR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       NDR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       NDR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       NDR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       NDR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NDR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       NDR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       NDR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       NDR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       NDR eCflDataId_e        >> $@.c
	# ACN : NWR
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       NWR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       NWR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       NWR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       NWR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       NWR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       NWR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       NWR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       NWR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       NWR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NWR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       NWR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       NWR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       NWR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       NWR eCflDataId_e        >> $@.c
	# ACN : PFR
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       PFR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       PFR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       PFR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       PFR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       PFR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Aes.h       PFR eAesDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       PFR eAmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asc.h       PFR eAscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Tfp.h       PFR eTfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       PFR eVenDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mif.h       PFR eMifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       PFR eNmcDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       PFR eNscDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       PFR eCflDataId_e        >> $@.c
	# AMN : Auxiliary Mechanisms
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       OMR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       OMR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       OMR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       OMR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       OMR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h       OMR eAgdDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agf.h       OMR eAgfDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ams.h       OMR eAmsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Can.h       OMR eCanDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfd.h       OMR eSfdDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfp.h       OMR eSfpDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Smf.h       OMR eSmfDataId_e        >> $@.c   
	$(HDR2ID) $(TTL_INCLUDE)/Mbs.h       OMR eMbsDataId_e        >> $@.c   
	# AMN : Primary Mirror Support (reported by OMR)
	$(HDR2ID) $(TTL_INCLUDE)/Oid.h       MSR eOidDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nci.h       MSR eNciDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nco.h       MSR eNcoDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Rep.h       MSR eRepDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h       MSR eAgnDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Pms.h       MSR ePmsDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sif.h       MSR esifDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Can.h       MSR eCanDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Acm.h       MSR eAcmDataId_e        >> $@.c
	# Autoguider
	$(HDR2ID) $(TTL_INCLUDE)/Ags.h       AGS eAgsDataId_e        >> $@.c
	### Parse SDB header file to extract the SDB units enumeration
	$(HDR2ID) $(TTL_INCLUDE)/Sdb.h       SDB eSdbUnit_e          >> $@.c
	### Parse header files to extract remaining enumerations
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h SYS eSysStatus_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h SYS eSysProcState_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h SYS eMcpDataId_e        >> $@.c
	# MCS applications
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       MCP eMcpAuthState_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       MCP eMcpSysReqIndex_e   >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       MCP eMcpSysReqOffset_e  >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       MCP eMcpSysRequest_e    >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ept.h       EPT eEptDataState_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ept.h       EPT eEptDmdState_e      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h EPT eSysTelescope_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Wms.h       WMS eWmsDewPointState_e >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       SPT eSptDmdState_e      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       SPT eSptDataState_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       SPT eSptSysStatus_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h SPT eSysTelescope_e     >> $@.c
	# ACN : AZR
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       AZR eCflCommand_e       >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       AZR eAmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       AZR eAmcControlMode_e   >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       AZR eNmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       AZR eNscTopLevelStates_e>> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       AZR eVenDirection_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       AZR eVenEnlState_e      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       AZR eVenLinEncState_e   >> $@.c
	# ACN : ELR
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       ELR eCflCommand_e       >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       ELR eAmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       ELR eAmcControlMode_e   >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       ELR eNmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       ELR eNscTopLevelStates_e>> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       ELR eVenDirection_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       ELR eVenEnlState_e      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       ELR eVenLinEncState_e   >> $@.c
	# ACN : CSR
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       CSR eCflCommand_e       >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       CSR eAmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       CSR eAmcControlMode_e   >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       CSR eNmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       CSR eNscTopLevelStates_e>> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       CSR eVenDirection_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       CSR eVenEnlState_e      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       CSR eVenLinEncState_e   >> $@.c
	# ACN : NDR
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       NDR eCflCommand_e       >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       NDR eAmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       NDR eAmcControlMode_e   >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       NDR eNmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       NDR eNscTopLevelStates_e>> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NDR eVenDirection_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NDR eVenEnlState_e      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NDR eVenLinEncState_e   >> $@.c
	# ACN : NWR
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       NWR eCflCommand_e       >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       NWR eAmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       NWR eAmcControlMode_e   >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       NWR eNmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       NWR eNscTopLevelStates_e>> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NWR eVenDirection_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NWR eVenEnlState_e      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       NWR eVenLinEncState_e   >> $@.c
	# ACN : PFR
	$(HDR2ID) $(TTL_INCLUDE)/Cfl.h       PFR eCflCommand_e       >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       PFR eAmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       PFR eAmcControlMode_e   >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       PFR eNmcStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       PFR eNscTopLevelStates_e>> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       PFR eVenDirection_e     >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       PFR eVenEnlState_e      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       PFR eVenLinEncState_e   >> $@.c
	# AMN : AMN
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h       OMR eAgdStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agf.h       OMR eAgfStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfd.h       OMR eSfdStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfp.h       OMR eSfpStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Smf.h       OMR eSmfStates_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h       OMR eAgdIRPos_e         >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h       OMR eAgdDSPos_e         >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfd.h       OMR eSfdPos_e           >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfp.h       OMR eSfpPort_e          >> $@.c
	# AMN : Mirror Support
	$(HDR2ID) $(TTL_INCLUDE)/Sif.h       MSR eSifOpStates_e      >> $@.c
	# Autoguider
	$(HDR2ID) $(TTL_INCLUDE)/Ags.h       AGS eAgsState_e         >> $@.c
	# AMT : Axis Motion Tool (test tool uses AZR, ELR & CSR for Cil Address)
	$(HDR2ID) $(TTL_INCLUDE)/Amt.h       AZR eAmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amt.h       ELR eAmtDataId_e        >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amt.h       CSR eAmtDataId_e        >> $@.c
	# Add the end code to the automatically generated C source code
	$(HDR2ID) end >> $@.c
	# Build the generated C source code
	$(CC) $(CC_OPT_NON_ANSI) $@.c -o $@.o
	$(LN) $(LN_OPT) -o $@.out $@.o
	# Run the executable to generate the user readable file
	./$@.out > $@.tmp
	# Sort the user readable file into alphabetical order
	$(SORT) -o $@.txt $@.tmp
	# Generate a lookup file by setting each record to be the same length
	$(FIXRECLEN) $@.txt > $@.idx
	# Generate the binary file
	$(GENBINARY) $@.idx $@.idb
	# Clean up working files
	$(RM) $@.c $@.o $@.out 

StatusTable:
	# Create a C module and function header
	$(HDR2ID) start > $@.c
	### Parse each header file to extract the Status error code enumerations.
	$(HDR2ID) $(TTL_INCLUDE)/Agc.h  AGC  eAgcStatus_e   -package    >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h  AGD  eAgdStatus_e   -package    >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agf.h  AGF  eAgfStatus_e   -package    >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agg.h  AGG  eAggStatus_e   -package    >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agn.h  AGN  eAgnStatus_e   -package    >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ags.h  AGS  eAgsStatus_e   -package    >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agt.h  AGT  eAgentStatus_e -package    >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Asi.h  ASI  eGenComErrCodes_e -package >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cct.h  CCT eCctStatus_e  -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cmt.h  CMT  eCmtStatus_e -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Df1.h  DFL  eDf1Status_e -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ept.h  EPT  eEptStatus_e -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Iet.h  IET  eIetStatus_e -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h  MCP  ePmmStatus_e -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h  CHB  eChbStatus_e -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h  UIT  eUitStatus_e -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h  AIT  eAitStatus_e -package      >> $@.c 
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h  MIT  eMitStatus_e -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sdb.h  SDB eSdbStatus_e  -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfd.h  SFD eSfdStatus_e  -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfr.h  SFR eSfrStatus_e  -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfp.h  SFP eSfpStatus_e  -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h  SPT eSptStatus_e  -package      >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Wms.h  WMS eWmsStatus_e  -package      >> $@.c
	# Add the end code to the automatically generated C source code
	$(HDR2ID) end >> $@.c
	# Build the generated C source code
	$(CC) $(CC_OPT_NON_ANSI) $@.c -o $@.o
	$(LN) $(LN_OPT) -o $@.out $@.o
	# Run the executable to generate the user readable file
	./$@.out > $@.tmp
	# Sort the user readable file into alphabetical order
	$(SORT) -o $@.txt $@.tmp
	# Generate a lookup file by setting each record to be the same length
	$(FIXRECLEN) $@.txt > $@.idx
	# Generate the binary file
	$(GENBINARY) $@.idx $@.idb -package
	# Clean up working files
	$(RM) $@.c $@.o $@.out $@.txt

EnumTable:
	# Create a C module and function header
	$(HDR2ID) start > $@.c
	### Parse each header file to extract the State enumerations.
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h E_SDB_PROCSTATE_UNITS     eSysProcState_e        -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ept.h       E_SDB_SHUTTER_STATE_UNITS eEptDmdState_e         -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Cil.h       E_SDB_CIL_ID_UNITS        eCilNames_e            -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       E_SDB_AUTH_STATE_UNITS    eMcpAuthState_e        -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Mcp.h       E_SDB_SYSREQ_UNITS        eMcpSysRequest_e       -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ept.h       E_SDB_EPT_DATA_UNITS      eEptDataState_e        -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Iet.h       E_SDB_IET_DATA_UNITS      eIetDataState_e        -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/TtlSystem.h E_SDB_TELESCOPE_UNITS     eSysTelescope_e        -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       E_SDB_PLC_SYSSTATUS_UNITS eSptSysStatus_e        -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ags.h       E_SDB_AGSTATE_UNITS       eAgsState_e            -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       E_SDB_PLC_ALARM_UNITS     eSptAlarmLevel_e       -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       E_SDB_SPLC_DEMAND_UNITS   eSptDmdState_e         -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       E_SDB_AXIS_TOP_UNITS      eNscTopLevelStates_e   -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nsc.h       E_SDB_AXIS_USER_UNITS     eNscUserAxisStates_e   -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Nmc.h       E_SDB_NODE_MOTION_UNITS   eNmcStates_e           -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Amc.h       E_SDB_AXIS_MOTION_UNITS   eAmcStates_e           -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       E_SDB_LIN_ENC_STATE_UNITS eVenLinEncState_e      -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       E_SDB_ENL_STATE_UNITS     eVenEnlState_e         -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Ven.h       E_SDB_HOMING_DIR_UNITS    eVenDirection_e        -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Hhe.h       E_SDB_ENC_STATE_UNITS     eHheEncoderState_e     -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Hhe.h       E_SDB_ENC_REF_STATE_UNITS eHheEncoderHomeState_e -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Hhe.h       E_SDB_ENC_STRENGTH_UNITS  eHheAmplitudeStatus_e  -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h       E_SDB_AGD_STATE_UNITS     eAgdStates_e           -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agf.h       E_SDB_AGF_STATE_UNITS     eAgfStates_e           -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfd.h       E_SDB_SFD_STATE_UNITS     eSfdStates_e           -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfp.h       E_SDB_SFP_STATE_UNITS     eSfpStates_e           -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Smf.h       E_SDB_SMF_STATE_UNITS     eSmfStates_e           -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h       E_SDB_IR_FILTER_POS_UNITS eAgdIRPos_e            -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Agd.h       E_SDB_DARKSLIDE_POS_UNITS eAgdDSPos_e            -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfd.h       E_SDB_SFD_POS_UNITS       eSfdPos_e              -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sfp.h       E_SDB_SFP_PORT_UNITS      eSfpPort_e             -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Acm.h       E_SDB_ACM_STATE_UNITS     eAcmStates_e           -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Sif.h       E_SDB_SIF_STATE_UNITS     eSifOpStates_e         -verbatim >> $@.c
	$(HDR2ID) $(TTL_INCLUDE)/Spt.h       E_SDB_PLC_HPLCSTATUS_UNITS eSptHplcStatus_e      -verbatim >> $@.c
	# Add the end code to the automatically generated C source code
	$(HDR2ID) end >> $@.c
	# Build the generated C source code
	$(CC) $(CC_OPT_NON_ANSI) $@.c -o $@.o
	$(LN) $(LN_OPT) -o $@.out $@.o
	# Run the executable to generate the user readable file
	./$@.out > $@.tmp
	# Sort the user readable file into alphabetical order
	$(SORT) -o $@.txt $@.tmp
	# Generate a lookup file by setting each record to be the same length
	$(FIXRECLEN) $@.txt > $@.idx
	# Generate the binary file
	$(GENBINARY) $@.idx $@.idb
	# Clean up working files
	$(RM) $@.c $@.o $@.out $@.txt


# Clean rule

clean : 
	$(RM) IdTable.c    IdTable.o    IdTable.out 
	$(RM) UnitsTable.c UnitsTable.o UnitsTable.out 
	$(RM) FullTable.c  FullTable.o  FullTable.out 
	$(RM) StatusTable.c  StatusTable.o  StatusTable.out 
	$(RM) EnumTable.c EnumTable.o EnumTable.out 

# Publish rule

publish :
	  $(CP) IdTable.idx    $(TTL_ETC)
	  $(CP) IdTable.idb    $(TTL_ETC)
	  $(CP) UnitsTable.idx $(TTL_ETC)
	  $(CP) UnitsTable.idb $(TTL_ETC)
	  $(CP) FullTable.idx  $(TTL_ETC)
	  $(CP) FullTable.idb  $(TTL_ETC)
	  $(CP) StatusTable.idb  $(TTL_ETC)
	  $(CP) StatusTable.idx  $(TTL_ETC)
	  $(CP) EnumTable.idb $(TTL_ETC)
	  $(CP) EnumTable.idx $(TTL_ETC)


### EOF

