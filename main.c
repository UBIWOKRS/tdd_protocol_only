/***************************************************************************//**
 *   @file   main.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "config.h"
#include "ad9361_api.h"
#include "parameters.h"
#include "platform.h"
#ifdef CONSOLE_COMMANDS
#include "command.h"
#include "console.h"
#endif
#ifdef XILINX_PLATFORM
#include <xil_cache.h>
#endif
#if defined XILINX_PLATFORM || defined LINUX_PLATFORM || defined ALTERA_PLATFORM
#include "adc_core.h"
#include "dac_core.h"
#include "xil_io.h"
#endif

#include "protocol.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#ifdef CONSOLE_COMMANDS
extern command	  	cmd_list[];
extern char			cmd_no;
extern cmd_function	cmd_functions[11];
unsigned char		cmd				 =  0;
double				param[5]		 = {0, 0, 0, 0, 0};
char				param_no		 =  0;
int					cmd_type		 = -1;
char				invalid_cmd		 =  0;
char				received_cmd[30] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif

AD9361_InitParam default_init_param = {
	/* Device selection */
		ID_AD9363A,	// dev_sel				// AD9361 -> AD9363A dylee modify
	/* Identification number */
	0,		//id_no
	/* Reference Clock */
	30720000UL,	//reference_clk_rate		// 40000000UL -> 30720000UL dylee modify
	/* Base Configuration */

//	1,		//two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
	0,		//two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable		// dylee change

//	1,		//one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num     // sylee change
//	1,		//one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num     // sylee change
	0,		//one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num
	0,		//one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num
	1,		//frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
	0,		//frequency_division_duplex_independent_mode_enable *** adi,frequency-division-duplex-independent-mode-enable
	0,		//tdd_use_dual_synth_mode_enable *** adi,tdd-use-dual-synth-mode-enable
	0,		//tdd_skip_vco_cal_enable *** adi,tdd-skip-vco-cal-enable
	0,		//tx_fastlock_delay_ns *** adi,tx-fastlock-delay-ns
	0,		//rx_fastlock_delay_ns *** adi,rx-fastlock-delay-ns
	0,		//rx_fastlock_pincontrol_enable *** adi,rx-fastlock-pincontrol-enable
	0,		//tx_fastlock_pincontrol_enable *** adi,tx-fastlock-pincontrol-enable
	0,		//external_rx_lo_enable *** adi,external-rx-lo-enable
	0,		//external_tx_lo_enable *** adi,external-tx-lo-enable
	5,		//dc_offset_tracking_update_event_mask *** adi,dc-offset-tracking-update-event-mask
	6,		//dc_offset_attenuation_high_range *** adi,dc-offset-attenuation-high-range
	5,		//dc_offset_attenuation_low_range *** adi,dc-offset-attenuation-low-range
	0x28,	//dc_offset_count_high_range *** adi,dc-offset-count-high-range
	0x32,	//dc_offset_count_low_range *** adi,dc-offset-count-low-range
	0,		//split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
	MAX_SYNTH_FREF,	//trx_synthesizer_target_fref_overwrite_hz *** adi,trx-synthesizer-target-fref-overwrite-hz
	0,		// qec_tracking_slow_mode_enable *** adi,qec-tracking-slow-mode-enable
	/* ENSM Control */
	0,		//ensm_enable_pin_pulse_mode_enable *** adi,ensm-enable-pin-pulse-mode-enable
	0,		//ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable

	/* LO Control */
	//2400000000UL,	//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
	//2400000000UL,	//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
	//3452320000UL,	//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz	// dylee change
	//3452320000UL,	//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz	// dylee change

	// test version ========================================================================================================
	1840000000UL,	//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz	// dylee change
	1840000000UL,	//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz	// dylee change

	// release version =====================================================================================================
	//1755000000UL,	//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz	// dylee change
	//1755000000UL,	//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz	// dylee change

	1,				//tx_lo_powerdown_managed_enable *** adi,tx-lo-powerdown-managed-enable
	/* Rate & BW Control */
	{983040000, 245760000, 122880000, 61440000, 30720000, 30720000},// rx_path_clock_frequencies[6] *** adi,rx-path-clock-frequencies
	{983040000, 122880000, 122880000, 61440000, 30720000, 30720000},// tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
//	1000000,//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
//	1000000,//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
	15360000,//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz		// dylee change
	15360000,//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz		// dylee change
//	5000000,//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz		// sylee change
//	5000000,//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz		// sylee change

	/* RF Port Control */

