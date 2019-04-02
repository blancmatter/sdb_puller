/*******************************************************************************
** Module Name:
**    CmtCpuVMS.h
**
** Purpose:
**    Header file for use in module CmtInterfaceVMS.c and contains the
**    processor specific definitions for VMS.
**
** Description:   
**    This code has been obtained from DECUS open-source and hence not been 
**    written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.01 01-Nov-2000.
**
** Author(s):
**    Philip Ottewell (amadeus)
**    Andrzej S. Piascik (asp) 
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: CmtCpuVms.h,v 0.2 2001/01/04 11:21:46 asp Exp $
**
** History:
**    $Log: CmtCpuVms.h,v $
**    Revision 0.2  2001/01/04 11:21:46  asp
**    Port to Linux and VMS added
**
**    Revision 0.1  2000/11/01 17:33:32  asp
**    Initial version
**
**
*******************************************************************************/

typedef struct cpu 
{       
long int cpu$l_curpcb;       
long int cpu$l_slot_va;      
short int cpu$w_size;
char cpu$b_type;       
char cpu$b_subtype;      
long int cpu$l_state;       
long int cpu$l_cpumtx;       
long int cpu$l_cur_pri;       
long int cpu$l_work_req;       
long int cpu$l_phy_cpuid;      
long int cpu$l_cpuid_mask;       
long int cpu$l_busywait;       
long int cpu$q_swiqfl[12];       
long int cpu$l_psfl;       
long int cpu$l_psbl; 

#ifdef __DECC 
# pragma message save 
# pragma message disable portable 
#endif      

variant_union 
{         
  long int cpu$q_work_fqfl[2];         
  long int cpu$q_work_ifq[2];       
} ignore_this_1; 

#ifdef __DECC 
# pragma message restore 
#endif       
long int cpu$l_zeroed_page_spte;       
long int cpu$l_zeroed_page_va;       
long int cpu$l_zeroed_page_state[2];       
long int cpu$q_phy_sys_hwpcb[2]; 
#ifdef __DECC 
# pragma message save 
# pragma message disable portable 
#endif       

variant_union 
{         
  long int cpu$q_sys_hwpcb[2];         
  long int cpu$q_sys_ksp[2];
} ignore_this_2;

#ifdef __DECC 
# pragma message restore 
#endif       
long int cpu$q_sys_esp[2];      
long int cpu$q_sys_ssp[2];       
long int cpu$q_sys_usp[2];       
long int cpu$q_sys_ptbr[2];       
long int cpu$q_sys_asn[2];       
long int cpu$q_sys_astsr_asten[2];       
long int cpu$q_sys_fen_datfx[2];       
long int cpu$q_sys_cc[2];       
long int cpu$q_sys_unq[2];       
long int cpu$q_sys_pal_rsvd[12]; 

#ifdef __DECC 
# pragma message save 
# pragma message disable portable 
#endif       

variant_union
{         
  long int cpu$q_term_hwpcb[2];         
  long int cpu$q_term_ksp[2];       
} ignore_this_3;

#ifdef __DECC 
# pragma message restore 
#endif       

long int cpu$q_term_esp[2];      
long int cpu$q_term_ssp[2];       
long int cpu$q_term_usp[2];       
long int cpu$q_term_ptbr[2];       
long int cpu$q_term_asn[2];       
long int cpu$q_term_astsr_asten[2];       
long int cpu$q_term_fen_datfx[2];       
long int cpu$q_term_cc[2];       
long int cpu$q_term_unq[2];       
long int cpu$q_term_pal_rsvd[12];       
long int cpu$q_phy_term_hwpcb[2];       
long int cpu$q_saved_pcbb[2];       
long int cpu$q_scbb[2];      
long int cpu$q_sisr[2];       
long int cpu$q_bc_ksp[2];       
long int cpu$q_bc_esp[2];      
long int cpu$q_bc_ssp[2];       
long int cpu$q_bc_usp[2];       
long int cpu$q_bc_ptbr[2];       
long int cpu$q_bc_asn[2];       
long int cpu$q_bc_astsr_asten[2];       
long int cpu$q_bc_fen[2];       
long int cpu$q_bc_cc[2];       
long int cpu$q_bc_unq[2];       
long int cpu$q_bc_pal_rsvd[12];       
long int cpu$q_bc_r0[2];      
long int cpu$q_bc_r1[2];       
long int cpu$q_bc_r2[2];       
long int cpu$q_bc_r3[2];      
long int cpu$q_bc_r4[2];       
long int cpu$q_bc_r5[2];       
long int cpu$q_bc_r6[2];       
long int cpu$q_bc_r7[2];       
long int cpu$q_bc_r8[2];      
long int cpu$q_bc_r9[2];       
long int cpu$q_bc_r10[2];       
long int cpu$q_bc_r11[2];       
long int cpu$q_bc_r12[2];       
long int cpu$q_bc_r13[2];       
long int cpu$q_bc_r14[2];       
long int cpu$q_bc_r15[2];       
long int cpu$q_bc_r16[2];       
long int cpu$q_bc_r17[2];       
long int cpu$q_bc_r18[2];       
long int cpu$q_bc_r19[2];       
long int cpu$q_bc_r20[2];       
long int cpu$q_bc_r21[2];       
long int cpu$q_bc_r22[2];       
long int cpu$q_bc_r23[2];       
long int cpu$q_bc_r24[2];       
long int cpu$q_bc_r25[2];       
long int cpu$q_bc_r26[2];       
long int cpu$q_bc_r27[2];       
long int cpu$q_bc_r28[2];       
long int cpu$q_bc_r29[2];       
long int cpu$q_bc_pc[2];       
long int cpu$q_bc_ps[2];      
long int cpu$q_bc_f0[2];       
long int cpu$q_bc_f1[2];       
long int cpu$q_bc_f2[2];       
long int cpu$q_bc_f3[2];       
long int cpu$q_bc_f4[2];      
long int cpu$q_bc_f5[2];       
long int cpu$q_bc_f6[2];       
long int cpu$q_bc_f7[2];       
long int cpu$q_bc_f8[2];       
long int cpu$q_bc_f9[2];      
long int cpu$q_bc_f10[2];       
long int cpu$q_bc_f11[2];       
long int cpu$q_bc_f12[2];       
long int cpu$q_bc_f13[2];       
long int cpu$q_bc_f14[2];       
long int cpu$q_bc_f15[2];       
long int cpu$q_bc_f16[2];       
long int cpu$q_bc_f17[2];       
long int cpu$q_bc_f18[2];       
long int cpu$q_bc_f19[2];       
long int cpu$q_bc_f20[2];       
long int cpu$q_bc_f21[2];       
long int cpu$q_bc_f22[2];       
long int cpu$q_bc_f23[2];       
long int cpu$q_bc_f24[2];       
long int cpu$q_bc_f25[2];       
long int cpu$q_bc_f26[2];       
long int cpu$q_bc_f27[2];       
long int cpu$q_bc_f28[2];       
long int cpu$q_bc_f29[2];       
long int cpu$q_bc_f30[2];       
long int cpu$q_bc_ipl[2];       
long int cpu$q_bc_mces[2];       
long int cpu$q_bc_pcbb[2];       
long int cpu$q_bc_prbr[2];       
long int cpu$q_bc_vptb[2];       
long int cpu$q_bc_scbb[2];       
long int cpu$q_bc_sisr[2];       
long int cpu$q_bc_fpcr[2];       
long int cpu$l_bugcode;       
long int cpu$l_capability;       
long int cpu$q_boot_time[2];       
long int cpu$q_asn[2];       
long int cpu$q_asnseq[2];       
long int cpu$q_kernel[10];      
long int cpu$q_mpsynch[2];       
long int cpu$q_nullcpu[2];       
long int cpu$l_hardaff;       
long int cpu$l_rank_vec;       
long int cpu$l_ipl_vec;      
long int cpu$l_ipl_array[32];       
long int cpu$l_tpointer;       
long int cpu$l_sanity_timer;       
long int cpu$l_sanity_ticks;       
long int cpu$l_flags;       
long int cpu$l_intflags;       
long int cpu$l_sys_stack_base;       
long int cpu$l_sys_stack_limit;       
long int cpu$l_variable_offset;       
long int cpu$l_variable_length;       
long int cpu$l_mchk_mask;       
long int cpu$l_mchk_sp[2];       
long int cpu$il_mchk_crash_area_va_h;       
long int cpu$l_proc_corrected_error_svap;      
long int cpu$l_proc_mchk_abort_svapte[2];       
long int cpu$il_logout_area_va_h;       
long int cpu$l_soft_tick;       
long int cpu$l_time_deviation;       
long int cpu$l_pcsample_buffer;       
long int cpu$l_pcsample_flags;     
} $CPU;


/*******************************************************************************
** End of File Name: CmtCpuVMS.h
*******************************************************************************/

