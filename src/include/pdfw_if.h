//===========================================================================
//
// NAME:    pdl_if.h
//
// DESCRIPTION:
//
//          PowerDAQ QNX driver firmware interface definition
//
// AUTHOR:  Alex Ivchenko
//
// DATE:    12-APR-2000
//
// REV:     0.8
//
// R DATE:  
//
// HISTORY:
//
//      Rev 0.8,     12-MAR-2000,     Initial version.
//
//
//---------------------------------------------------------------------------
//      Copyright (C) 2000 United Electronic Industries, Inc.
//      All rights reserved.
//---------------------------------------------------------------------------

#ifndef __PDL_IF_H__
#define __PDL_IF_H__

#ifdef __cplusplus
extern "C"
{
#endif

void pd_main();

// Start dealing with boards
extern int pd_find_devices(void);

// Cleanup boards after use
extern int pd_clean_devices(void);

// Get board capabilities
#if 0
extern int pd_get_adapter_info(int board, Adapter_Info* pAdInfo);
#endif

// pdl_fwi.c
u32 pd_dsp_get_status(int board);
u32 pd_dsp_get_flags(int board);
void pd_dsp_set_flags(int board, u32 new_flags);
void pd_dsp_command(int board, int command);
void pd_dsp_cmd_no_ret(int board, u16 command);
void pd_dsp_write(int board, u32 data);
u32 pd_dsp_read(int board);
u32 pd_dsp_read_x(int board) ;
u32 pd_dsp_cmd_ret_ack(int board, u16 wCmd);
u32 pd_dsp_cmd_ret_value(int board, u16 wCmd);
u32 pd_dsp_read_ack(int board);
u32 pd_dsp_write_ack(int board, u32 dwValue);
u32 pd_dsp_cmd_write_ack(int board, u16 wCmd, u32 dwValue);
u32 pd_dsp_int_status(int board);
u32 pd_dsp_acknowledge_interrupt(int board);
int pd_dsp_startup(int board);
void  pd_init_calibration(int board);
int pd_reset_dsp(int board);
int pd_download_firmware_bootstrap(int board);
int pd_reset_board(int board);
int pd_download_firmware(int board);
int pd_echo_test(int board);
int pd_dsp_reg_write(int board, u32 reg, u32 data);
int pd_dsp_reg_read(int board, u32 reg, u32* data);

// pdl_ain.c
int pd_ain_set_config(int, u32, u32, u32);
int pd_ain_set_cv_clock(int board, u32 clock_divisor);
int pd_ain_set_cl_clock(int board, u32 clock_divisor);
int pd_ain_set_channel_list(int board, u32 num_entries, u32 list[]);
int pd_ain_set_events (int board, u32 dwEvents);
int pd_ain_get_status(int board, u32* status);
int pd_ain_sw_start_trigger(int board);
int pd_ain_sw_stop_trigger(int board);
int pd_ain_set_enable_conversion(int board, int enable);
int pd_ain_get_value(int board, u16* value);
int pd_ain_set_ssh_gain(int board, u32 dwCfg);
int pd_ain_get_samples(int board, int max_samples, u16* buffer );
int pd_ain_reset(int board);
int pd_ain_sw_cl_start(int board);
int pd_ain_sw_cv_start(int board);
int pd_ain_reset_cl(int board);
int pd_ain_clear_data(int board);
int pd_ain_flush_fifo(int board, int from_isr);
int pd_ain_get_xfer_samples(int board, int size, uint16_t* buffer);
int pd_ain_set_xfer_size(int board, u32 size);
int pd_ain_get_BM_ctr(int board, u32 dwCh, u32* pBurstCtr, u32* pFrameCtr);
int pd_ain_set_BM_ctr(int board, u32 dwCh, u32* pdwChList);


ssize_t pd_ain_read_scan(
   int board,
   const char *buffer,
   size_t count );  // function for read() from ain

// pdl_aio.c
int pd_register_daq_buffer(int board, u32 SubSystem,
                           u32 ScanSize, u32 FrameSize, u32 NumFrames,
                           int user_shm_handle, int bWrap);
int pd_unregister_daq_buffer(int board, u32 SubSystem, int *user_shm_handle);
int pd_clear_daq_buffer(int board, int subsystem);
int pd_ain_async_init(int board, PTAinAsyncCfg pAInCfg);
int pd_ain_async_term(int board);
int pd_ain_async_start(int board);
int pd_ain_async_stop(int board);
int pd_ain_async_retrieve(int board);
int pd_ain_get_scans(int board, PTScan_Info pScanInfo);
int pd_aout_async_init(int board, PTAinAsyncCfg pAInCfg);
int pd_aout_async_term(int board);
int pd_aout_async_start(int board);
int pd_aout_async_stop(int board);
int pd_aout_get_scans(int board, PTScan_Info pScanInfo);
int pd_get_buf_status(int board, int subsystem, PTBuf_Info pDaqBuf);
int pd_aout_dmaSet(int board, u32 offset, u32 count, u32 source);
int pd_aout_put_xbuf(int board, u32 NumToCopy, u32 *UserBufCopied);


// pdl_ao.c
int pd_aout_set_config(int board, u32 config, u32 posttrig);
int pd_aout_set_cv_clk(int board, u32 dwClkDiv);
int pd_aout_set_events(int board, u32 dwEvents);
int pd_aout_get_status(int board, u32* dwStatus);
int pd_aout_set_enable_conversion(int board, u32 dwEnable);
int pd_aout_sw_start_trigger(int board);
int pd_aout_sw_stop_trigger(int board);
int pd_aout_sw_cv_start(int board);
int pd_aout_clear_data(int board);
int pd_aout_reset(int board);
int pd_aout_put_value(int board, u32 dwValue);
int pd_aout_put_block(int board, u32 dwNumValues, u32* pdwBuf, u32* pdwCount);
       
int pd_aout_write_scan(
   int board,
   const char *buffer,
   int count);  // function for write() from aout
   
// pdl_dio.c
int pd_din_set_config(int board, u32 config);
int pd_din_set_event(int board, u32 events);
int pd_din_clear_events(int board, u32 events);
int pd_din_read_inputs(int board, u32 *pdwValue);
int pd_din_clear_data(int board);
int pd_din_reset(int board);
int pd_din_status(int board, u32 *pdwValue);
int pd_dout_write_outputs(int board, uint32_t val);
int pd_dio256_write_output(int board, uint32_t cmd, uint32_t val);
int pd_dio256_read_input(int board, uint32_t cmd, uint32_t* val);
int pd_dio_dmaSet(int board, u32 offset, u32 count, u32 source);
int pd_dio256_setIntrMask(int board);
int pd_dio256_getIntrData(int board);
int pd_dio256_intrEnable(int board, u32 enable);


int pd_din_read(int board, char* buffer, int count);
int pd_dout_write(int board, const char* buffer, int count);
int pd_dsp_reg_read(int board, u32 reg, u32 *data);
int pd_dsp_reg_write(int board, u32 reg, u32 data);

// pdl_uct.c
int pd_uct_set_config(int board, u32 config);
int pd_uct_set_event(int board, u32 events);
int pd_uct_clear_event(int board, u32 events);
int pd_uct_get_status(int board, u32* status);
int pd_uct_write(int board, u32 value);
int pd_uct_read(int board, u32 config, u32* value);
int pd_uct_set_sw_gate(int board, u32 gate_level);
int pd_uct_sw_strobe(int board);
int pd_uct_reset(int board);

// pdl_event.c
int pd_enable_events(int board, PTEvents pEvents);
int pd_disable_events(int board, PTEvents pEvents);
int pd_set_user_events(int board, u32 subsystem, u32 events);
int pd_clear_user_events(int board, u32 subsystem, u32 events);
int pd_get_user_events(int board, u32 subsystem, u32* events);
int pd_immediate_update(int board);
void pd_debug_show_events (TEvents *Event, char* msg);

// pdl_brd.c
int pd_adapter_enable_interrupt(int board, u32 val);
int pd_adapter_acknowledge_interrupt(int board);
int pd_adapter_get_board_status(int board, PTEvents pEvent);
int pd_adapter_set_board_event1(int board, u32 dwEvents);
int pd_adapter_set_board_event2(int board, u32 dwEvents);
int pd_adapter_eeprom_read(int board, u32 dwMaxSize, uint16_t *pwReadBuf);
int pd_adapter_eeprom_write(int board, u32 dwBufSize, u16* pwWriteBuf);
int pd_cal_dac_write(int board, u32 dwCalDACValue);
int pd_adapter_test_interrupt(int board);

// pdl_int.c
void pd_stop_and_disable_ain(int board);
void pd_process_pd_ain_get_samples(int board, int bFHFState);
void pd_process_ain_move_samples(int board, u32 page, u32 numready);
void pd_stop_and_disable_aout(int board);
void pd_process_pd_aout_put_samples(int board, int bFHFState);
void pd_process_driver_events(int board, PTEvents pEvents);
int pd_notify_user_events(int board, PTEvents pNewFwEvents);
void pd_process_events(int board);

// pdl_init.c
void  pd_init_pd_board(int board);
int pd_register_user_isr(int board, TUser_isr user_isr, void* user_param);
int pd_unregister_user_isr(int board);
int pd_interrupt_attach(int board);
int pd_interrupt_detach(int board);

int pd_notify_event(int board, PD_SUBSYSTEM ss, int event);
int pd_sleep_on_event(int board, PD_SUBSYSTEM, int event, int timeoutms);

int pd_alloc_contig_memory(int board, AllocContigMem *allocMemory); 
void pd_dealloc_contig_memory(int board, AllocContigMem *pDeallocMemory);

int pd_wait_for_event(int board, TSynchSS *synch, int timeoutms);
int pd_set_event(int board, TSynchSS *synch);

void* pd_malloc(u32 size);
void pd_free(void *buf, u32 size);
u32 pd_virt_to_phys(void *addr);
u32 pd_readl(char* addr);
void pd_writel(u32 val, char* addr);

void* pd_isr_thread(void *data);
int pd_isr_serve_board(int board);
const struct sigevent* pd_isr(void *area, int id);
void pd_bottom_half(void *data)
;

// pd2_dao.c
int _PdDIOReset(int board);
int _PdDIOEnableOutput(int board, u32 dwRegMask);
int _PdDIOLatchAll(int board, u32 dwRegister);
int _PdDIOSimpleRead(int board, u32 dwRegister, u32 *pdwValue);
int _PdDIOSimpleWrite(int board, u32 dwRegister, u32 dwValue);
int _PdDIORead(int board, u32 dwRegister, u32 *pdwValue);
int _PdDIOWrite(int board, u32 dwRegister, u32 dwValue);
int _PdDIOPropEnable(int board, u32 dwRegMask);
int _PdDIOExtLatchEnable(int board, u32 dwRegister, int bEnable);
int _PdDIOExtLatchRead(int board, u32 dwRegister, int *bLatch);
int _PdDIOIntrEnable(int boart, u32 dwEnable);
int _PdDIOSetIntrMask(int board, u32 *pdwIntMask);
int _PdDIOGetIntrData(int board, u32 *dwIntFata, u32 *dwEdgeData);

int _PdAO32Reset(int board);
int _PdAO32Write(int board, u16 wChannel, u16 wValue);
int _PdAO32WriteHold(int board, u16 wChannel, u16 wValue);
int _PdAO32Update(int board);
int _PdAO32SetUpdateChannel(int board, u16 wChannel, int bEnable);

int pd_ain_raw_to_volts(int board, DWORD dwMode, WORD* rawData, double *fVoltage, DWORD dwCount);

#ifdef __cplusplus
}
#endif

#endif // _PDL_IF_H