//	0,		//rx_rf_port_input_select *** adi,rx-rf-port-input-select
	3,		//rx_rf_port_input_select *** adi,rx-rf-port-input-select		// dylee change 3 ok

	0,		//tx_rf_port_input_select *** adi,tx-rf-port-input-select
	/* TX Attenuation Control */
	10000,	//tx_attenuation_mdB *** adi,tx-attenuation-mdB
	0,		//update_tx_gain_in_alert_enable *** adi,update-tx-gain-in-alert-enable
	/* Reference Clock Control */
	0,		//xo_disable_use_ext_refclk_enable *** adi,xo-disable-use-ext-refclk-enable
	{8, 5920},	//dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
	CLKOUT_DISABLE,	//clk_output_mode_select *** adi,clk-output-mode-select
	/* Gain Control */
//	2,		//gc_rx1_mode *** adi,gc-rx1-mode  // Original
//	2,		//gc_rx2_mode *** adi,gc-rx2-mode  // Original
	0,		//gc_rx1_mode *** adi,gc-rx1-mode															// sylee change
	0,		//gc_rx2_mode *** adi,gc-rx2-mode															// sylee change
	58,		//gc_adc_large_overload_thresh *** adi,gc-adc-large-overload-thresh
	4,		//gc_adc_ovr_sample_size *** adi,gc-adc-ovr-sample-size
	47,		//gc_adc_small_overload_thresh *** adi,gc-adc-small-overload-thresh
	8,		//gc_dec_pow_measurement_duration *** adi,gc-dec-pow-measurement-duration
	0,		//gc_dig_gain_enable *** adi,gc-dig-gain-enable
	800,	//gc_lmt_overload_high_thresh *** adi,gc-lmt-overload-high-thresh
	704,	//gc_lmt_overload_low_thresh *** adi,gc-lmt-overload-low-thresh
	24,		//gc_low_power_thresh *** adi,gc-low-power-thresh
	15,		//gc_max_dig_gain *** adi,gc-max-dig-gain
	/* Gain MGC Control */
	2,		//mgc_dec_gain_step *** adi,mgc-dec-gain-step
	2,		//mgc_inc_gain_step *** adi,mgc-inc-gain-step
	0,		//mgc_rx1_ctrl_inp_enable *** adi,mgc-rx1-ctrl-inp-enable
	0,		//mgc_rx2_ctrl_inp_enable *** adi,mgc-rx2-ctrl-inp-enable
	0,		//mgc_split_table_ctrl_inp_gain_mode *** adi,mgc-split-table-ctrl-inp-gain-mode
	/* Gain AGC Control */
	10,		//agc_adc_large_overload_exceed_counter *** adi,agc-adc-large-overload-exceed-counter
	2,		//agc_adc_large_overload_inc_steps *** adi,agc-adc-large-overload-inc-steps
	0,		//agc_adc_lmt_small_overload_prevent_gain_inc_enable *** adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable
	10,		//agc_adc_small_overload_exceed_counter *** adi,agc-adc-small-overload-exceed-counter
	4,		//agc_dig_gain_step_size *** adi,agc-dig-gain-step-size
	3,		//agc_dig_saturation_exceed_counter *** adi,agc-dig-saturation-exceed-counter
	1000,	//agc_gain_update_interval_us *** adi,agc-gain-update-interval-us
	0,		//agc_immed_gain_change_if_large_adc_overload_enable *** adi,agc-immed-gain-change-if-large-adc-overload-enable
	0,		//agc_immed_gain_change_if_large_lmt_overload_enable *** adi,agc-immed-gain-change-if-large-lmt-overload-enable
	10,		//agc_inner_thresh_high *** adi,agc-inner-thresh-high
	1,		//agc_inner_thresh_high_dec_steps *** adi,agc-inner-thresh-high-dec-steps
	12,		//agc_inner_thresh_low *** adi,agc-inner-thresh-low
	1,		//agc_inner_thresh_low_inc_steps *** adi,agc-inner-thresh-low-inc-steps
	10,		//agc_lmt_overload_large_exceed_counter *** adi,agc-lmt-overload-large-exceed-counter
	2,		//agc_lmt_overload_large_inc_steps *** adi,agc-lmt-overload-large-inc-steps
	10,		//agc_lmt_overload_small_exceed_counter *** adi,agc-lmt-overload-small-exceed-counter
	5,		//agc_outer_thresh_high *** adi,agc-outer-thresh-high
	2,		//agc_outer_thresh_high_dec_steps *** adi,agc-outer-thresh-high-dec-steps
	18,		//agc_outer_thresh_low *** adi,agc-outer-thresh-low
	2,		//agc_outer_thresh_low_inc_steps *** adi,agc-outer-thresh-low-inc-steps
	1,		//agc_attack_delay_extra_margin_us; *** adi,agc-attack-delay-extra-margin-us
	0,		//agc_sync_for_gain_counter_enable *** adi,agc-sync-for-gain-counter-enable
	/* Fast AGC */
	64,		//fagc_dec_pow_measuremnt_duration ***  adi,fagc-dec-pow-measurement-duration
	260,	//fagc_state_wait_time_ns ***  adi,fagc-state-wait-time-ns
	/* Fast AGC - Low Power */
	0,		//fagc_allow_agc_gain_increase ***  adi,fagc-allow-agc-gain-increase-enable
	5,		//fagc_lp_thresh_increment_time ***  adi,fagc-lp-thresh-increment-time
	1,		//fagc_lp_thresh_increment_steps ***  adi,fagc-lp-thresh-increment-steps
	/* Fast AGC - Lock Level (Lock Level is set via slow AGC inner high threshold) */
	1,		//fagc_lock_level_lmt_gain_increase_en ***  adi,fagc-lock-level-lmt-gain-increase-enable
	5,		//fagc_lock_level_gain_increase_upper_limit ***  adi,fagc-lock-level-gain-increase-upper-limit
	/* Fast AGC - Peak Detectors and Final Settling */
	1,		//fagc_lpf_final_settling_steps ***  adi,fagc-lpf-final-settling-steps
	1,		//fagc_lmt_final_settling_steps ***  adi,fagc-lmt-final-settling-steps
	3,		//fagc_final_overrange_count ***  adi,fagc-final-overrange-count
	/* Fast AGC - Final Power Test */
	0,		//fagc_gain_increase_after_gain_lock_en ***  adi,fagc-gain-increase-after-gain-lock-enable
	/* Fast AGC - Unlocking the Gain */
	0,		//fagc_gain_index_type_after_exit_rx_mode ***  adi,fagc-gain-index-type-after-exit-rx-mode
	1,		//fagc_use_last_lock_level_for_set_gain_en ***  adi,fagc-use-last-lock-level-for-set-gain-enable
	1,		//fagc_rst_gla_stronger_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable
	5,		//fagc_optimized_gain_offset ***  adi,fagc-optimized-gain-offset
	10,		//fagc_rst_gla_stronger_sig_thresh_above_ll ***  adi,fagc-rst-gla-stronger-sig-thresh-above-ll
	1,		//fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable
	1,		//fagc_rst_gla_engergy_lost_goto_optim_gain_en ***  adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable
	10,		//fagc_rst_gla_engergy_lost_sig_thresh_below_ll ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll
	8,		//fagc_energy_lost_stronger_sig_gain_lock_exit_cnt ***  adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt
	1,		//fagc_rst_gla_large_adc_overload_en ***  adi,fagc-rst-gla-large-adc-overload-enable
	1,		//fagc_rst_gla_large_lmt_overload_en ***  adi,fagc-rst-gla-large-lmt-overload-enable
	0,		//fagc_rst_gla_en_agc_pulled_high_en ***  adi,fagc-rst-gla-en-agc-pulled-high-enable
	0,		//fagc_rst_gla_if_en_agc_pulled_high_mode ***  adi,fagc-rst-gla-if-en-agc-pulled-high-mode
	64,		//fagc_power_measurement_duration_in_state5 ***  adi,fagc-power-measurement-duration-in-state5
	/* RSSI Control */
	1,		//rssi_delay *** adi,rssi-delay
	1000,	//rssi_duration *** adi,rssi-duration
	3,		//rssi_restart_mode *** adi,rssi-restart-mode
	0,		//rssi_unit_is_rx_samples_enable *** adi,rssi-unit-is-rx-samples-enable
	1,		//rssi_wait *** adi,rssi-wait
	/* Aux ADC Control */
	256,	//aux_adc_decimation *** adi,aux-adc-decimation
	30720000UL,	//aux_adc_rate *** adi,aux-adc-rate		// 40000000UL -> 30720000UL dylee modify
	/* AuxDAC Control */
	1,		//aux_dac_manual_mode_enable ***  adi,aux-dac-manual-mode-enable
	0,		//aux_dac1_default_value_mV ***  adi,aux-dac1-default-value-mV
	0,		//aux_dac1_active_in_rx_enable ***  adi,aux-dac1-active-in-rx-enable
	0,		//aux_dac1_active_in_tx_enable ***  adi,aux-dac1-active-in-tx-enable
	0,		//aux_dac1_active_in_alert_enable ***  adi,aux-dac1-active-in-alert-enable
	0,		//aux_dac1_rx_delay_us ***  adi,aux-dac1-rx-delay-us
	0,		//aux_dac1_tx_delay_us ***  adi,aux-dac1-tx-delay-us
	0,		//aux_dac2_default_value_mV ***  adi,aux-dac2-default-value-mV
	0,		//aux_dac2_active_in_rx_enable ***  adi,aux-dac2-active-in-rx-enable
	0,		//aux_dac2_active_in_tx_enable ***  adi,aux-dac2-active-in-tx-enable
	0,		//aux_dac2_active_in_alert_enable ***  adi,aux-dac2-active-in-alert-enable
	0,		//aux_dac2_rx_delay_us ***  adi,aux-dac2-rx-delay-us
	0,		//aux_dac2_tx_delay_us ***  adi,aux-dac2-tx-delay-us
	/* Temperature Sensor Control */
	256,	//temp_sense_decimation *** adi,temp-sense-decimation
	1000,	//temp_sense_measurement_interval_ms *** adi,temp-sense-measurement-interval-ms
	0xCE,	//temp_sense_offset_signed *** adi,temp-sense-offset-signed
	1,		//temp_sense_periodic_measurement_enable *** adi,temp-sense-periodic-measurement-enable
	/* Control Out Setup */
	0xFF,	//ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
	0,		//ctrl_outs_index *** adi,ctrl-outs-index
	/* External LNA Control */
	0,		//elna_settling_delay_ns *** adi,elna-settling-delay-ns
	0,		//elna_gain_mdB *** adi,elna-gain-mdB
	0,		//elna_bypass_loss_mdB *** adi,elna-bypass-loss-mdB
	0,		//elna_rx1_gpo0_control_enable *** adi,elna-rx1-gpo0-control-enable
	0,		//elna_rx2_gpo1_control_enable *** adi,elna-rx2-gpo1-control-enable
	0,		//elna_gaintable_all_index_enable *** adi,elna-gaintable-all-index-enable
	/* Digital Interface Control */
	0,		//digital_interface_tune_skip_mode *** adi,digital-interface-tune-skip-mode
	0,		//digital_interface_tune_fir_disable *** adi,digital-interface-tune-fir-disable
	1,		//pp_tx_swap_enable *** adi,pp-tx-swap-enable
	1,		//pp_rx_swap_enable *** adi,pp-rx-swap-enable
	0,		//tx_channel_swap_enable *** adi,tx-channel-swap-enable
	0,		//rx_channel_swap_enable *** adi,rx-channel-swap-enable
	1,		//rx_frame_pulse_mode_enable *** adi,rx-frame-pulse-mode-enable
	0,		//two_t_two_r_timing_enable *** adi,2t2r-timing-enable
	0,		//invert_data_bus_enable *** adi,invert-data-bus-enable
	0,		//invert_data_clk_enable *** adi,invert-data-clk-enable
	0,		//fdd_alt_word_order_enable *** adi,fdd-alt-word-order-enable
	0,		//invert_rx_frame_enable *** adi,invert-rx-frame-enable
	0,		//fdd_rx_rate_2tx_enable *** adi,fdd-rx-rate-2tx-enable
	0,		//swap_ports_enable *** adi,swap-ports-enable
	0,		//single_data_rate_enable *** adi,single-data-rate-enable
	1,		//lvds_mode_enable *** adi,lvds-mode-enable
	0,		//half_duplex_mode_enable *** adi,half-duplex-mode-enable
	0,		//single_port_mode_enable *** adi,single-port-mode-enable
	0,		//full_port_enable *** adi,full-port-enable
	0,		//full_duplex_swap_bits_enable *** adi,full-duplex-swap-bits-enable
	0,		//delay_rx_data *** adi,delay-rx-data
	0,		//rx_data_clock_delay *** adi,rx-data-clock-delay
	4,		//rx_data_delay *** adi,rx-data-delay
	7,		//tx_fb_clock_delay *** adi,tx-fb-clock-delay
	0,		//tx_data_delay *** adi,tx-data-delay
