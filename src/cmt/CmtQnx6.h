/*******************************************************************************
** Module Name:
**    CmtQnx6.h
**
** Purpose:
**    Private header file for neutrino hardware monitoring.
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.01 1-Nov-2000.
**
** Author(s):
**   Steve Foale (sxf)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2004
**
** Version:
**    $Id: CmtQnx6.h,v 0.2 2006/03/21 11:21:56 sxf Exp $
**
** History:
**    $Log: CmtQnx6.h,v $
**    Revision 0.2  2006/03/21 11:21:56  sxf
**    Added NTP lock functionaity for QNX6, Linux and VMS.
**
**    Revision 0.1  2004/01/15 16:14:11  sxf
**    Initial version.
**
**
**
*******************************************************************************/

#include <sys/neutrino.h>
#include <sys/times.h>

typedef long nid_t;      /* Used for newtork IDs */

struct _osinfo {
	short unsigned  cpu_speed,  	/* NA A PC is around 960             	*/
					num_procs,  	/* NA Max number of processes          	*/
					tick_size,  	/* Tick size, in microsecond units  	*/
					version,    	/* Version number of QNX * 100      	*/
					timesel,    	/* NA Segment in which the time is kept	*/
					totmemk,    	/* Total memory in the system       	*/
					freememk;   	/* Free memory in the system 	       	*/

	char            primary_monitor,    /* NA monitor type     				*/
					secondary_monitor;  /* NA monitor type     				*/
	short unsigned  machinesel;     /* NA */
	unsigned short  disksel;        /* NA pointer to _diskinfo structure 	*/
	unsigned long   diskoff;        /* NA */
	unsigned long   ssinfo_offset;  /* NA */
	short unsigned  ssinfo_sel,     /* NA */
					microkernel_size;	/* NA */
	char            release,        /* NA Release letter               		*/
					zero2;
	long            sflags;         /* System flags, as defined below   	*/
	nid_t           nodename;       /* Logical node number of this cpu
									   Note that this is the QNX 4 network
									   id as obtained by sending to the
									   migration name manager process and
									   not a QNX Neutrino node descriptor 	*/
	long            cpu,            /* Processor type 486, 586, ...     	*/
					fpu;            /* NA Floating-point unit 287, 387,... 	*/
	char            machine[16],    /* Machine name                     	*/
					bootsrc,        /* NA 'F'loppy 'H'ard disk 'N'etwork   	*/
					zero3[9];
	short unsigned  num_names,      /* NA Maximum number of names      		*/
					num_timers,     /* NA Maximum number of timers     		*/
					num_sessions,   /* NA Maximum number of sessions   		*/
					num_handlers,   /* NA Maximum number of interrupt handlers */
					reserve64k,     /* NA Relocation offset 				*/
					num_semaphores, /* MA */
					prefix_len,     /* NA */
					zero4[4],
					max_nodes,      /* NA Number of nodes you are licensed for */
					proc_freemem,   /* NA */
					cpu_loadmask,   /* NA */
					fd_freemem,     /* NA */
					ldt_freemem,    /* NA */
					num_fds[3],     /* NA Number of fd's					*/
					pidmask,        /* NA Process ID bit mask				*/
					name_freemem;	/* NA */
	long unsigned   top_of_mem;		/* NA */
	long unsigned   freepmem;		/* Free physical memory					*/
	long unsigned   freevmem;       /* NA */
	long unsigned   totpmem;        /* Total physical memory				*/
	long unsigned   totvmem;        /* NA */
	long unsigned   cpu_features;   /* Contains CPU speed in Mhz			*/
	short unsigned  zero5[13];
};

/* System flag definitions */

#define _PSF_PROTECTED          0x0001	/* Running in protected mode.	*/
#define _PSF_NDP_INSTALLED      0x0002 	/* An 80x87 is installed.		*/
#define _PSF_EMULATOR_INSTALLED 0x000c 	/* An 80x87 emulator is running	*/
#define _PSF_32BIT_KERNEL       0x1000 	/* 32-bit kernel is being used.	*/
#define _PSF_PCI_BIOS           0x2000 	/* A PCI BIOS is present.		*/


struct _psinfo {
	pid_t       	pid,
					pid_zero,
					blocked_on,
					blocked_on_zero,
					pid_group,
					pid_group_zero;
	long        	flags;     		/* NA */
	short int   	rgid,
					ruid,
					egid,
					euid;
	long        	sp_reg;			/* NA */
	short unsigned  ss_reg;			/* NA */
	long            magic_off;		/* NA */
	short unsigned  magic_sel,		/* NA */
					ldt,			/* NA */
					umask;
	long            signal_ignore,
					signal_pending,	/* NA */
					signal_mask,
					signal_off;		/* NA */
	short unsigned  signal_sel;		/* NA */
	char            state,
					zero0,
					zero0a,
					priority,
					max_priority,
					sched_algorithm;
	short unsigned  sid;
	nid_t           sid_nid;		/* NA */
	short unsigned  zero1[5];
	union {
		struct {
			pid_t       	father,
							father_zero,
							son,
							son_zero, 
							brother,
							brother_zero,
							debugger,		/* NA */
							debugger_zero,
							mpass_pid,		/* NA */
							mpass_pid_zero;
			short unsigned  mpass_sel,		/* NA */
							mpass_flags;	/* NA */

			char            name[100];
			short unsigned  links;			/* NA */
			time_t          file_time;		/* NA */

			short unsigned  nselectors;		/* NA */
			time_t          start_time;		/* NA */
			struct tms      times;
			short unsigned  mxcount;		/* NA */
			short unsigned  zero2[7];
		} proc;
		struct {
			pid_t       local_pid,          /* NA */
						local_pid_zero,
						remote_pid,         /* NA */
						remote_pid_zero,
						remote_vid,         /* NA */
						remote_vid_zero;
			nid_t       remote_nid;         /* NA */
			short unsigned  vidseg,         /* NA */
							links;          /* NA */
			char            substate,       /* NA */
							zero_v1;
			short unsigned  zero2[49];
		} vproc;
		struct {
			short unsigned           count, /* NA */
									 zero2[50];
		} mproc;
	} un;
	short unsigned zero3[12];
};

#define I_CMT_NTP_LOCK_FILE "/ttl/sw/data/TtlNtpLocked.log"

/* End of file CmtQnx6.h */