#ifdef ALTERA_PLATFORM
	300,	//lvds_bias_mV *** adi,lvds-bias-mV
#else
	150,	//lvds_bias_mV *** adi,lvds-bias-mV
#endif
	1,		//lvds_rx_onchip_termination_enable *** adi,lvds-rx-onchip-termination-enable
	0,		//rx1rx2_phase_inversion_en *** adi,rx1-rx2-phase-inversion-enable
	0xFF,	//lvds_invert1_control *** adi,lvds-invert1-control
	0x0F,	//lvds_invert2_control *** adi,lvds-invert2-control
	/* GPO Control */
	0,		//gpo0_inactive_state_high_enable *** adi,gpo0-inactive-state-high-enable
	0,		//gpo1_inactive_state_high_enable *** adi,gpo1-inactive-state-high-enable
	0,		//gpo2_inactive_state_high_enable *** adi,gpo2-inactive-state-high-enable
	0,		//gpo3_inactive_state_high_enable *** adi,gpo3-inactive-state-high-enable
	0,		//gpo0_slave_rx_enable *** adi,gpo0-slave-rx-enable
	0,		//gpo0_slave_tx_enable *** adi,gpo0-slave-tx-enable
	0,		//gpo1_slave_rx_enable *** adi,gpo1-slave-rx-enable
	0,		//gpo1_slave_tx_enable *** adi,gpo1-slave-tx-enable
	0,		//gpo2_slave_rx_enable *** adi,gpo2-slave-rx-enable
	0,		//gpo2_slave_tx_enable *** adi,gpo2-slave-tx-enable
	0,		//gpo3_slave_rx_enable *** adi,gpo3-slave-rx-enable
	0,		//gpo3_slave_tx_enable *** adi,gpo3-slave-tx-enable
	0,		//gpo0_rx_delay_us *** adi,gpo0-rx-delay-us
	0,		//gpo0_tx_delay_us *** adi,gpo0-tx-delay-us
	0,		//gpo1_rx_delay_us *** adi,gpo1-rx-delay-us
	0,		//gpo1_tx_delay_us *** adi,gpo1-tx-delay-us
	0,		//gpo2_rx_delay_us *** adi,gpo2-rx-delay-us
	0,		//gpo2_tx_delay_us *** adi,gpo2-tx-delay-us
	0,		//gpo3_rx_delay_us *** adi,gpo3-rx-delay-us
	0,		//gpo3_tx_delay_us *** adi,gpo3-tx-delay-us
	/* Tx Monitor Control */
	37000,	//low_high_gain_threshold_mdB *** adi,txmon-low-high-thresh
	0,		//low_gain_dB *** adi,txmon-low-gain
	24,		//high_gain_dB *** adi,txmon-high-gain
	0,		//tx_mon_track_en *** adi,txmon-dc-tracking-enable
	0,		//one_shot_mode_en *** adi,txmon-one-shot-mode-enable
	511,	//tx_mon_delay *** adi,txmon-delay
	8192,	//tx_mon_duration *** adi,txmon-duration
	2,		//tx1_mon_front_end_gain *** adi,txmon-1-front-end-gain
	2,		//tx2_mon_front_end_gain *** adi,txmon-2-front-end-gain
	48,		//tx1_mon_lo_cm *** adi,txmon-1-lo-cm
	48,		//tx2_mon_lo_cm *** adi,txmon-2-lo-cm
	/* GPIO definitions */
	-1,		//gpio_resetb *** reset-gpios
	/* MCS Sync */
	-1,		//gpio_sync *** sync-gpios
	-1,		//gpio_cal_sw1 *** cal-sw1-gpios
	-1,		//gpio_cal_sw2 *** cal-sw2-gpios
	/* External LO clocks */
	NULL,	//(*ad9361_rfpll_ext_recalc_rate)()
	NULL,	//(*ad9361_rfpll_ext_round_rate)()
	NULL	//(*ad9361_rfpll_ext_set_rate)()
};

//================================================================================================================================================================================================================

// SY Lee
// Generated with AD9361 Filter Design Wizard 16.1.3
// MATLAB 9.8.0.1396136 (R2020a) Update 3, 07-Jul-2020 10:18:41
// Inputs:
// Data Sample Frequency = 30720000 Hz
// LTE 20MBW

AD9361_RXFIRConfig rx_fir_config = {
	3, // rx
	-6, // rx_gain
	2, // rx_dec
	{-9,-23,-20,-22,12,20,29,-5,-30,-43,-6,40,64,24,-47,-90,-49,49,122,85,-44,-158,-134,29,196,196,-1,-234,-274,-46,270,368,114,-298,-481,-212,315,612,344,-312,-762,-522,282,935,757,-212,-1133,-1074,80,1365,1510,149,-1646,-2146,-551,2016,3173,1310,-2634,-5298,-3194,4431,14435,21470,21470,14435,4431,-3194,-5298,-2634,1310,3173,2016,-551,-2146,-1646,149,1510,1365,80,-1074,-1133,-212,757,935,282,-522,-762,-312,344,612,315,-212,-481,-298,114,368,270,-46,-274,-234,-1,196,196,29,-134,-158,-44,85,122,49,-49,-90,-47,24,64,40,-6,-43,-30,-5,29,20,12,-22,-20,-23,-9}, // rx_coef[128]
	128, // rx_coef_size
	{983040000,491520000,245760000,122880000,61440000,30720000}, // rx_path_clks[6]
	19365514 // rx_bandwidth
};

AD9361_TXFIRConfig tx_fir_config = {
	3, // tx
	0, // tx_gain
	2, // tx_int
	{-5,0,4,23,36,39,18,-13,-36,-26,11,48,46,-2,-60,-72,-16,69,104,43,-74,-142,-82,71,184,136,-57,-228,-205,26,272,291,25,-311,-395,-101,342,519,210,-357,-663,-361,349,829,564,-307,-1022,-841,216,1251,1225,-44,-1536,-1793,-268,1930,2728,873,-2622,-4718,-2416,4788,13982,20380,20380,13982,4788,-2416,-4718,-2622,873,2728,1930,-268,-1793,-1536,-44,1225,1251,216,-841,-1022,-307,564,829,349,-361,-663,-357,210,519,342,-101,-395,-311,25,291,272,26,-205,-228,-57,136,184,71,-82,-142,-74,43,104,69,-16,-72,-60,-2,46,48,11,-26,-36,-13,18,39,36,23,4,0,-5}, // tx_coef[128]
	128, // tx_coef_size
	{983040000,245760000,245760000,122880000,61440000,30720000}, // tx_path_clks[6]
	19365438 // tx_bandwidth
};

//================================================================================================================================================================================================================

//================================================================================================================================================================================================================

// SY Lee
// Generated with AD9361 Filter Design Wizard 16.1.3
// MATLAB 9.8.0.1396136 (R2020a) Update 3, 07-Jul-2020 12:19:53
// Inputs:
// Data Sample Frequency = 30720000 Hz
// LTE 5MBW

//AD9361_RXFIRConfig rx_fir_config = {
//	3, // rx
//	6, // rx_gain
//	2, // rx_dec
//	{5631,-17480,27007,-27949,24215,-19683,15942,-12850,10390,-8568,6945,-5970,4782,-4355,3380,-3311,2448,-2604,1823,-2089,1404,-1679,1127,-1335,954,-1033,847,-770,777,-558,731,-409,672,-318,590,-289,471,-311,327,-365,161,-426,11,-474,-107,-476,-150,-436,-102,-315,21,-115,228,143,475,465,721,788,976,1038,1202,1212,1326,1318,1318,1326,1212,1202,1038,976,788,721,465,475,143,228,-115,21,-315,-102,-436,-150,-476,-107,-474,11,-426,161,-365,327,-311,471,-289,590,-318,672,-409,731,-558,777,-770,847,-1033,954,-1335,1127,-1679,1404,-2089,1823,-2604,2448,-3311,3380,-4355,4782,-5970,6945,-8568,10390,-12850,15942,-19683,24215,-27949,27007,-17480,5631}, // rx_coef[128]
//	128, // rx_coef_size
//	{983040000,491520000,245760000,122880000,61440000,30720000}, // rx_path_clks[6]
//	5533004 // rx_bandwidth
//};

//AD9361_TXFIRConfig tx_fir_config = {
//	3, // tx
//	0, // tx_gain
//	2, // tx_int
//	{3272,-11435,17277,-17976,14606,-12402,9074,-8258,5457,-5814,3206,-4405,1791,-3552,901,-2966,377,-2485,135,-2019,110,-1525,245,-1006,484,-495,755,-35,988,316,1121,506,1107,514,925,327,591,-11,147,-437,-335,-856,-757,-1171,-1031,-1277,-1062,-1126,-792,-669,-220,67,635,1023,1662,2122,2737,3211,3748,4145,4553,4812,5032,5123,5123,5032,4812,4553,4145,3748,3211,2737,2122,1662,1023,635,67,-220,-669,-792,-1126,-1062,-1277,-1031,-1171,-757,-856,-335,-437,147,-11,591,327,925,514,1107,506,1121,316,988,-35,755,-495,484,-1006,245,-1525,110,-2019,135,-2485,377,-2966,901,-3552,1791,-4405,3206,-5814,5457,-8258,9074,-12402,14606,-17976,17277,-11435,3272}, // tx_coef[128]
//	128, // tx_coef_size
//	{983040000,245760000,245760000,122880000,61440000,30720000}, // tx_path_clks[6]
//	5648252 // tx_bandwidth
//};

//================================================================================================================================================================================================================






struct ad9361_rf_phy *ad9361_phy;
#ifdef FMCOMMS5
struct ad9361_rf_phy *ad9361_phy_b;
#endif

uint8_t ch = 0;
struct rf_rssi *rssi;
uint32_t *rssi_db_x_1000;

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{

#ifdef XILINX_PLATFORM
	Xil_ICacheEnable();
	Xil_DCacheEnable();
#endif

#ifdef ALTERA_PLATFORM
	if (altera_bridge_init()) {
		printf("Altera Bridge Init Error!\n");
		return -1;
	}
#endif

	// NOTE: The user has to choose the GPIO numbers according to desired
	// carrier board.
	default_init_param.gpio_resetb = GPIO_RESET_PIN;
#ifdef FMCOMMS5
	default_init_param.gpio_sync = GPIO_SYNC_PIN;
	default_init_param.gpio_cal_sw1 = GPIO_CAL_SW1_PIN;
	default_init_param.gpio_cal_sw2 = GPIO_CAL_SW2_PIN;
	default_init_param.rx1rx2_phase_inversion_en = 1;
#else
	default_init_param.gpio_sync = -1;
	default_init_param.gpio_cal_sw1 = -1;
	default_init_param.gpio_cal_sw2 = -1;
#endif

#ifdef LINUX_PLATFORM
	gpio_init(default_init_param.gpio_resetb);
#else
	gpio_init(GPIO_DEVICE_ID);
#endif
	gpio_direction(default_init_param.gpio_resetb, 1);


	mdelay(3000);		// dylee add



	spi_init(SPI_DEVICE_ID, 1, 0);

	if (AD9364_DEVICE)
		default_init_param.dev_sel = ID_AD9364;
	if (AD9363A_DEVICE)
		default_init_param.dev_sel = ID_AD9363A;

#if defined FMCOMMS5 || defined ADI_RF_SOM || defined ADI_RF_SOM_CMOS
	default_init_param.xo_disable_use_ext_refclk_enable = 1;
#endif

#ifdef ADI_RF_SOM_CMOS
	default_init_param.swap_ports_enable = 1;
	default_init_param.lvds_mode_enable = 0;
	default_init_param.lvds_rx_onchip_termination_enable = 0;
	default_init_param.full_port_enable = 1;
	default_init_param.digital_interface_tune_fir_disable = 1;
#endif



	ad9361_init(&ad9361_phy, &default_init_param);



	mdelay(2000);		// dylee add



	ad9361_set_tx_fir_config(ad9361_phy, tx_fir_config);
	ad9361_set_rx_fir_config(ad9361_phy, rx_fir_config);

	ad9361_set_rx_gain_control_mode(ad9361_phy, 0, 0);
	console_print("rx1_gc_mode=%d\n", 0);

	ad9361_set_rx_rf_gain(ad9361_phy, 0, 40);
	console_print("rx1_rf_gain=%d\n", 40);

	ad9361_set_rx_fir_en_dis(ad9361_phy, 1);
	console_print("rx_fir_en=%d\n", 1);

	mdelay(1000);		// dylee add




#ifdef FMCOMMS5
#ifdef LINUX_PLATFORM
	gpio_init(default_init_param.gpio_sync);
#endif
	gpio_direction(default_init_param.gpio_sync, 1);
	default_init_param.id_no = 1;
	default_init_param.gpio_resetb = GPIO_RESET_PIN_2;
#ifdef LINUX_PLATFORM
	gpio_init(default_init_param.gpio_resetb);
#endif
	default_init_param.gpio_sync = -1;
	default_init_param.gpio_cal_sw1 = -1;
	default_init_param.gpio_cal_sw2 = -1;
	default_init_param.rx_synthesizer_frequency_hz = 2300000000UL;
	default_init_param.tx_synthesizer_frequency_hz = 2300000000UL;
	gpio_direction(default_init_param.gpio_resetb, 1);
	ad9361_init(&ad9361_phy_b, &default_init_param);

	ad9361_set_tx_fir_config(ad9361_phy_b, tx_fir_config);
	ad9361_set_rx_fir_config(ad9361_phy_b, rx_fir_config);
#endif

#ifndef AXI_ADC_NOT_PRESENT
#if defined XILINX_PLATFORM || defined LINUX_PLATFORM || defined ALTERA_PLATFORM
#ifdef DAC_DMA_EXAMPLE
#ifdef FMCOMMS5
	dac_init(ad9361_phy_b, DATA_SEL_DMA, 0);
#endif
	dac_init(ad9361_phy, DATA_SEL_DMA, 1);
#else
#ifdef FMCOMMS5
	dac_init(ad9361_phy_b, DATA_SEL_DDS, 0);
#endif
	dac_init(ad9361_phy, DATA_SEL_DDS, 1);
#endif
#endif
#endif

#ifdef FMCOMMS5
	ad9361_do_mcs(ad9361_phy, ad9361_phy_b);
#endif

#ifndef AXI_ADC_NOT_PRESENT
#if (defined XILINX_PLATFORM || defined ALTERA_PLATFORM) && \
	(defined ADC_DMA_EXAMPLE || defined ADC_DMA_IRQ_EXAMPLE)
    // NOTE: To prevent unwanted data loss, it's recommended to invalidate
    // cache after each adc_capture() call, keeping in mind that the
    // size of the capture and the start address must be alinged to the size
    // of the cache line.
	mdelay(1000);
	adc_capture(16384, ADC_DDR_BASEADDR);
#ifdef XILINX_PLATFORM
#ifdef FMCOMMS5
	Xil_DCacheInvalidateRange(ADC_DDR_BASEADDR, 16384 * 16);
#else
	Xil_DCacheInvalidateRange(ADC_DDR_BASEADDR,
			ad9361_phy->pdata->rx2tx2 ? 16384 * 8 : 16384 * 4);
#endif
#endif
#endif
#endif

#ifdef CONSOLE_COMMANDS
	get_help(NULL, 0);

	while(1)
	{
		console_get_command(received_cmd);
		invalid_cmd = 0;
		for(cmd = 0; cmd < cmd_no; cmd++)
		{
			param_no = 0;
			cmd_type = console_check_commands(received_cmd, cmd_list[cmd].name,
											  param, &param_no);
			if(cmd_type == UNKNOWN_CMD)
			{
				invalid_cmd++;
			}
			else
			{
				cmd_list[cmd].function(param, param_no);
			}
		}
		if(invalid_cmd == cmd_no)
		{

			// receive and send protocol data test
			char test_protocol_data[MAXDATA];
			test_make_protocol_data(test_protocol_data);

#ifdef DEBUG_PROTOCOL
			for(int i  = 0; i < MAXDATA; i++)
			{
				console_print("test protocol data[%d] = 0x%x\n", i, test_protocol_data[i]);
			}
#endif

			eco_data_protocol(test_protocol_data, MAXDATA);

		}

	}
#endif

	printf("Done.\n");

#ifdef TDD_SWITCH_STATE_EXAMPLE
	uint32_t ensm_mode;
	if (!ad9361_phy->pdata->fdd) {
		if (ad9361_phy->pdata->ensm_pin_ctrl) {
			gpio_direction(GPIO_ENABLE_PIN, 1);
			gpio_direction(GPIO_TXNRX_PIN, 1);
			gpio_set_value(GPIO_ENABLE_PIN, 0);
			gpio_set_value(GPIO_TXNRX_PIN, 0);
			udelay(10);
			ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
			printf("TXNRX control - Alert: %s\n",
					ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
			mdelay(1000);

			if (ad9361_phy->pdata->ensm_pin_pulse_mode) {
				while(1) {
					gpio_set_value(GPIO_TXNRX_PIN, 0);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - RX: %s\n",
							ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - Alert: %s\n",
							ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_TXNRX_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - TX: %s\n",
							ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - Alert: %s\n",
							ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					mdelay(1000);
				}
			} else {
				while(1) {
					gpio_set_value(GPIO_TXNRX_PIN, 0);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - RX: %s\n",
							ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_ENABLE_PIN, 0);
					udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - Alert: %s\n",
							ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_TXNRX_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - TX: %s\n",
							ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_ENABLE_PIN, 0);
					udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - Alert: %s\n",
							ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					mdelay(1000);
				}
			}
		} else {
			while(1) {
				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_RX);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - RX: %s\n",
						ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
				mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_ALERT);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - Alert: %s\n",
						ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
				mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_TX);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - TX: %s\n",
						ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
				mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_ALERT);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - Alert: %s\n",
						ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
				mdelay(1000);
			}
		}
	}
#endif




#ifdef XILINX_PLATFORM
	Xil_DCacheDisable();
	Xil_ICacheDisable();
#endif

#ifdef ALTERA_PLATFORM
	if (altera_bridge_uninit()) {
		printf("Altera Bridge Uninit Error!\n");
		return -1;
	}
#endif

	return 0;
}
