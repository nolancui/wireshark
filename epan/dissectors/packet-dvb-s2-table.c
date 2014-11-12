/* packet-dvb-s2-table.c
 * Routines for DVB-RCS2 Table dissection
 * Copyright 2015-2020, Thales Alenia Space
 * Copyright 2015-2020, Viveris Technologies <adrien.destugues@opensource.viveris.fr>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include <glib.h>
#include <epan/packet.h>
#include <epan/prefs.h>
#include <epan/etypes.h>
#include <stdio.h>

void proto_register_dvb_s2_table(void);
void proto_reg_handoff_dvb_s2_table(void);


/* preferences */
#define DVB_S2_RCS_TABLE_DECODING      0
#define DVB_S2_RCS2_TABLE_DECODING     1

static gint dvb_s2_rcs_version = 0;

/* Initialize the protocol and registered fields */
static int proto_dvb_s2_table = -1;
static int hf_dvb_s2_table_id = -1;
static int hf_dvb_s2_table_section = -1;
static int hf_dvb_s2_table_private = -1;
static int hf_dvb_s2_table_reserved = -1;
static int hf_dvb_s2_table_msb_len = -1;
static int hf_dvb_s2_table_lsb_len = -1;
static int hf_dvb_s2_table_network_interactive_id = -1;
static int hf_dvb_s2_table_reserved2 = -1;
static int hf_dvb_s2_table_version_number = -1;
static int hf_dvb_s2_table_current_next_indicator = -1;
static int hf_dvb_s2_table_section_number = -1;
static int hf_dvb_s2_table_last_section_number = -1;

static int hf_dvb_s2_table_smt_id = -1;
static int hf_dvb_s2_table_smt_section_syntax_indicator = -1;
static int hf_dvb_s2_table_smt_futur_use = -1;
static int hf_dvb_s2_table_smt_reserved = -1;
static int hf_dvb_s2_table_smt_section_length = -1;
static int hf_dvb_s2_table_smt_esn0 = -1;
static int hf_dvb_s2_table_smt_modcod = -1;

/* SCT */
static int hf_dvb_s2_table_superframe_loop_count = -1;
static int hf_dvb_s2_table_superframe = -1;
static int hf_dvb_s2_table_sf_sequence = -1;
static int hf_dvb_s2_table_sf_id = -1;
static int hf_dvb_s2_table_sf_large_timing_uncertaintly_flag = -1;
static int hf_dvb_s2_table_sf_uplink_polarization = -1;
static int hf_dvb_s2_table_sf_absolute_time = -1;
static int hf_dvb_s2_table_sf_duration = -1;
static int hf_dvb_s2_table_sf_centre_frequency = -1;
static int hf_dvb_s2_table_sf_count = -1;
static int hf_dvb_s2_table_sf_frame_loop_count = -1;
static int hf_dvb_s2_table_sf_frame = -1;
static int hf_dvb_s2_table_sf_frame_type = -1;
static int hf_dvb_s2_table_sf_frame_id = -1;
static int hf_dvb_s2_table_sf_frame_start_time = -1;
static int hf_dvb_s2_table_sf_frame_centre_frequency_offset = -1;
/* TIM */
static int hf_dvb_s2_table_rcst_status = -1;
static int hf_dvb_s2_table_network_status = -1;
static int hf_dvb_s2_table_desc_loop_count = -1;
static int hf_dvb_s2_table_pad_bytes = -1;
/* TBTP2 */
static int hf_dvb_s2_table_group_id = -1;
static int hf_dvb_s2_table_assign_context = -1;
static int hf_dvb_s2_table_superframe_count = -1;
static int hf_dvb_s2_table_assign_format = -1;
static int hf_dvb_s2_table_frame_loop_count = -1;
static int hf_dvb_s2_table_frame_number = -1;
static int hf_dvb_s2_table_frame_assign_offset = -1;
static int hf_dvb_s2_table_frame_assign_loop_count = -1;
static int hf_dvb_s2_table_frame_assignment = -1;
static int hf_dvb_s2_table_frame_assign_id48 = -1;
static int hf_dvb_s2_table_frame_assign_id8 = -1;
static int hf_dvb_s2_table_frame_assign_id16 = -1;
static int hf_dvb_s2_table_frame_assign_id24 = -1;
static int hf_dvb_s2_table_frame_dynamic_tx_type = -1;
/* CMT */
static int hf_dvb_s2_table_entry_loop_count = -1;
static int hf_dvb_s2_table_entry = -1;
static int hf_dvb_s2_table_entry_login_id = -1;
/* TMST2 */
static int hf_dvb_s2_table_common_sytem_margin = -1;
static int hf_dvb_s2_table_tx_mode_count = -1;
static int hf_dvb_s2_table_tx_mode = -1;
static int hf_dvb_s2_table_tx_mode_frame_length = -1;
static int hf_dvb_s2_table_tx_mode_pilot_symbols = -1;
static int hf_dvb_s2_table_tx_mode_modcod = -1;
static int hf_dvb_s2_table_tx_mode_modcod_system_margin = -1;
static int hf_dvb_s2_table_tx_mode_isi = -1;
/* FCT2 */
static int hf_dvb_s2_table_frame_type_loop_count = -1;
static int hf_dvb_s2_table_frame_type_branch = -1;
static int hf_dvb_s2_table_frame_type = -1;
static int hf_dvb_s2_table_frame_type_frame_duration = -1;
static int hf_dvb_s2_table_frame_type_tx_format_class = -1;
static int hf_dvb_s2_table_frame_type_btu_duration = -1;
static int hf_dvb_s2_table_frame_type_btu_carrier_bw = -1;
static int hf_dvb_s2_table_frame_type_btu_symbol_rate = -1;
static int hf_dvb_s2_table_frame_type_time_unit_count = -1;
static int hf_dvb_s2_table_frame_type_grid_repeat_count = -1;
static int hf_dvb_s2_table_frame_type_grid_frequency_offset = -1;
static int hf_dvb_s2_table_frame_type_section_loop_count = -1;
static int hf_dvb_s2_table_frame_type_section = -1;
static int hf_dvb_s2_table_frame_type_section_default_tx_type = -1;
static int hf_dvb_s2_table_frame_type_section_fix_acc_method = -1;
static int hf_dvb_s2_table_frame_type_section_repeat_count= -1;
/* FCT */
static int hf_dvb_s2_table_frame_ID_loop_count = -1;
static int hf_dvb_s2_table_frame_ID_branch = -1;
static int hf_dvb_s2_table_frame_ID = -1;
static int hf_dvb_s2_table_frame_ID_frame_duration = -1;
static int hf_dvb_s2_table_frame_ID_total_timeslot_count = -1;
static int hf_dvb_s2_table_frame_ID_start_timeslot_number = -1;
static int hf_dvb_s2_table_frame_ID_timeslot_loop_count = -1;
static int hf_dvb_s2_table_frame_ID_timeslot = -1;
static int hf_dvb_s2_table_frame_ID_timeslot_timeslot_frequency_offset = -1;
static int hf_dvb_s2_table_frame_ID_timeslot_timeslot_time_offset = -1;
static int hf_dvb_s2_table_frame_ID_timeslot_timeslot_id = -1;
static int hf_dvb_s2_table_frame_ID_timeslot_repeat_count = -1;
/* BCT */
static int hf_dvb_s2_table_tx_type_loop_count = -1;
static int hf_dvb_s2_table_tx_type_branch = -1;
static int hf_dvb_s2_table_tx_type = -1;
static int hf_dvb_s2_table_tx_type_tx_content_type = -1;
static int hf_dvb_s2_table_tx_type_tx_format_class = -1;
static int hf_dvb_s2_table_tx_type_tx_format_data_lenght = -1;
static int hf_dvb_s2_table_tx_type_tx_format_data = -1;
/* BCT Common Tx */
static int hf_dvb_s2_table_tx_type_tx_block_size = -1;
static int hf_dvb_s2_table_tx_type_threshold_es_n0 = -1;
static int hf_dvb_s2_table_tx_type_payload_size = -1;
static int hf_dvb_s2_table_tx_type_modulation_scheme = -1;
static int hf_dvb_s2_table_tx_type_p = -1;
static int hf_dvb_s2_table_tx_type_q0 = -1;
static int hf_dvb_s2_table_tx_type_q1 = -1;
static int hf_dvb_s2_table_tx_type_q2 = -1;
static int hf_dvb_s2_table_tx_type_q3 = -1;
static int hf_dvb_s2_table_tx_type_y_period = -1;
static int hf_dvb_s2_table_tx_type_w_period = -1;
static int hf_dvb_s2_table_tx_type_y_pattern = -1;
static int hf_dvb_s2_table_tx_type_y_pattern_bit = -1;
static int hf_dvb_s2_table_tx_type_w_pattern = -1;
static int hf_dvb_s2_table_tx_type_w_pattern_bit = -1;
static int hf_dvb_s2_table_tx_type_preamble_len = -1;
static int hf_dvb_s2_table_tx_type_postamble_len = -1;
static int hf_dvb_s2_table_tx_type_pilot_period = -1;
static int hf_dvb_s2_table_tx_type_pilot_block_len = -1;
static int hf_dvb_s2_table_tx_type_pilot_sum = -1;
static int hf_dvb_s2_table_tx_type_uw_symbol = -1;
static int hf_dvb_s2_table_tx_type_uw_symbol_unit = -1;
static int hf_dvb_s2_table_tx_type_uw_symbol_qpsk = -1;
static int hf_dvb_s2_table_tx_type_uw_symbol_8psk = -1;
static int hf_dvb_s2_table_tx_type_uw_symbol_16qam = -1;
static int hf_dvb_s2_table_tx_type_waveform_id = -1;
static int hf_dvb_s2_table_tx_type_tx_start_offset = -1;
/* BCT LM Tx */
static int hf_dvb_s2_table_tx_type_tx_start_offset_1 = -1;
static int hf_dvb_s2_table_tx_type_tx_start_offset_2 = -1;
/* BCT CPM Tx */
static int hf_dvb_s2_table_tx_type_modulation_mh = -1;
static int hf_dvb_s2_table_tx_type_modulation_ph = -1;
static int hf_dvb_s2_table_tx_type_modulation_type = -1;
static int hf_dvb_s2_table_tx_type_alpha_rc = -1;
static int hf_dvb_s2_table_tx_type_code_rate = -1;
static int hf_dvb_s2_table_tx_type_constraint_length_k = -1;
static int hf_dvb_s2_table_tx_type_uw_length = -1;
static int hf_dvb_s2_table_tx_type_nbr_uw_segments = -1;
static int hf_dvb_s2_table_tx_type_uw_segment = -1;
static int hf_dvb_s2_table_tx_type_uw_segment_start = -1;
static int hf_dvb_s2_table_tx_type_uw_segment_length = -1;
static int hf_dvb_s2_table_tx_type_param_interleaver = -1;
static int hf_dvb_s2_table_tx_type_n = -1;
static int hf_dvb_s2_table_tx_type_s = -1;
static int hf_dvb_s2_table_tx_type_p_interleaver = -1;
static int hf_dvb_s2_table_tx_type_n1_12 = -1;
static int hf_dvb_s2_table_tx_type_k1_12 = -1;
static int hf_dvb_s2_table_tx_type_K2_12 = -1;
static int hf_dvb_s2_table_tx_type_K3_12 = -1;
static int hf_dvb_s2_table_tx_type_pi_i = -1;
/* SPT */
static int hf_dvb_s2_table_satellite_loop_count = -1;
static int hf_dvb_s2_table_satellite = -1;
static int hf_dvb_s2_table_satellite_id = -1;
static int hf_dvb_s2_table_satellite_x_coordinate = -1;
static int hf_dvb_s2_table_satellite_y_coordinate = -1;
static int hf_dvb_s2_table_satellite_z_coordinate = -1;
/* NIT - RMT */
static int hf_dvb_s2_table_network_descriptors_length = -1;
static int hf_dvb_s2_table_multiplex_streams_spec_length = -1;
static int hf_dvb_s2_table_multiplex = -1;
static int hf_dvb_s2_table_multiplex_forward_multiplex = -1;
static int hf_dvb_s2_table_multiplex_reward_multiplex = -1;
static int hf_dvb_s2_table_multiplex_original_network_id = -1;
static int hf_dvb_s2_table_multiplex_transport_descriptors_length = -1;
/* MMT2 */
static int hf_dvb_s2_table_svn_number = -1;
static int hf_dvb_s2_table_svn_prefix_size = -1;
static int hf_dvb_s2_table_pt_count = -1;
static int hf_dvb_s2_table_protocol = -1;
static int hf_dvb_s2_table_pt_protocol_type = -1;
static int hf_dvb_s2_table_pt_address_size = -1;
static int hf_dvb_s2_table_pt_mapping_sections = -1;
static int hf_dvb_s2_table_pt_mapping_section = -1;
static int hf_dvb_s2_table_pt_ms_inclusion_start = -1;
static int hf_dvb_s2_table_pt_ms_inclusion_end = -1;
static int hf_dvb_s2_table_pt_ms_exclusions = -1;
static int hf_dvb_s2_table_pt_ms_exclusion = -1;
static int hf_dvb_s2_table_pt_ms_exclusion_start = -1;
static int hf_dvb_s2_table_pt_ms_exclusion_end = -1;
static int hf_dvb_s2_table_pt_ms_mac24_base = -1;
static int hf_dvb_s2_table_pt_ms_mcast_prefix_size = -1;

/* descriptors */
static int hf_dvb_s2_table_descriptor = -1;
static int hf_dvb_s2_table_desc_tag = -1;
static int hf_dvb_s2_table_desc_length = -1;
/* Control assign descriptor */
static int hf_dvb_s2_table_desc_sync_achieved_time_threshold = -1;
static int hf_dvb_s2_table_desc_max_sync_tries = -1;
static int hf_dvb_s2_table_desc_sync_achieved_freq_threshold = -1;
static int hf_dvb_s2_table_desc_ctrl_start_superframe_count = -1;
static int hf_dvb_s2_table_desc_ctrl_frame_nbr = -1;
static int hf_dvb_s2_table_desc_ctrl_repeat_period = -1;
static int hf_dvb_s2_table_desc_ctrl_timeslot_nbr = -1;
static int hf_dvb_s2_table_desc_sync_start_superframe = -1;
static int hf_dvb_s2_table_desc_sync_frame_nbr = -1;
static int hf_dvb_s2_table_desc_sync_repeat_period = -1;
static int hf_dvb_s2_table_desc_sync_slot_nbr = -1;
/* Correction message descriptor */
static int hf_dvb_s2_table_desc_time_correct_flag = -1;
static int hf_dvb_s2_table_desc_power_correct_flag = -1;
static int hf_dvb_s2_table_desc_freq_correct_flag = -1;
static int hf_dvb_s2_table_desc_slot_type = -1;
static int hf_dvb_s2_table_desc_burst_time_scaling = -1;
static int hf_dvb_s2_table_desc_burst_time_correct = -1;
static int hf_dvb_s2_table_desc_power_ctrl_flag = -1;
static int hf_dvb_s2_table_desc_power_correction = -1;
static int hf_dvb_s2_table_desc_power_esn0 = -1;
static int hf_dvb_s2_table_desc_freq_correction = -1;
/* Correction message extension descriptor */
static int hf_dvb_s2_table_desc_slot_nbr = -1;
static int hf_dvb_s2_table_desc_sf_sequence = -1;
static int hf_dvb_s2_table_desc_frame_number = -1;
/* Logon response descriptor */
static int hf_dvb_s2_table_desc_keep_id_after_logoff = -1;
static int hf_dvb_s2_table_desc_power_ctrl_mode = -1;
static int hf_dvb_s2_table_desc_rcst_access_status = -1;
static int hf_dvb_s2_table_desc_logon_id = -1;
static int hf_dvb_s2_table_desc_lowest_assign_id = -1;
static int hf_dvb_s2_table_desc_assign_id_count = -1;
static int hf_dvb_s2_table_desc_unicast_mac24_count = -1;

static int hf_dvb_s2_table_mac24 = -1;
static int hf_dvb_s2_table_mac24_prefix_size = -1;
static int hf_dvb_s2_table_mac24_unicast = -1;
static int hf_dvb_s2_table_mac24_mcast_mapping_method = -1;
static int hf_dvb_s2_table_mac24_mcast_ip_version_ind_pres = -1;
static int hf_dvb_s2_table_mac24_mcast_synthesis_field_size = -1;
static int hf_dvb_s2_table_desc_default_svn_number = -1;
static int hf_dvb_s2_table_desc_reserved = -1;
/* Lower layer service descriptor */
static int hf_dvb_s2_table_desc_default_ctrl_random_interval = -1;
static int hf_dvb_s2_table_desc_dynamic_rate_persistence = -1;
static int hf_dvb_s2_table_desc_volume_backlog_persistence = -1;
static int hf_dvb_s2_table_desc_lls_count = -1;
static int hf_dvb_s2_table_desc_rc_count = -1;
static int hf_dvb_s2_table_desc_ra_ac_count = -1;

static int hf_dvb_s2_table_lls = -1;
static int hf_dvb_s2_table_lls_index = -1;
static int hf_dvb_s2_table_lls_random_access = -1;
static int hf_dvb_s2_table_lls_dedicated_access = -1;
static int hf_dvb_s2_table_lls_nominal_rc_index = -1;
static int hf_dvb_s2_table_lls_nominal_da_ac_index = -1;
static int hf_dvb_s2_table_lls_conditional_demand_rc_map = -1;
static int hf_dvb_s2_table_lls_conditional_scheduler_da_ac_map = -1;
static int hf_dvb_s2_table_lls_nominal_ra_ac_index = -1;
static int hf_dvb_s2_table_lls_conditional_scheduler_ra_ac_map = -1;

static int hf_dvb_s2_table_rc = -1;
static int hf_dvb_s2_table_rc_index = -1;
static int hf_dvb_s2_table_rc_constant_assignment_provided = -1;
static int hf_dvb_s2_table_rc_volume_allowed = -1;
static int hf_dvb_s2_table_rc_rbdc_allowed = -1;
static int hf_dvb_s2_table_rc_maximum_service_rate = -1;
static int hf_dvb_s2_table_rc_minimum_service_rate = -1;
static int hf_dvb_s2_table_rc_constant_service_rate = -1;
static int hf_dvb_s2_table_rc_maximum_backlog = -1;

static int hf_dvb_s2_table_ra_ac = -1;
static int hf_dvb_s2_table_ra_ac_index = -1;
static int hf_dvb_s2_table_ra_ac_max_unique_payload_per_block = -1;
static int hf_dvb_s2_table_ra_ac_max_consecutive_block_accessed = -1;
static int hf_dvb_s2_table_ra_ac_min_idle_block = -1;
static int hf_dvb_s2_table_ra_ac_defaults_field_size = -1;
static int hf_dvb_s2_table_ra_ac_defaults_for_ra_load_control = -1;

static int hf_dvb_s2_table_crc32 = -1;

/* Initialize the subtree pointers */
static gint ett_dvb_s2_hdr_table = -1;
static gint ett_dvb_s2_hdr_table_sf = -1;
static gint ett_dvb_s2_hdr_table_sf_frame = -1;
static gint ett_dvb_s2_hdr_table_desc = -1;
static gint ett_dvb_s2_hdr_table_frame = -1;
static gint ett_dvb_s2_hdr_table_frame_assign = -1;
static gint ett_dvb_s2_hdr_table_entry = -1;
static gint ett_dvb_s2_hdr_table_mac24 = -1;
static gint ett_dvb_s2_hdr_table_frametype = -1;
static gint ett_dvb_s2_hdr_table_frame_ID = -1;
static gint ett_dvb_s2_hdr_table_frame_ID_timeslot = -1;
static gint ett_dvb_s2_hdr_table_frametype_section = -1;
static gint ett_dvb_s2_hdr_table_lls = -1;
static gint ett_dvb_s2_hdr_table_rc = -1;
static gint ett_dvb_s2_hdr_table_raac = -1;
static gint ett_dvb_s2_hdr_table_txmode = -1;
static gint ett_dvb_s2_hdr_table_txtype = -1;
static gint ett_dvb_s2_hdr_table_txtype_ypattern = -1;
static gint ett_dvb_s2_hdr_table_txtype_wpattern = -1;
static gint ett_dvb_s2_hdr_table_txtype_uwsymbol = -1;
static gint ett_dvb_s2_hdr_table_txtype_uwsegment = -1;
static gint ett_dvb_s2_hdr_table_satellite = -1;
static gint ett_dvb_s2_hdr_table_multiplex = -1;
static gint ett_dvb_s2_hdr_table_pt = -1;
static gint ett_dvb_s2_hdr_table_pt_ms = -1;
static gint ett_dvb_s2_hdr_table_pt_ms_exclusion = -1;

static const value_string table_modcods[] = {
    { 0, "DUMMY PLFRAME"},
    { 1, "QPSK 1/4"},
    { 2, "QPSK 1/3"},
    { 3, "QPSK 2/5"},
    { 4, "QPSK 1/2"},
    { 5, "QPSK 3/5"},
    { 6, "QPSK 2/3"},
    { 7, "QPSK 3/4"},
    { 8, "QPSK 4/5"},
    { 9, "QPSK 5/6"},
    {10, "QPSK 8/9"},
    {11, "QPSK 9/10"},
    {12, "8PSK 3/5"},
    {13, "8PSK 2/3"},
    {14, "8PSK 3/4"},
    {15, "8PSK 5/6"},
    {16, "8PSK 8/9"},
    {17, "8PSK 9/10"},
    {18, "16APSK 2/3"},
    {19, "16APSK 3/4"},
    {20, "16APSK 4/5"},
    {21, "16APSK 5/6"},
    {22, "16APSK 8/9"},
    {23, "16APSK 9/10"},
    {24, "32APSK 3/4"},
    {25, "32APSK 4/5"},
    {26, "32APSK 5/6"},
    {27, "32APSK 8/9"},
    {28, "32APSK 9/10"},
    {29, "reserved"},
    {30, "reserved"},
    {31, "reserved"},
    { 0, NULL}
};

#define DVB_S2_TABLE_SECTION_MASK 0x80
#define DVB_S2_TABLE_PRIVATE_MASK 0x40
#define DVB_S2_TABLE_RESERVED_MASK 0x30
#define DVB_S2_TABLE_MSB_LEN_MASK 0x0F
#define DVB_S2_TABLE_RESERVED2_MASK 0xC0
#define DVB_S2_TABLE_VERSION_NUMBER_MASK 0x3E
#define DVB_S2_TABLE_CURRENT_NEXT_INDICATOR_MASK 0x01

#define DVB_S2_TABLE_SMT_SECTION_INDICATOR_MASK 0x8000
#define DVB_S2_TABLE_SMT_FUTUR_USE_MASK 0x4000
#define DVB_S2_TABLE_SMT_RESERVED_MASK 0x3000
#define DVB_S2_TABLE_SMT_SECTION_LENGTH_MASK 0x0FFF

#define DVB_S2_TABLE_SCT_LARGE_TIMING_FLAG_MASK 0x80
#define DVB_S2_TABLE_SCT_UPLINK_POLARIZATION_MASK 0x03
#define DVB_S2_TABLE_SCT_START_TIME_BASE_MASK 0x8000
#define DVB_S2_TABLE_SCT_START_TIME_EXT_MASK 0x01FF
#define DVB_S2_TABLE_SCT_FRAME_LOOP_COUNT_MASK 0x1F

#define DVB_S2_TABLE_FRAME_TYPE_SECTION_FAM_MASK 0xC
#define DVB_S2_TABLE_FRAME_ID_TOT_TIME_COUNT_MASK 0x07FF
#define DVB_S2_TABLE_NETWORK_DESC_LENGTH_MASK 0x0FFF
#define DVB_S2_TABLE_MULTIPLEX_STREAMS_LENGTH_MASK 0x0FFF
#define DVB_S2_TABLE_MULTIPLEX_TRANSPORT_DESC_LENGTH_MASK 0x0FFF
#define DVB_S2_TABLE_SVN_PREFIX_SIZE_MASK 0x1F
#define DVB_S2_TABLE_PT_MS_MCAST_PREFIX_SIZE_MASK 0x1F
#define DVB_S2_TABLE_TX_TYPE_TX_START_OFFSET_MASK 0x000FFFFF
#define DVB_S2_TABLE_TX_TYPE_QX_MASK 0x0F
#define DVB_S2_TABLE_TX_TYPE_W_Y_PERIOD_MASK 0x1F
#define DVB_S2_TABLE_TX_TYPE_PILOT_PERIOD_MASK 0x0FFF
#define DVB_S2_TABLE_TX_TYPE_W_Y_PATTERN_BIT_MASK 0x80
#define DVB_S2_TABLE_TX_TYPE_MODULATION_MH_MASK 0xE0
#define DVB_S2_TABLE_TX_TYPE_MODULATION_PH_MASK 0x1C
#define DVB_S2_TABLE_TX_TYPE_MODULATION_TYPE_MASK 0x01
#define DVB_S2_TABLE_TX_TYPE_CODE_RATE_MASK 0x1C
#define DVB_S2_TABLE_TX_TYPE_CONSTRAINT_LENGTH_K_MASK 0x01
#define DVB_S2_TABLE_TX_TYPE_PARAM_INTERLEAVER_MASK 0x01
#define DVB_S2_TABLE_TX_TYPE_N_MASK 0xFFF0
#define DVB_S2_TABLE_TX_TYPE_S_MASK 0x0FC0
#define DVB_S2_TABLE_TX_TYPE_P_MASK 0x3FF0
#define DVB_S2_TABLE_TX_TYPE_N1_12_MASK 0x0FF8
#define DVB_S2_TABLE_TX_TYPE_K1_12_MASK 0x07FC
#define DVB_S2_TABLE_TX_TYPE_K2_12_MASK 0x03FE
#define DVB_S2_TABLE_TX_TYPE_K3_12_MASK 0x01FF
#define DVB_S2_TABLE_TX_TYPE_UW_SYMBOL_QPSK_MASK 0xC0
#define DVB_S2_TABLE_TX_TYPE_UW_SYMBOL_8PSK_MASK 0xE0
#define DVB_S2_TABLE_TX_TYPE_UW_SYMBOL_16QAM_MASK 0xF0

#define DVB_S2_TABLE_DESC_TIME_CORRECT_FLAG_MASK 0x80
#define DVB_S2_TABLE_DESC_POWER_CORRECT_FLAG_MASK 0x40
#define DVB_S2_TABLE_DESC_FREQ_CORRECT_FLAG_MASK 0x20
#define DVB_S2_TABLE_DESC_SLOT_TYPE_MASK 0x18
#define DVB_S2_TABLE_DESC_BURST_TIME_SCALING_MASK 0x7
#define DVB_S2_TABLE_DESC_POWER_CTRL_FLAG_MASK 0x80
#define DVB_S2_TABLE_DESC_POWER_CORRECTION_MASK 0x7F
#define DVB_S2_TABLE_DESC_KEEP_ID_AFTER_LOGOFF_MASK 0x40
#define DVB_S2_TABLE_DESC_POWER_CTRLMODE_MASK 0x30
#define DVB_S2_TABLE_DESC_RCST_ACCESS_STATUS_MASK 0x0F
#define DVB_S2_TABLE_DESC_ASSIGN_ID_COUNT_MASK 0xF0
#define DVB_S2_TABLE_DESC_UNICAST_MAC24_COUNT_MASK 0x0F
#define DVB_S2_TABLE_DESC_LLS_COUNT_MASK 0x0F
#define DVB_S2_TABLE_DESC_RC_COUNT_MASK 0x0F
#define DVB_S2_TABLE_DESC_RA_AC_COUNT_MASK 0x0F
#define DVB_S2_TABLE_DESC_CTRL_TIMESLOT_NBR_MASK 0x07FF
#define DVB_S2_TABLE_DESC_SYNC_FRAME_NBR_MASK 0x07FF

#define DVB_S2_TABLE_RA_AC_INDEX_MASK 0x0F
#define DVB_S2_TABLE_RC_RBDC_ALLOWED_MASK 0x01
#define DVB_S2_TABLE_RC_VOLUME_ALLOWED_MASK 0x02
#define DVB_S2_TABLE_RC_CONSTANT_ASSIGN_MASK 0x04
#define DVB_S2_TABLE_RC_INDEX_MASK 0xF0
#define DVB_S2_TABLE_LLS_NOMINAL_RA_AC_INDEX_MASK 0x0F
#define DVB_S2_TABLE_LLS_NOMINAL_DA_AC_INDEX_MASK 0x0F
#define DVB_S2_TABLE_LLS_NOMINAL_RC_INDEX_MASK 0xF0
#define DVB_S2_TABLE_LLS_DEDICATED_ACCESS_MASK 0x01
#define DVB_S2_TABLE_LLS_RANDOM_ACCESS_MASK 0x02
#define DVB_S2_TABLE_LLS_INDEX_MASK 0x3C

#define DVB_S2_TABLE_MAC24_PREFIX_SIZE_MASK 0x1F
#define DVB_S2_TABLE_MAC24_MAPPING_METHOD_MASK 0x40
#define DVB_S2_TABLE_MAC24_MCAST_IP_VERSION_IND_PRES_MASK 0x20
#define DVB_S2_TABLE_MAC24_MCAST_SYNTHESIS_FIELD_SIZE_MASK 0x1F

#define DVB_S2_TABLE_TX_MODE_FRAME_LENGTH_MASK 0xC0
#define DVB_S2_TABLE_TX_MODE_PILOT_SYMBOLS_MASK 0x20
#define DVB_S2_TABLE_TX_MODE_MODCOD_MASK 0x1F

#define DVB_S2_TABLE_ENTRY_SIZE 8
#define DVB_S2_TABLE_TX_MODE_SIZE 3
#define DVB_S2_TABLE_SAT_SIZE 13
#define DVB_S2_TABLE_TX_TYPE_UW_SEGMENT_SIZE 3

#define DVB_S2_TABLE_PAT 0x00
#define DVB_S2_TABLE_CAT 0x01
#define DVB_S2_TABLE_PMT 0x02
#define DVB_S2_TABLE_NIT 0x40
#define DVB_S2_TABLE_RMT 0x41
#define DVB_S2_TABLE_SDT 0x42
#define DVB_S2_TABLE_DTD 0x70
#define DVB_S2_TABLE_SCT 0xA0
#define DVB_S2_TABLE_FCT 0xA1
#define DVB_S2_TABLE_TCT 0xA2
#define DVB_S2_TABLE_SPT 0xA3
#define DVB_S2_TABLE_CMT 0xA4
#define DVB_S2_TABLE_TBTP 0xA5
#define DVB_S2_TABLE_PCR 0xA6
#define DVB_S2_TABLE_TMST 0xAA
#define DVB_S2_TABLE_TCTE 0xAB
#define DVB_S2_TABLE_FCT2 0xAB
#define DVB_S2_TABLE_BCT 0xAC
#define DVB_S2_TABLE_TBTP2 0xAD
#define DVB_S2_TABLE_TMST2 0xAE
#define DVB_S2_TABLE_FAT 0xAF
#define DVB_S2_TABLE_TIM 0xB0
#define DVB_S2_TABLE_MMT2 0xB2
#define DVB_S2_TABLE_MMT 0xC0
#define DVB_S2_TABLE_SMT 0x80
#define DVB_S2_TABLE_TIMB 0xFE
static const value_string tabletype[] = {
    {DVB_S2_TABLE_PAT, "PAT"}, /**< Program Association Table */
    {DVB_S2_TABLE_CAT, "CAT"}, /**< Conditional Access Table */
    {DVB_S2_TABLE_PMT, "PMT"}, /**< Program Map Table */
    {DVB_S2_TABLE_NIT, "NIT"}, /**< Network Information Table */
    {DVB_S2_TABLE_RMT, "RMT"}, /**< RCS Mapping Table */
    {DVB_S2_TABLE_SDT, "SDT"}, /**< Service Description Table */
    {DVB_S2_TABLE_DTD, "DTD"}, /**< Time and Date Table */
    {DVB_S2_TABLE_SCT, "SCT"}, /**< Superframe Composition Table */
    {DVB_S2_TABLE_FCT, "FCT"}, /**< Frame Composition Table */
    {DVB_S2_TABLE_TCT, "TCT"}, /**< Timeslot Composition Table*/
    {DVB_S2_TABLE_SPT, "SPT"}, /**< Satellite Position Table */
    {DVB_S2_TABLE_CMT, "CMT"}, /**< Correction Message Table */
    {DVB_S2_TABLE_TBTP, "TBTP"}, /**< Terminal Burst Time Plan Table */
    {DVB_S2_TABLE_PCR, "PCR"}, /**< PCR Packet Payload */
    {DVB_S2_TABLE_TMST, "TMST"}, /**< Transmission Mode Support Table */
    {DVB_S2_TABLE_TCTE, "TCTE(RCS)-FCT2(RCS2)"}, /**< Timeslot Composition Table extended */
//    {DVB_S2_TABLE_FCT2, "FCT2"}, /**< Frame Composition Table RCS2 */
    {DVB_S2_TABLE_BCT, "BCT"}, /**< Broadcast Configuration Table RCS2 */
    {DVB_S2_TABLE_TBTP2, "TBTP2"}, /**< Terminal Burst Time Plan Table RCS2 */
    {DVB_S2_TABLE_TMST2, "TMST2"}, /**< Transmission Mode Support Table RCS2 */
    {DVB_S2_TABLE_FAT, "FAT"}, /**< Fast Access Table RCS2 */
    {DVB_S2_TABLE_TIM, "TIM"}, /**< Terminal Information Message Table */
    {DVB_S2_TABLE_MMT, "MMT"}, /**< Multicast Mapping Table */
    {DVB_S2_TABLE_MMT2, "MMT2"}, /**< Multicast Mapping Table RCS2*/
    {DVB_S2_TABLE_SMT, "SMT"}, /**< Signal Measurement Table */
    {DVB_S2_TABLE_TIMB, "TIMB"}, /**< Broadcast TIM, for internal use only */
    {0, NULL}
};

#define DVB_S2_TABLE_DESC_CTRL_ASSIGN 0xA4
#define DVB_S2_TABLE_DESC_CORRECTION_MSG 0xA1
#define DVB_S2_TABLE_DESC_CORRECTION_MSG_EXT 0xB1
#define DVB_S2_TABLE_DESC_LOGON_RESPONSE 0xB9
#define DVB_S2_TABLE_DESC_LOWER_LAYER_SERVICE 0xBB

static const value_string table_desc_type[] = {
    {0x4A, "Linkage_descriptor"},
    {0xA0, "Network_layer_info_descriptor"},
    {DVB_S2_TABLE_DESC_CORRECTION_MSG, "Correction_message_descriptor"},
    {DVB_S2_TABLE_DESC_CTRL_ASSIGN, "SYNC_assign_descriptor (RCS) - control_assign_descriptor (RCS2)"},
    {0xA6, "Echo_value_descriptor"},
    {0xA7, "RCS_content_descriptor"},
    {0xA8, "Satellite_forward_link_descriptor"},
    {0xA9, "Satellite_return_link_descriptor"},
    {0xAB, "Logon_Contention_descriptor"},
    {0xAC, "Correction_control_descriptor"},
    {0xB0, "Mobility_control_descriptor"},
    {DVB_S2_TABLE_DESC_CORRECTION_MSG_EXT, "Correction_message_extension_descriptor"},
    {0xB2, "Return_Transmission_Modes_descriptor"},
    {0xB5, "Implementation_type_descriptor"},
    {0xB6, "LL_FEC_identifier_descriptor"},
    {0xB7, "Frame_payload_format_descriptor"},
    {0xB8, "Pointing_alignment_support_descriptor"},
    {DVB_S2_TABLE_DESC_LOGON_RESPONSE, "Logon_response_descriptor"},
    {0xBA, "DHCP_option_descriptor"},
    {DVB_S2_TABLE_DESC_LOWER_LAYER_SERVICE, "lower_layer_service_descriptor"},
    {0xBC, "TRANSEC_message_descriptor"},
    {0xBD, "Forward_link_streams_descriptor"},
    {0xBE, "Logon_Security_descriptor"},
    {0xBF, "Transmission_offset_descriptor"},
    {0xC0, "Random_assess_load_control_descriptor"},
    {0xC1, "CLI_instruction_descriptor"},
    {0xC2, "random_access_traffic_method_descriptor"},
    {0xC4, "higher_layers_initialize_descriptor"},
    {0xC5, "lowest_sw_version_descriptor"},
    {0xC6, "Mesh_system_descriptor"},
    {0xC7, "Extension_protocol_descriptor"},
    {0xC8, "Continuous_carrier_control_descriptor"},
    {0, NULL}
};

static const value_string table_uplinkPolarization[] = {
    {0, "linear - horizontal"},
    {1, "linear - vertical"},
    {2, "circular - left"},
    {3, "circular - right"},
    {0, NULL}
};

#define DVB_S2_TABLE_TXFORMAT_LMBT 0x1
#define DVB_S2_TABLE_TXFORMAT_CPMBT 0x2
#define DVB_S2_TABLE_TXFORMAT_CT 0x3
#define DVB_S2_TABLE_TXFORMAT_SSLMBT 0x4
static const value_string table_frameType_txFormatClass[] = {
    {0, "Reserved"},
    {DVB_S2_TABLE_TXFORMAT_LMBT, "Linear Modulation Burst Transmission"},
    {DVB_S2_TABLE_TXFORMAT_CPMBT, "Continous Phase Modulation Burst Transmission"},
    {DVB_S2_TABLE_TXFORMAT_CT, "Continous Transmission"},
    {DVB_S2_TABLE_TXFORMAT_SSLMBT, "Spread-Spectrum Linear Modulation Burst Transmission"},
    {0, NULL}
};

static const value_string table_assignContext[] = {
    {0, "All traffic context"},
    {1, "Transparent star traffic"},
    {2, "Logon"},
    {3, "Transparent mesh traffic"},
    {4, "Continous carrier"},
    {0, NULL}
};


static const value_string table_timeslotContent[] = {
    {0, "Traffic"},
    {1, "Logon"},
    {2, "Reserved"},
    {3, "Control"},
    {0, NULL}
};

static const value_string table_txType_contentType[] = {
    {0, "Reserved"},
    {1, "Logon payload"},
    {2, "Control payload"},
    {3, "Traffic and control payload"},
    {4, "Traffic payload"},
    {0, NULL}
};

#define DVB_S2_TABLE_MODULATION_SCHEME_BPSK 0x0
#define DVB_S2_TABLE_MODULATION_SCHEME_QPSK 0x1
#define DVB_S2_TABLE_MODULATION_SCHEME_8PSK 0x2
#define DVB_S2_TABLE_MODULATION_SCHEME_16QAM 0x3
#define DVB_S2_TABLE_MODULATION_SCHEME_PI2BPSK 0x5
static const value_string table_txType_modulationScheme[] = {
    {DVB_S2_TABLE_MODULATION_SCHEME_BPSK, "Reserved (BPSK)"},
    {DVB_S2_TABLE_MODULATION_SCHEME_QPSK, "QPSK"},
    {DVB_S2_TABLE_MODULATION_SCHEME_8PSK, "8PSK"},
    {DVB_S2_TABLE_MODULATION_SCHEME_16QAM, "16QAM"},
    {4, "Reserved"},
    {DVB_S2_TABLE_MODULATION_SCHEME_PI2BPSK, "pi/2-BPSK"},
    {0, NULL}
};

static const value_string table_txType_modulationType[] = {
    {0, "Quaternary - Linear mapping"},
    {1, "Quaternary - Gray mapping"},
    {0, NULL}
};

static const value_string table_txType_codeRate[] = {
    {0, "1/2"},
    {1, "2/3"},
    {2, "4/5"},
    {3, "6/7"},
    {0, NULL}
};

static const value_string table_txType_constraintLengthK[] = {
    {0, "3"},
    {1, "4"},
    {0, NULL}
};

#define DVB_S2_TABLE_HEADER_LEN     8
#define DVB_S2_TABLE_HEADER_RCS2_LEN 4
#define DVB_S2_TABLE_CRC32_LEN 4
#define DVB_S2_TABLE_SCT_FRAME_LEN  8

/* *** Code to actually dissect the packets *** */
static int dissect_dvb_s2_table_correct_msg(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int new_off = 0;
    guint8 time_correction_flag, frequency_correction_flag, power_correction_flag, power_control_flag;

    time_correction_flag =  tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_DESC_TIME_CORRECT_FLAG_MASK;
    frequency_correction_flag =  tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_DESC_FREQ_CORRECT_FLAG_MASK;
    power_correction_flag =  tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_DESC_POWER_CORRECT_FLAG_MASK;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_time_correct_flag, tvb, cur_off + new_off, 1, ENC_NA);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_power_correct_flag, tvb, cur_off + new_off, 1, ENC_NA);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_freq_correct_flag, tvb, cur_off + new_off, 1, ENC_NA);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_slot_type, tvb, cur_off + new_off, 1, ENC_NA);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_burst_time_scaling, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    if(time_correction_flag)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_burst_time_correct, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
    }
    if(power_correction_flag)
    {
        power_control_flag = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_DESC_POWER_CTRL_FLAG_MASK;
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_power_ctrl_flag, tvb, cur_off + new_off, 1, ENC_NA);
        if(power_control_flag)
            proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_power_correction, tvb, cur_off + new_off, 1, ENC_NA);
        else
            proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_power_esn0, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
    }
    if(frequency_correction_flag)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_freq_correction, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
    }

    return new_off;
}


static int dissect_dvb_s2_table_desc(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree, int desc_loop_count)
{
    int cur_desc, lls_size, rc_size, raac_size, new_off = 0;
    guint8 desc_tag, desc_length;
    proto_tree *dvb_s2_hdr_table_desc_tree, *dvb_s2_hdr_table_mac24_tree, *dvb_s2_hdr_table_lls_tree,
               *dvb_s2_hdr_table_rc_tree, *dvb_s2_hdr_table_raac_tree;
    proto_item *ti;

    guint8 mac24_count, cur_mac24, lls_count, cur_lls, rc_count, cur_rc, raac_count, cur_raac;
    guint8 dedicated_access, random_access, constant_assign_provided, volume_allowed, defaults_field_size;

    for(cur_desc=0 ; cur_desc<=desc_loop_count ; cur_desc++)
    {
        desc_length = tvb_get_guint8(tvb, cur_off + new_off + 1);
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_descriptor, tvb, cur_off + new_off, desc_length + 2, ENC_NA);
        dvb_s2_hdr_table_desc_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_desc);
        desc_tag = tvb_get_guint8(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_tag, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_length, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        switch(desc_tag){
            case(DVB_S2_TABLE_DESC_CTRL_ASSIGN):
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_sync_achieved_time_threshold, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_max_sync_tries, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_sync_achieved_freq_threshold, tvb, cur_off + new_off, 2, ENC_NA);
                new_off += 2;
                if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
                {
                    proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_sync_start_superframe, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_sync_frame_nbr, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_sync_repeat_period, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_sync_slot_nbr, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                }
                if (dvb_s2_rcs_version == DVB_S2_RCS2_TABLE_DECODING)
                {
                    proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_ctrl_start_superframe_count, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_ctrl_frame_nbr, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_ctrl_repeat_period, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_ctrl_timeslot_nbr, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                }
                break;
            case(DVB_S2_TABLE_DESC_CORRECTION_MSG):
                new_off += dissect_dvb_s2_table_correct_msg(tvb, cur_off + new_off, dvb_s2_hdr_table_desc_tree);
                break;
            case(DVB_S2_TABLE_DESC_CORRECTION_MSG_EXT):
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_sf_sequence, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_sf_count, tvb, cur_off + new_off, 2, ENC_NA);
                new_off += 2;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_frame_number, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_slot_nbr, tvb, cur_off + new_off, 2, ENC_NA);
                new_off += 2;
                break;
            case(DVB_S2_TABLE_DESC_LOGON_RESPONSE):
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_keep_id_after_logoff, tvb, cur_off + new_off, 1, ENC_NA);
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_power_ctrl_mode, tvb, cur_off + new_off, 1, ENC_NA);
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_rcst_access_status, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_group_id, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_logon_id, tvb, cur_off + new_off, 2, ENC_NA);
                new_off += 2;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_lowest_assign_id, tvb, cur_off + new_off, 3, ENC_NA);
                new_off += 3;
                mac24_count = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_DESC_UNICAST_MAC24_COUNT_MASK;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_assign_id_count, tvb, cur_off + new_off, 1, ENC_NA);
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_unicast_mac24_count, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                for(cur_mac24=0 ; cur_mac24<mac24_count ; cur_mac24++)
                {
                    ti = proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_mac24, tvb, cur_off + new_off, 5, ENC_NA);
                    dvb_s2_hdr_table_mac24_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_desc);
                    proto_tree_add_item(dvb_s2_hdr_table_mac24_tree, hf_dvb_s2_table_mac24_prefix_size, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_mac24_tree, hf_dvb_s2_table_mac24_unicast, tvb, cur_off + new_off, 3, ENC_NA);
                    new_off += 3;
                    proto_tree_add_item(dvb_s2_hdr_table_mac24_tree, hf_dvb_s2_table_mac24_mcast_mapping_method, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_mac24_tree, hf_dvb_s2_table_mac24_mcast_ip_version_ind_pres, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_mac24_tree, hf_dvb_s2_table_mac24_mcast_synthesis_field_size, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                }
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_default_svn_number, tvb, cur_off + new_off, 2, ENC_NA);
                new_off += 2;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_reserved, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                break;
            case(DVB_S2_TABLE_DESC_LOWER_LAYER_SERVICE):
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_default_ctrl_random_interval, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_dynamic_rate_persistence, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_volume_backlog_persistence, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                lls_count = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_DESC_LLS_COUNT_MASK;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_lls_count, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                for(cur_lls=0 ; cur_lls<lls_count ; cur_lls++)
                {
                    dedicated_access = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_LLS_DEDICATED_ACCESS_MASK;
                    random_access = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_LLS_RANDOM_ACCESS_MASK;
                    lls_size=1;
                    if(dedicated_access) lls_size += 5;
                    if(random_access) lls_size += 2;
                    ti = proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_lls, tvb, cur_off + new_off, lls_size, ENC_NA);
                    dvb_s2_hdr_table_lls_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_desc);
                    proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_index, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_random_access, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_dedicated_access, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    if(dedicated_access)
                    {
                        proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_nominal_rc_index, tvb, cur_off + new_off, 1, ENC_NA);
                        proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_nominal_da_ac_index, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                        proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_conditional_demand_rc_map, tvb, cur_off + new_off, 2, ENC_NA);
                        new_off += 2;
                        proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_conditional_scheduler_da_ac_map, tvb, cur_off + new_off, 2, ENC_NA);
                        new_off += 2;
                    }
                    if(random_access)
                    {
                        proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_nominal_ra_ac_index, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                        proto_tree_add_item(dvb_s2_hdr_table_lls_tree, hf_dvb_s2_table_lls_conditional_scheduler_ra_ac_map, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                    }
                }
                rc_count = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_DESC_RC_COUNT_MASK;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_rc_count, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                for(cur_rc=0 ; cur_rc<rc_count ; cur_rc++)
                {
                    constant_assign_provided = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_RC_CONSTANT_ASSIGN_MASK;
                    volume_allowed = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_RC_VOLUME_ALLOWED_MASK;
                    rc_size=5;
                    if(constant_assign_provided) rc_size += 2;
                    if(volume_allowed) rc_size += 1;
                    ti = proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_rc, tvb, cur_off + new_off, rc_size, ENC_NA);
                    dvb_s2_hdr_table_rc_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_desc);
                    proto_tree_add_item(dvb_s2_hdr_table_rc_tree, hf_dvb_s2_table_rc_index, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_rc_tree, hf_dvb_s2_table_rc_constant_assignment_provided, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_rc_tree, hf_dvb_s2_table_rc_volume_allowed, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_rc_tree, hf_dvb_s2_table_rc_rbdc_allowed, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_rc_tree, hf_dvb_s2_table_rc_maximum_service_rate, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    proto_tree_add_item(dvb_s2_hdr_table_rc_tree, hf_dvb_s2_table_rc_minimum_service_rate, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    if(constant_assign_provided)
                    {
                        proto_tree_add_item(dvb_s2_hdr_table_rc_tree, hf_dvb_s2_table_rc_constant_service_rate, tvb, cur_off + new_off, 2, ENC_NA);
                        new_off += 2;
                    }
                    if(volume_allowed)
                    {
                        proto_tree_add_item(dvb_s2_hdr_table_rc_tree, hf_dvb_s2_table_rc_maximum_backlog, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                    }
                }
                raac_count = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_DESC_RA_AC_COUNT_MASK;
                proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_desc_ra_ac_count, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                for(cur_raac=0 ; cur_raac<raac_count ; cur_raac++)
                {
                    defaults_field_size = tvb_get_guint8(tvb, cur_off + new_off + 4);
                    raac_size = 5 + defaults_field_size;
                    ti = proto_tree_add_item(dvb_s2_hdr_table_desc_tree, hf_dvb_s2_table_ra_ac, tvb, cur_off + new_off, raac_size, ENC_NA);
                    dvb_s2_hdr_table_raac_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_desc);
                    proto_tree_add_item(dvb_s2_hdr_table_raac_tree, hf_dvb_s2_table_ra_ac_index, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_raac_tree, hf_dvb_s2_table_ra_ac_max_unique_payload_per_block, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_raac_tree, hf_dvb_s2_table_ra_ac_max_consecutive_block_accessed, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_raac_tree, hf_dvb_s2_table_ra_ac_min_idle_block, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_raac_tree, hf_dvb_s2_table_ra_ac_defaults_field_size, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_raac_tree, hf_dvb_s2_table_ra_ac_defaults_for_ra_load_control,
                                        tvb, cur_off + new_off, defaults_field_size, ENC_NA);
                    new_off += defaults_field_size;
                }
                break;
            default:
                new_off += desc_length;
                break;
        }
    }

    return new_off;
}

static int dissect_dvb_s2_table_sct(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int sf_start_offset, new_off = 0;
    guint8      superframe_loop_count, frame_loop_count;
    int         cur_sf, cur_frame;
    proto_item *ti, *tf;

    proto_tree  *dvb_s2_hdr_table_sf_tree, *dvb_s2_hdr_table_sf_frame_tree;

    superframe_loop_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_superframe_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    for(cur_sf=0 ; cur_sf<=superframe_loop_count ; cur_sf++)
    {
        sf_start_offset = new_off;
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_superframe, tvb, cur_off + new_off, -1, ENC_NA);
        dvb_s2_hdr_table_sf_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_sf);
        if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
            proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_id, tvb, cur_off + new_off, 1, ENC_NA);
        else if (dvb_s2_rcs_version == DVB_S2_RCS2_TABLE_DECODING)
            proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_sequence, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_large_timing_uncertaintly_flag, tvb, cur_off + new_off, 1, ENC_NA);
        proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_uplink_polarization, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_absolute_time , tvb, cur_off + new_off, 6, ENC_NA);
        new_off += 6;
        proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_duration, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
        proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_centre_frequency, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
        proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_count, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        frame_loop_count = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_SCT_FRAME_LOOP_COUNT_MASK;
        proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_frame_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;

        for(cur_frame=0 ; cur_frame<=frame_loop_count ; cur_frame++)
        {
            tf = proto_tree_add_item(dvb_s2_hdr_table_sf_tree, hf_dvb_s2_table_sf_frame, tvb, cur_off + new_off,
                                      DVB_S2_TABLE_SCT_FRAME_LEN, ENC_NA);
            dvb_s2_hdr_table_sf_frame_tree = proto_item_add_subtree(tf, ett_dvb_s2_hdr_table_sf_frame);
            if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
                proto_tree_add_item(dvb_s2_hdr_table_sf_frame_tree, hf_dvb_s2_table_sf_frame_id, tvb, cur_off + new_off, 1, ENC_NA);
            if (dvb_s2_rcs_version == DVB_S2_RCS2_TABLE_DECODING)
                proto_tree_add_item(dvb_s2_hdr_table_sf_frame_tree, hf_dvb_s2_table_sf_frame_type, tvb, cur_off + new_off, 1, ENC_NA);
            new_off += 1;
            proto_tree_add_item(dvb_s2_hdr_table_sf_frame_tree, hf_dvb_s2_table_sf_frame_start_time, tvb, cur_off + new_off, 4, ENC_NA);
            new_off += 4;
            proto_tree_add_item(dvb_s2_hdr_table_sf_frame_tree, hf_dvb_s2_table_sf_frame_centre_frequency_offset, tvb, cur_off + new_off, 3, ENC_NA);
            new_off += 3;
        }
        proto_item_set_len(ti, new_off - sf_start_offset);
    }
    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_crc32, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
    }

    return new_off;
}

static int dissect_dvb_s2_table_tim(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree, guint16 table_len, gboolean isUnicast)
{
    int desc_loop_count, new_off = 0;
    int pad_bytes_length = 0;

    /* compensate private header format for RCS */
    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
        new_off += 4;

    if(isUnicast)
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_rcst_status, tvb, cur_off + new_off, 1, ENC_NA);
    else
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_network_status, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    desc_loop_count = tvb_get_guint8(tvb, cur_off + new_off);
    new_off += 1;

    new_off += dissect_dvb_s2_table_desc(tvb, cur_off + new_off, dvb_s2_hdr_table_tree, desc_loop_count);

    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
        if(new_off != (table_len - DVB_S2_TABLE_HEADER_LEN - DVB_S2_TABLE_CRC32_LEN))
            pad_bytes_length = table_len - DVB_S2_TABLE_HEADER_LEN - DVB_S2_TABLE_CRC32_LEN - new_off;
    if (dvb_s2_rcs_version == DVB_S2_RCS2_TABLE_DECODING)
        if(new_off != (table_len - DVB_S2_TABLE_HEADER_RCS2_LEN))
            pad_bytes_length = table_len - DVB_S2_TABLE_HEADER_RCS2_LEN - new_off;

    if(pad_bytes_length > 0)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_pad_bytes, tvb, cur_off + new_off, pad_bytes_length, ENC_NA);
        new_off += pad_bytes_length;
    }

    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_crc32, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
    }

    return new_off;
}

static int dissect_dvb_s2_table_tbtp2(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree, guint16 table_len)
{
    int frame_loop_count, frame_start_offset, assign_start_offset, cur_frame, cur_assign, new_off = 0;
    guint16 assignment_loop_count;
    guint8 assignment_format = 0;
    proto_item *ti, *tf;
    proto_tree *dvb_s2_hdr_table_frame_tree, *dvb_s2_hdr_table_frame_assign_tree;

    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_group_id, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_sf_sequence, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_assign_context, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_superframe_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    assignment_format = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_assign_format, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    frame_loop_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_frame_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    for(cur_frame=0 ; cur_frame<=frame_loop_count ; cur_frame++)
    {
        frame_start_offset = new_off;
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_sf_frame, tvb, cur_off + new_off, -1, ENC_NA);
        dvb_s2_hdr_table_frame_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_frame);
        proto_tree_add_item(dvb_s2_hdr_table_frame_tree, hf_dvb_s2_table_frame_number, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_frame_tree,hf_dvb_s2_table_frame_assign_offset, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        assignment_loop_count = tvb_get_ntohs(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_frame_tree, hf_dvb_s2_table_frame_assign_loop_count, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        for(cur_assign=0 ; cur_assign<=assignment_loop_count ; cur_assign++)
        {
            assign_start_offset = new_off;
            tf = proto_tree_add_item(dvb_s2_hdr_table_frame_tree, hf_dvb_s2_table_frame_assignment, tvb, cur_off + new_off, -1, ENC_NA);
            dvb_s2_hdr_table_frame_assign_tree = proto_item_add_subtree(tf, ett_dvb_s2_hdr_table_frame_assign);
            switch (assignment_format) {
                case 0 :
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_assign_id48, tvb, cur_off + new_off, 6, ENC_NA);
                    new_off += 6;
                    break;
                case 1 :
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_assign_id8, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    break;
                case 2 :
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_assign_id16, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    break;
                case 3 :
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_assign_id24, tvb, cur_off + new_off, 3, ENC_NA);
                    new_off += 3;
                    break;
                case 10 :
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_dynamic_tx_type, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_assign_id8, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    break;
                case 11 :
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_dynamic_tx_type, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_assign_id16, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    break;
                case 12 :
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_dynamic_tx_type, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_frame_assign_tree, hf_dvb_s2_table_frame_assign_id24, tvb, cur_off + new_off, 3, ENC_NA);
                    new_off += 3;
                    break;
            }
            proto_item_set_len(tf, new_off - assign_start_offset);
        }
        proto_item_set_len(ti, new_off - frame_start_offset);
    }
    return (table_len - DVB_S2_TABLE_HEADER_RCS2_LEN);
}

static int dissect_dvb_s2_table_cmt(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int entry_loop_count, cur_entry, entry_start_offset, new_off = 0;
    proto_item *ti;
    proto_tree  *dvb_s2_hdr_table_entry_tree;

    entry_loop_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_entry_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    for(cur_entry=0 ; cur_entry<=entry_loop_count ; cur_entry++)
    {
        entry_start_offset = new_off;
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_entry, tvb, cur_off + new_off, -1, ENC_NA);
        dvb_s2_hdr_table_entry_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_entry);
        proto_tree_add_item(dvb_s2_hdr_table_entry_tree, hf_dvb_s2_table_group_id, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_entry_tree, hf_dvb_s2_table_entry_login_id, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;

        new_off += dissect_dvb_s2_table_correct_msg(tvb, cur_off + new_off, dvb_s2_hdr_table_entry_tree);
        proto_item_set_len(ti, new_off - entry_start_offset);
    }

    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_crc32, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
    }

    return new_off;
}

static int dissect_dvb_s2_table_tmst2(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int txmode_count, cur_txmode, new_off = 0;
    proto_item *ti;
    proto_tree  *dvb_s2_hdr_table_txmode_tree;

    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_common_sytem_margin, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    txmode_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_tx_mode_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    for(cur_txmode=0 ; cur_txmode<txmode_count ; cur_txmode++)
    {
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_tx_mode, tvb, cur_off + new_off, DVB_S2_TABLE_TX_MODE_SIZE, ENC_NA);
        dvb_s2_hdr_table_txmode_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_txmode);
        proto_tree_add_item(dvb_s2_hdr_table_txmode_tree, hf_dvb_s2_table_tx_mode_frame_length, tvb, cur_off + new_off, 1, ENC_NA);
        proto_tree_add_item(dvb_s2_hdr_table_txmode_tree, hf_dvb_s2_table_tx_mode_pilot_symbols, tvb, cur_off + new_off, 1, ENC_NA);
        proto_tree_add_item(dvb_s2_hdr_table_txmode_tree, hf_dvb_s2_table_tx_mode_modcod, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_txmode_tree, hf_dvb_s2_table_tx_mode_modcod_system_margin, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_txmode_tree, hf_dvb_s2_table_tx_mode_isi, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
    }
    return new_off;
}

static int dissect_dvb_s2_table_bct(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int txtype_count, cur_txtype, tx_format_data_length, uw_symbol_len, cur_uwsymbol, uwsegment_count, cur_uwsegment, cur_period, cpm_offset, new_off = 0;
    proto_item *ti;
    proto_tree  *dvb_s2_hdr_table_txtype_tree, *dvb_s2_hdr_table_txtype_uwsegment_tree,
                *dvb_s2_hdr_table_txtype_ypattern_tree, *dvb_s2_hdr_table_txtype_wpattern_tree,
                *dvb_s2_hdr_table_txtype_uwsymbol_tree;
    guint8 tx_format, tx_type, y_period, w_period, modulation_scheme, param_interleaver;

    txtype_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_tx_type_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    for(cur_txtype=0 ; cur_txtype<txtype_count ; cur_txtype++)
    {
        tx_format_data_length = tvb_get_guint8(tvb, cur_off + new_off + 3);
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_tx_type_branch, tvb, cur_off + new_off, tx_format_data_length + 4, ENC_NA);
        dvb_s2_hdr_table_txtype_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_txtype);
        tx_type = tvb_get_guint8(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_content_type, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        tx_format = tvb_get_guint8(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_format_class, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_format_data_lenght, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        switch(tx_format)
        {
            case DVB_S2_TABLE_TXFORMAT_LMBT :
                proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_block_size, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_threshold_es_n0, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_start_offset_1, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_start_offset_2, tvb, cur_off + new_off, 3, ENC_NA);
                new_off += 3;
                if(tx_type > 127)
                {
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_payload_size, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    modulation_scheme = tvb_get_guint8(tvb, cur_off + new_off);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_modulation_scheme, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_p, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_q0, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_q1, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_q2, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_q3, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    y_period = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_TX_TYPE_W_Y_PERIOD_MASK;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_y_period, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    w_period = tvb_get_guint8(tvb, cur_off + new_off) & DVB_S2_TABLE_TX_TYPE_W_Y_PERIOD_MASK;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_w_period, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    ti = proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_y_pattern, tvb, cur_off + new_off, y_period, ENC_NA);
                    dvb_s2_hdr_table_txtype_ypattern_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_txtype_ypattern);
                    for(cur_period=0 ; cur_period<y_period ; cur_period++)
                    {
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_ypattern_tree, hf_dvb_s2_table_tx_type_y_pattern_bit, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                    }
                    ti = proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_w_pattern, tvb, cur_off + new_off, w_period, ENC_NA);
                    dvb_s2_hdr_table_txtype_wpattern_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_txtype_wpattern);
                    for(cur_period=0 ; cur_period<w_period ; cur_period++)
                    {
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_wpattern_tree, hf_dvb_s2_table_tx_type_w_pattern_bit, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                    }
                    uw_symbol_len = 0;
                    uw_symbol_len += tvb_get_guint8(tvb, cur_off + new_off);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_preamble_len, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    uw_symbol_len += tvb_get_guint8(tvb, cur_off + new_off);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_postamble_len, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_pilot_period, tvb, cur_off + new_off, 2, ENC_NA);
                    new_off += 2;
                    uw_symbol_len += tvb_get_guint8(tvb, cur_off + new_off);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_pilot_block_len, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_pilot_sum, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    ti = proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_uw_symbol, tvb, cur_off + new_off, uw_symbol_len, ENC_NA);
                    dvb_s2_hdr_table_txtype_uwsymbol_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_txtype_uwsymbol);
                    for(cur_uwsymbol=0 ; cur_uwsymbol<uw_symbol_len ; cur_uwsymbol++)
                    {
                        switch(modulation_scheme)
                        {
                            case DVB_S2_TABLE_MODULATION_SCHEME_QPSK:
                                proto_tree_add_item(dvb_s2_hdr_table_txtype_uwsymbol_tree, hf_dvb_s2_table_tx_type_uw_symbol_qpsk,
                                                    tvb, cur_off + new_off, 1, ENC_NA);
                                new_off += 1;
                                break;
                            case DVB_S2_TABLE_MODULATION_SCHEME_8PSK:
                                proto_tree_add_item(dvb_s2_hdr_table_txtype_uwsymbol_tree, hf_dvb_s2_table_tx_type_uw_symbol_8psk,
                                                    tvb, cur_off + new_off, 1, ENC_NA);
                                new_off += 1;
                                break;
                            case DVB_S2_TABLE_MODULATION_SCHEME_16QAM:
                                proto_tree_add_item(dvb_s2_hdr_table_txtype_uwsymbol_tree, hf_dvb_s2_table_tx_type_uw_symbol_16qam,
                                                    tvb, cur_off + new_off, 1, ENC_NA);
                                new_off += 1;
                                break;
                            default:
                                proto_tree_add_item(dvb_s2_hdr_table_txtype_uwsymbol_tree, hf_dvb_s2_table_tx_type_uw_symbol_unit,
                                                    tvb, cur_off + new_off, 1, ENC_NA);
                                new_off += 1;
                                break;
                        }
                    }
               }
                else
                {
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_waveform_id, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                }
                break;
            case DVB_S2_TABLE_TXFORMAT_CPMBT :
                cpm_offset = new_off;
                proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_block_size, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_threshold_es_n0, tvb, cur_off + new_off, 1, ENC_NA);
                new_off += 1;
                proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_start_offset, tvb, cur_off + new_off, 4, ENC_NA);
                new_off += 4;
                if(tx_type > 127)
                {
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_modulation_mh, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_modulation_ph, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_modulation_type, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_alpha_rc, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_code_rate, tvb, cur_off + new_off, 1, ENC_NA);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_constraint_length_k, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    uw_symbol_len = ( tvb_get_guint8(tvb, cur_off + new_off) * 2 ) / 8 + 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_uw_length, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_uw_symbol, tvb, cur_off + new_off, uw_symbol_len, ENC_NA);
                    new_off += uw_symbol_len;
                    uwsegment_count = tvb_get_guint8(tvb, cur_off + new_off);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_nbr_uw_segments, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    for(cur_uwsegment=0 ; cur_uwsegment<uwsegment_count ; cur_uwsegment++)
                    {
                        ti = proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_uw_segment, tvb, cur_off + new_off,
                                                 DVB_S2_TABLE_TX_TYPE_UW_SEGMENT_SIZE, ENC_NA);
                        dvb_s2_hdr_table_txtype_uwsegment_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_txtype_uwsegment);
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_uwsegment_tree, hf_dvb_s2_table_tx_type_uw_segment_start, tvb, cur_off + new_off, 2, ENC_NA);
                        new_off += 2;
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_uwsegment_tree, hf_dvb_s2_table_tx_type_uw_segment_length, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                    }
                    param_interleaver = tvb_get_guint8(tvb, cur_off + new_off);
                    proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_param_interleaver, tvb, cur_off + new_off, 1, ENC_NA);
                    new_off += 1;
                    if(param_interleaver)
                    {
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_n, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_s, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_p_interleaver, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_n1_12, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_k1_12, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_K2_12, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_K3_12, tvb, cur_off + new_off, 1, ENC_NA);
                        new_off += 1;
                    }
                    else
                    {
                        proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_pi_i, tvb, cur_off + new_off,
                                            tx_format_data_length - (new_off - cpm_offset), ENC_NA);
                        new_off += tx_format_data_length - (new_off - cpm_offset);
                    }
                }
                break;
            case DVB_S2_TABLE_TXFORMAT_CT :
            case DVB_S2_TABLE_TXFORMAT_SSLMBT :
            default :
                proto_tree_add_item(dvb_s2_hdr_table_txtype_tree, hf_dvb_s2_table_tx_type_tx_format_data, tvb, cur_off + new_off, tx_format_data_length, ENC_NA);
                new_off += tx_format_data_length;
                break;
        }
    }
    return new_off;
}

static int dissect_dvb_s2_table_fat(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int desc_loop_count, new_off = 0;

    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_desc_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    desc_loop_count = tvb_get_guint8(tvb, cur_off + new_off);
    new_off += 1;

    new_off += dissect_dvb_s2_table_desc(tvb, cur_off + new_off, dvb_s2_hdr_table_tree, desc_loop_count);

    return new_off;
}

static int dissect_dvb_s2_table_fct2(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int frametype_count, cur_frametype, frame_type_start_off, gridrepeat_count, cur_gridrepeat, section_count, cur_section, new_off = 0;
    proto_item *ti, *tf;
    proto_tree  *dvb_s2_hdr_table_frametype_tree, *dvb_s2_hdr_table_frametype_section_tree;

    frametype_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_frame_type_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    for(cur_frametype=0 ; cur_frametype<=frametype_count ; cur_frametype++)
    {
        frame_type_start_off = new_off;
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_frame_type_branch, tvb, cur_off + new_off, -1, ENC_NA);
        dvb_s2_hdr_table_frametype_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_frametype);
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_frame_duration, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_tx_format_class, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_btu_duration, tvb, cur_off + new_off, 3, ENC_NA);
        new_off += 3;
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_btu_carrier_bw, tvb, cur_off + new_off, 3, ENC_NA);
        new_off += 3;
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_btu_symbol_rate, tvb, cur_off + new_off, 3, ENC_NA);
        new_off += 3;
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_time_unit_count, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        gridrepeat_count = tvb_get_guint8(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_grid_repeat_count, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        for(cur_gridrepeat=0 ; cur_gridrepeat<gridrepeat_count ; cur_gridrepeat++)
        {
            proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_grid_frequency_offset, tvb, cur_off + new_off, 3, ENC_NA);
            new_off += 3;
        }
        section_count = tvb_get_guint8(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_section_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        for(cur_section=0 ; cur_section<=section_count ; cur_section++)
        {
            tf = proto_tree_add_item(dvb_s2_hdr_table_frametype_tree, hf_dvb_s2_table_frame_type_section, tvb, cur_off + new_off, 4, ENC_NA);
            dvb_s2_hdr_table_frametype_section_tree = proto_item_add_subtree(tf, ett_dvb_s2_hdr_table_frametype_section);
            proto_tree_add_item(dvb_s2_hdr_table_frametype_section_tree, hf_dvb_s2_table_frame_type_section_default_tx_type, tvb, cur_off + new_off, 1, ENC_NA);
            new_off += 1;
            proto_tree_add_item(dvb_s2_hdr_table_frametype_section_tree, hf_dvb_s2_table_frame_type_section_fix_acc_method, tvb, cur_off + new_off, 1, ENC_NA);
            new_off += 1;
            proto_tree_add_item(dvb_s2_hdr_table_frametype_section_tree, hf_dvb_s2_table_frame_type_section_repeat_count, tvb, cur_off + new_off, 2, ENC_NA);
            new_off += 2;
        }
        proto_item_set_len(ti, new_off - frame_type_start_off);
    }

    return new_off;
}

static int dissect_dvb_s2_table_fct(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int frame_ID_count, cur_frame_ID, frame_type_start_off, timeslot_loop_count, cur_timeslot, new_off = 0;
    proto_item *ti, *tf;
    proto_tree  *dvb_s2_hdr_table_frame_ID_tree, *dvb_s2_hdr_table_frame_ID_timeslot_tree;

    frame_ID_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_frame_ID_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    for(cur_frame_ID=0 ; cur_frame_ID<=frame_ID_count ; cur_frame_ID++)
    {
        frame_type_start_off = new_off;
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_frame_ID_branch, tvb, cur_off + new_off, -1, ENC_NA);
        dvb_s2_hdr_table_frame_ID_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_frame_ID);
        proto_tree_add_item(dvb_s2_hdr_table_frame_ID_tree, hf_dvb_s2_table_frame_ID, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_frame_ID_tree, hf_dvb_s2_table_frame_ID_frame_duration, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
        proto_tree_add_item(dvb_s2_hdr_table_frame_ID_tree, hf_dvb_s2_table_frame_ID_total_timeslot_count, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        proto_tree_add_item(dvb_s2_hdr_table_frame_ID_tree, hf_dvb_s2_table_frame_ID_start_timeslot_number, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        timeslot_loop_count = tvb_get_guint8(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_frame_ID_tree, hf_dvb_s2_table_frame_ID_timeslot_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        for(cur_timeslot=0 ; cur_timeslot<=timeslot_loop_count ; cur_timeslot++)
        {
            tf = proto_tree_add_item(dvb_s2_hdr_table_frame_ID_tree, hf_dvb_s2_table_frame_ID_timeslot, tvb, cur_off + new_off, 9, ENC_NA);
            dvb_s2_hdr_table_frame_ID_timeslot_tree = proto_item_add_subtree(tf, ett_dvb_s2_hdr_table_frame_ID_timeslot);
            proto_tree_add_item(dvb_s2_hdr_table_frame_ID_timeslot_tree, hf_dvb_s2_table_frame_ID_timeslot_timeslot_frequency_offset, tvb, cur_off + new_off, 3, ENC_NA);
            new_off += 3;
            proto_tree_add_item(dvb_s2_hdr_table_frame_ID_timeslot_tree, hf_dvb_s2_table_frame_ID_timeslot_timeslot_time_offset, tvb, cur_off + new_off, 4, ENC_NA);
            new_off += 4;
            proto_tree_add_item(dvb_s2_hdr_table_frame_ID_timeslot_tree, hf_dvb_s2_table_frame_ID_timeslot_timeslot_id, tvb, cur_off + new_off, 1, ENC_NA);
            new_off += 1;
            proto_tree_add_item(dvb_s2_hdr_table_frame_ID_timeslot_tree, hf_dvb_s2_table_frame_ID_timeslot_repeat_count, tvb, cur_off + new_off, 1, ENC_NA);
            new_off += 1;
        }
        proto_item_set_len(ti, new_off - frame_type_start_off);
    }
    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_crc32, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
    }
    return new_off;
}

static int dissect_dvb_s2_table_spt(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int satellite_count, cur_satellite, new_off = 0;
    proto_item *ti;
    proto_tree  *dvb_s2_hdr_table_satellite_tree;

    satellite_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_satellite_loop_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    for(cur_satellite=0 ; cur_satellite<=satellite_count ; cur_satellite++)
    {
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_satellite, tvb, cur_off + new_off, DVB_S2_TABLE_SAT_SIZE, ENC_NA);
        dvb_s2_hdr_table_satellite_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_satellite);
        proto_tree_add_item(dvb_s2_hdr_table_satellite_tree, hf_dvb_s2_table_satellite_id, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_satellite_tree, hf_dvb_s2_table_satellite_x_coordinate, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
        proto_tree_add_item(dvb_s2_hdr_table_satellite_tree, hf_dvb_s2_table_satellite_y_coordinate, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
        proto_tree_add_item(dvb_s2_hdr_table_satellite_tree, hf_dvb_s2_table_satellite_z_coordinate, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
    }
    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_crc32, tvb, cur_off + new_off, 4, ENC_NA);
        new_off += 4;
    }
    return new_off;
}

static int dissect_dvb_s2_table_smt(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int new_off = 0;

    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_smt_id, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_smt_section_syntax_indicator, tvb, cur_off + new_off, 2, ENC_NA);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_smt_futur_use, tvb, cur_off + new_off, 2, ENC_NA);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_smt_reserved, tvb, cur_off + new_off, 2, ENC_NA);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_smt_section_length, tvb, cur_off + new_off, 2, ENC_NA);
    new_off += 2;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_smt_esn0, tvb, cur_off + new_off, 2, ENC_NA);
    new_off += 2;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_smt_modcod, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_crc32, tvb, cur_off + new_off, DVB_S2_TABLE_CRC32_LEN, ENC_NA);
    new_off += 4;

    return new_off;
}

static int dissect_dvb_s2_table_nit_rmt(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree, guint8 table_id)
{
    int desc_off, desc_count, max_multiplex_off, mux_start_off, new_off = 0;
    guint16 network_descriptors_length, multiplex_streams_spec_length, transport_descriptors_length;
    proto_item *ti;
    proto_tree  *dvb_s2_hdr_table_multiplex_tree;

    network_descriptors_length = tvb_get_ntohs(tvb, cur_off + new_off) & DVB_S2_TABLE_NETWORK_DESC_LENGTH_MASK;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_network_descriptors_length, tvb, cur_off + new_off, 2, ENC_NA);
    new_off += 2;
    //proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_network_descriptors, tvb, cur_off + new_off, network_descriptors_length, ENC_NA);
    //new_off += network_descriptors_length;

    /* calculate descriptors_count */
    desc_off=0;
    for(desc_count=0 ; desc_off<network_descriptors_length ; desc_count++){
        desc_off += tvb_get_guint8(tvb, cur_off + new_off + desc_off + 1) + 2;
    }

    new_off += dissect_dvb_s2_table_desc(tvb, cur_off + new_off, dvb_s2_hdr_table_tree, desc_count - 1);

    multiplex_streams_spec_length = tvb_get_ntohs(tvb, cur_off + new_off) & DVB_S2_TABLE_MULTIPLEX_STREAMS_LENGTH_MASK;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_multiplex_streams_spec_length, tvb, cur_off + new_off, 2, ENC_NA);
    new_off += 2;
    //proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_multiplex_streams_spec, tvb, cur_off + new_off, multiplex_streams_spec_length, ENC_NA);
    //new_off += multiplex_streams_spec_length;

    max_multiplex_off = new_off + multiplex_streams_spec_length;
    while(new_off < max_multiplex_off)
    {
        mux_start_off = new_off;
        ti = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_multiplex, tvb, cur_off + new_off, -1, ENC_NA);
        dvb_s2_hdr_table_multiplex_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table_multiplex);
        proto_tree_add_item(dvb_s2_hdr_table_multiplex_tree, hf_dvb_s2_table_multiplex_forward_multiplex, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        if(table_id == DVB_S2_TABLE_NIT)
        {
            proto_tree_add_item(dvb_s2_hdr_table_multiplex_tree, hf_dvb_s2_table_multiplex_original_network_id, tvb, cur_off + new_off, 2, ENC_NA);
            new_off += 2;
        }
        else
        {
            proto_tree_add_item(dvb_s2_hdr_table_multiplex_tree, hf_dvb_s2_table_multiplex_reward_multiplex, tvb, cur_off + new_off, 2, ENC_NA);
            new_off += 2;
        }
        transport_descriptors_length = tvb_get_ntohs(tvb, cur_off + new_off) & DVB_S2_TABLE_MULTIPLEX_TRANSPORT_DESC_LENGTH_MASK;
        proto_tree_add_item(dvb_s2_hdr_table_multiplex_tree, hf_dvb_s2_table_multiplex_transport_descriptors_length, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        /* calculate descriptors_count */
        desc_off=0;
        for(desc_count=0 ; desc_off<transport_descriptors_length ; desc_count++)
            desc_off += tvb_get_guint8(tvb, cur_off + new_off + desc_off + 1) + 2;
        new_off += dissect_dvb_s2_table_desc(tvb, cur_off + new_off, dvb_s2_hdr_table_multiplex_tree, desc_count - 1);
        proto_item_set_len(ti, new_off - mux_start_off);
    }

    return new_off;
}

static int dissect_dvb_s2_table_mmt2(tvbuff_t *tvb, int cur_off, proto_tree *dvb_s2_hdr_table_tree)
{
    int protocol_count, cur_protocol, pt_start_off, ms_count, cur_ms, ms_start_off, exclusion_count, cur_exclusion, new_off = 0;
    proto_item *ti_pt, *ti_ms, *ti_exc;
    proto_tree  *dvb_s2_hdr_table_protocol_tree, *dvb_s2_hdr_table_protocol_ms_tree, *dvb_s2_hdr_table_protocol_ms_excl_tree;
    guint8 address_size;

    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_svn_number, tvb, cur_off + new_off, 2, ENC_NA);
    new_off += 2;
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_svn_prefix_size, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;
    protocol_count = tvb_get_guint8(tvb, cur_off + new_off);
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_pt_count, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    for(cur_protocol=0 ; cur_protocol<=protocol_count ; cur_protocol++)
    {
        pt_start_off = new_off;
        ti_pt = proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_protocol, tvb, cur_off + new_off, -1, ENC_NA);
        dvb_s2_hdr_table_protocol_tree = proto_item_add_subtree(ti_pt, ett_dvb_s2_hdr_table_pt);
        proto_tree_add_item(dvb_s2_hdr_table_protocol_tree, hf_dvb_s2_table_pt_protocol_type, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 1;
        address_size = tvb_get_guint8(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_protocol_tree, hf_dvb_s2_table_pt_address_size, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        ms_count = tvb_get_guint8(tvb, cur_off + new_off);
        proto_tree_add_item(dvb_s2_hdr_table_protocol_tree, hf_dvb_s2_table_pt_mapping_sections, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        for(cur_ms=0 ; cur_ms<=ms_count ; cur_ms++)
        {
            ms_start_off = new_off;
            ti_ms = proto_tree_add_item(dvb_s2_hdr_table_protocol_tree, hf_dvb_s2_table_pt_mapping_section, tvb, cur_off + new_off, -1, ENC_NA);
            dvb_s2_hdr_table_protocol_ms_tree = proto_item_add_subtree(ti_ms, ett_dvb_s2_hdr_table_pt_ms);
            proto_tree_add_item(dvb_s2_hdr_table_protocol_ms_tree, hf_dvb_s2_table_pt_ms_inclusion_start, tvb, cur_off + new_off, address_size, ENC_NA);
            new_off += address_size;
            proto_tree_add_item(dvb_s2_hdr_table_protocol_ms_tree, hf_dvb_s2_table_pt_ms_inclusion_end, tvb, cur_off + new_off, address_size, ENC_NA);
            new_off += address_size;
            exclusion_count = tvb_get_guint8(tvb, cur_off + new_off);
            proto_tree_add_item(dvb_s2_hdr_table_protocol_ms_tree, hf_dvb_s2_table_pt_ms_exclusions, tvb, cur_off + new_off, 1, ENC_NA);
            new_off += 1;
            for(cur_exclusion=0 ; cur_exclusion<=exclusion_count ; cur_exclusion++)
            {
                ti_exc = proto_tree_add_item(dvb_s2_hdr_table_protocol_ms_tree, hf_dvb_s2_table_pt_ms_exclusion, tvb, cur_off + new_off, 2 * address_size, ENC_NA);
                dvb_s2_hdr_table_protocol_ms_excl_tree = proto_item_add_subtree(ti_exc, ett_dvb_s2_hdr_table_pt_ms_exclusion);
                proto_tree_add_item(dvb_s2_hdr_table_protocol_ms_excl_tree, hf_dvb_s2_table_pt_ms_exclusion_start, tvb, cur_off + new_off, address_size, ENC_NA);
                new_off += address_size;
                proto_tree_add_item(dvb_s2_hdr_table_protocol_ms_excl_tree, hf_dvb_s2_table_pt_ms_exclusion_end, tvb, cur_off + new_off, address_size, ENC_NA);
                new_off += address_size;
            }
            proto_tree_add_item(dvb_s2_hdr_table_protocol_ms_tree, hf_dvb_s2_table_pt_ms_mac24_base, tvb, cur_off + new_off, 3, ENC_NA);
            new_off += 3;
            proto_tree_add_item(dvb_s2_hdr_table_protocol_ms_tree, hf_dvb_s2_table_pt_ms_mcast_prefix_size, tvb, cur_off + new_off, 1, ENC_NA);
            new_off += 1;
            proto_item_set_len(ti_ms, new_off - ms_start_off);
        }
        proto_item_set_len(ti_pt, new_off - pt_start_off);
    }
    return new_off;
}

static int dissect_dvb_s2_table(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
    int         new_off                      = 0;
    int         cur_off                      = 0;
    guint16     data_len                     = 0;
    guint8      table_id                     = 0;
    gboolean    dvb_s2_isUnicast         = TRUE;

    proto_item *ti = NULL;
    proto_tree *dvb_s2_hdr_table_tree;

    ti = proto_tree_add_item(tree, proto_dvb_s2_table, tvb, 0, -1, ENC_NA);
    dvb_s2_hdr_table_tree = proto_item_add_subtree(ti, ett_dvb_s2_hdr_table);
    table_id = tvb_get_guint8(tvb, cur_off + new_off);
    col_append_str(pinfo->cinfo, COL_INFO, val_to_str_const(table_id, tabletype, "Unknown table id" ));
    col_append_str(pinfo->cinfo, COL_INFO, " ");
    proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_id, tvb, cur_off + new_off, 1, ENC_NA);
    new_off += 1;

    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_section, tvb, cur_off + new_off, 1, ENC_NA);
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_private, tvb, cur_off + new_off, 1, ENC_NA);
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_reserved, tvb, cur_off + new_off, 1, ENC_NA);
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_msb_len, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_lsb_len, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
    }
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_network_interactive_id, tvb, cur_off + new_off, 2, ENC_NA);
        new_off += 2;
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_reserved2, tvb, cur_off + new_off, 1, ENC_NA);
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_version_number, tvb, cur_off + new_off, 1, ENC_NA);
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_current_next_indicator, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;

    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
    {
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_section_number, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
        proto_tree_add_item(dvb_s2_hdr_table_tree, hf_dvb_s2_table_last_section_number, tvb, cur_off + new_off, 1, ENC_NA);
        new_off += 1;
    }

    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
        data_len = tvb_captured_length(tvb);
    else if (dvb_s2_rcs_version == DVB_S2_RCS2_TABLE_DECODING)
        data_len = tvb_captured_length(tvb);

    switch (table_id) {
        case DVB_S2_TABLE_SMT:
            dissect_dvb_s2_table_smt(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_SCT:
            dissect_dvb_s2_table_sct(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_TIM:
            dissect_dvb_s2_table_tim(tvb, cur_off + new_off, dvb_s2_hdr_table_tree, data_len, dvb_s2_isUnicast);
            break;
        case DVB_S2_TABLE_TBTP2:
            dissect_dvb_s2_table_tbtp2(tvb, cur_off + new_off, dvb_s2_hdr_table_tree, data_len);
            break;
        case DVB_S2_TABLE_CMT:
            dissect_dvb_s2_table_cmt(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_FAT:
            dissect_dvb_s2_table_fat(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_FCT:
            dissect_dvb_s2_table_fct(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_TMST2:
            dissect_dvb_s2_table_tmst2(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_TCTE:
            if (dvb_s2_rcs_version == DVB_S2_RCS2_TABLE_DECODING)
            {
                dissect_dvb_s2_table_fct2(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            }
            break;
        case DVB_S2_TABLE_BCT:
            dissect_dvb_s2_table_bct(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_SPT:
            dissect_dvb_s2_table_spt(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_NIT:
        case DVB_S2_TABLE_RMT:
            if (dvb_s2_rcs_version == DVB_S2_RCS2_TABLE_DECODING)
            {
                dissect_dvb_s2_table_nit_rmt(tvb, cur_off + new_off, dvb_s2_hdr_table_tree, table_id);
            }
            break;
        case DVB_S2_TABLE_MMT2:
            dissect_dvb_s2_table_mmt2(tvb, cur_off + new_off, dvb_s2_hdr_table_tree);
            break;
        case DVB_S2_TABLE_PAT:
        case DVB_S2_TABLE_CAT:
        case DVB_S2_TABLE_PMT:
        case DVB_S2_TABLE_SDT:
        case DVB_S2_TABLE_DTD:
        case DVB_S2_TABLE_TCT:
        case DVB_S2_TABLE_TBTP:
        case DVB_S2_TABLE_PCR:
        case DVB_S2_TABLE_TMST:
        case DVB_S2_TABLE_MMT:
        case DVB_S2_TABLE_TIMB:
            break;
    }
    if (dvb_s2_rcs_version == DVB_S2_RCS_TABLE_DECODING)
        new_off += data_len - DVB_S2_TABLE_HEADER_LEN;
    else if (dvb_s2_rcs_version == DVB_S2_RCS2_TABLE_DECODING)
        new_off += data_len - DVB_S2_TABLE_HEADER_RCS2_LEN;

    return new_off;
}

/* Register the protocol with Wireshark */
void proto_register_dvb_s2_table(void)
{
    module_t *dvb_s2_table_module;

    /* DVB-S2 Table */
    static hf_register_info hf_table[] = {
        {&hf_dvb_s2_table_id, {
                "Table ID", "dvb-s2_table.id",
                FT_UINT8, BASE_HEX, VALS(tabletype), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_section, {
                "Table Section", "dvb-s2_table.section",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_SECTION_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_private, {
                "Table private indicator", "dvb-s2_table.section",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_PRIVATE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_reserved, {
                "Table reserved field", "dvb-s2_table.reserved",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_RESERVED_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_msb_len, {
                "Table length MSB", "dvb-s2_table.len.msb",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_MSB_LEN_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lsb_len, {
                "Table length LSB", "dvb-s2_table.len.lsb",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_network_interactive_id, {
                "Table network interactive id", "dvb-s2_table.network_interactive_id",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_reserved2, {
                "Table reserved field 2", "dvb-s2_table.reserved2",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_RESERVED2_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_version_number, {
                "Table version number", "dvb-s2_table.version_number",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_VERSION_NUMBER_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_current_next_indicator, {
                "Table current next indicator", "dvb-s2_table.current_next_indicator",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_CURRENT_NEXT_INDICATOR_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_section_number, {
                "Table section number", "dvb-s2_table.section_number",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_last_section_number, {
                "Table last section number", "dvb-s2_table.len",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_smt_id, {
                "Table ID", "dvb-s2_table.id",
                FT_UINT8, BASE_HEX, VALS(tabletype), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_smt_section_syntax_indicator, {
                "Table section", "dvb-s2_table.section",
                FT_UINT16, BASE_HEX, NULL, DVB_S2_TABLE_SMT_SECTION_INDICATOR_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_smt_futur_use, {
                "Table future use", "dvb-s2_table.future_use",
                FT_UINT16, BASE_HEX, NULL, DVB_S2_TABLE_SMT_FUTUR_USE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_smt_reserved, {
                "Table reserved", "dvb-s2_table.reserved",
                FT_UINT16, BASE_HEX, NULL, DVB_S2_TABLE_SMT_RESERVED_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_smt_section_length, {
                "Table section length", "dvb-s2_table.section_length",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_SMT_SECTION_LENGTH_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_smt_esn0, {
                "Table Es/N0", "dvb-s2_table.esn0",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_smt_modcod, {
                "Table modcod", "dvb-s2_table.modcod",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
/* SCT */
        {&hf_dvb_s2_table_superframe_loop_count, {
                "Table superframe loop count", "dvb-s2_table.superframe_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_superframe, {
                "Superframe", "dvb-s2_table.superframe",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                "Superframe definition", HFILL}
        },
        {&hf_dvb_s2_table_sf_sequence, {
                "Superframe sequence", "dvb-s2_table.sf.sequence",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_id, {
                "Superframe id", "dvb-s2_table.sf.id",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_large_timing_uncertaintly_flag, {
                "Superframe large timing uncertainty flag", "dvb-s2_table.sf.large_timing_uncertainty_flag",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_SCT_LARGE_TIMING_FLAG_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_uplink_polarization, {
                "Superframe uplink polarization", "dvb-s2_table.sf.uplink_polarization",
                FT_UINT8, BASE_HEX, VALS(table_uplinkPolarization), DVB_S2_TABLE_SCT_UPLINK_POLARIZATION_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_absolute_time, {
                "Superframe absolute time (NCR format)", "dvb-s2_table.sf.absolute",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_duration, {
                "Superframe duration", "dvb-s2_table.sf.duration",
                FT_UINT32, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_centre_frequency, {
                "Superframe center frequency", "dvb-s2_table.sf.center_frequency",
                FT_UINT32, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_count, {
                "Superframe count", "dvb-s2_table.sf.count",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_frame_loop_count, {
                "Superframe frame loop count", "dvb-s2_table.sf.frame_loop_count",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_SCT_FRAME_LOOP_COUNT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_frame, {
                "Frame", "dvb-s2_table.sf.frame",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                "Frame definition", HFILL}
        },
        {&hf_dvb_s2_table_sf_frame_type, {
                "Frame type", "dvb-s2_table.sf.frame.type",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_frame_id, {
                "Frame id", "dvb-s2_table.sf.frame.id",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_frame_start_time, {
                "Frame start time", "dvb-s2_table.sf.frame.start_time",
                FT_UINT32, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_sf_frame_centre_frequency_offset, {
                "Frame center frequency offset", "dvb-s2_table.sf.frame.center_frequency_offset",
                FT_INT24, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
/* TIM */
        {&hf_dvb_s2_table_rcst_status, {
                "Table RCST status", "dvb-s2_table.rcst_status",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_network_status, {
                "Table network status", "dvb-s2_table.network_status",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pad_bytes, {
                "Table PAD bytes", "dvb-s2_table.pad_bytes",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_loop_count, {
                "Table descriptor loop count", "dvb-s2_table.desc_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
/* TBTP2 */
        {&hf_dvb_s2_table_group_id, {
                "Table Group ID", "dvb-s2_table.group_id",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_assign_context, {
                "Table assignment context", "dvb-s2_table.assign_context",
                FT_UINT8, BASE_HEX, VALS(table_assignContext), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_superframe_count, {
                "Superframe count", "dvb-s2_table.superframe_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_assign_format, {
                "Table assignment Format", "dvb-s2_table.assign_format",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_loop_count, {
                "Table frame loop count", "dvb-s2_table.frame_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_number, {
                "Frame number", "dvb-s2_table.frame.number",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_assign_offset, {
                "Frame assignment offset", "dvb-s2_table.frame.assign_offset",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_assign_loop_count, {
                "Frame assignment loop count", "dvb-s2_table.frame.assign_loop_count",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_assignment, {
                "Frame assignment", "dvb-s2_table.frame.assignment",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_assign_id8, {
                "Frame assignment ID", "dvb-s2_table.frame.assign_id8",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_assign_id16, {
                "Frame assignment ID", "dvb-s2_table.frame.assign_id16",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_assign_id24, {
                "Frame assignment ID", "dvb-s2_table.frame.assign_id24",
                FT_UINT24, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_assign_id48, {
                "Frame assignment ID", "dvb-s2_table.frame.assign_id48",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_dynamic_tx_type, {
                "Frame dynamic tx_type", "dvb-s2_table.frame.dynamic_tx_type",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
/* CMT */
        {&hf_dvb_s2_table_entry_loop_count, {
                "Table entry loop count", "dvb-s2_table.entry_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_entry, {
                "Entry Correction Message", "dvb-s2_table.entry",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_entry_login_id, {
                "Entry login ID", "dvb-s2_table.entry.login_id",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
/* TMST2 */
        {&hf_dvb_s2_table_common_sytem_margin, {
                "Table common system margin", "dvb-s2_table.common_system_margin",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_mode_count, {
                "Table transmission mode count", "dvb-s2_table.tx_mode_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_mode, {
                "Transmission mode", "dvb-s2_table.tx_mode",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_mode_frame_length, {
                "tx mode frame length", "dvb-s2_table.tx_mode.frame_length",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_MODE_FRAME_LENGTH_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_mode_pilot_symbols, {
                "tx mode pilot_symbols", "dvb-s2_table.tx_mode.pilot_symbols",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_MODE_PILOT_SYMBOLS_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_mode_modcod, {
                "tx mode MODCOD", "dvb-s2_table.tx_mode.modcod",
                FT_UINT8, BASE_HEX, VALS(table_modcods), DVB_S2_TABLE_TX_MODE_MODCOD_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_mode_modcod_system_margin, {
                "tx mode modcod system margin", "dvb-s2_table.tx_mode.modcod_system_margin",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_mode_isi, {
                "tx mode ISI", "dvb-s2_table.tx_mode.isi",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
/* FCT2 */
        {&hf_dvb_s2_table_frame_type_loop_count, {
                "Table frame type loop count", "dvb-s2_table.frame_type_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_branch, {
                "Frame type", "dvb-s2_table.frame_type_branch",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type, {
                "Frame type", "dvb-s2_table.frame_type.id",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_frame_duration, {
                "Frame type frame duration", "dvb-s2_table.frame_type.frame_duration",
                FT_UINT32, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_tx_format_class, {
                "Frame type tx format class", "dvb-s2_table.frame_type.tx_format_class",
                FT_UINT8, BASE_HEX, VALS(table_frameType_txFormatClass), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_btu_duration, {
                "Frame type btu duration", "dvb-s2_table.frame_type.btu_duration",
                FT_UINT24, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_btu_carrier_bw, {
                "Frame type btu carrier bw", "dvb-s2_table.frame_type.btu_carrier_bw",
                FT_UINT24, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_btu_symbol_rate, {
                "Frame type btu symbol rate", "dvb-s2_table.frame_type.btu_symbol_rate",
                FT_UINT24, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_time_unit_count, {
                "Frame type time unit count", "dvb-s2_table.frame_type.time_unit_count",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_grid_repeat_count, {
                "Frame type grid repeat count", "dvb-s2_table.frame_type.grid_repeat_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_grid_frequency_offset, {
                "Frame type grid frequency offset", "dvb-s2_table.frame_type.grid_frequency_offset",
                FT_INT24, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_section_loop_count, {
                "Frame type section loop count", "dvb-s2_table.frame_type.section_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_section, {
                "Section", "dvb-s2_table.frame_type.section",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_section_default_tx_type, {
                "Section default tx type", "dvb-s2_table.frame_type.section.default_tx_type",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_section_fix_acc_method, {
                "Section fixed access method", "dvb-s2_table.frame_type.section.fixed_access_method",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_FRAME_TYPE_SECTION_FAM_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_type_section_repeat_count, {
                "Section repeat count", "dvb-s2_table.frame_type.section.repeat_count",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
/* FCT */
        {&hf_dvb_s2_table_frame_ID_loop_count, {
                "Frame loop count", "dvb-s2_table.frame_ID_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_branch, {
                "Frame", "dvb-s2_table.frame_ID_branch",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID, {
                "Frame ID", "dvb-s2_table.frame_ID.frame_ID.id",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_frame_duration, {
                "Frame duration", "dvb-s2_table.frame_ID.frame_duration",
                FT_UINT32, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_total_timeslot_count, {
                "Frame timeslot count", "dvb-s2_table.frame_ID.total_timeslot_count",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_FRAME_ID_TOT_TIME_COUNT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_start_timeslot_number, {
                "Frame timeslot start number", "dvb-s2_table.frame_ID.start_timeslot_number",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_FRAME_ID_TOT_TIME_COUNT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_timeslot_loop_count, {
                "Frame timeslot loop count", "dvb-s2_table.frame_ID.timeslot_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_timeslot, {
                "Frame timeslot", "dvb-s2_table.frame_ID.timeslot",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_timeslot_timeslot_frequency_offset, {
                "Frame timeslot frequency offset", "dvb-s2_table.frame_ID.timeslot.frequency_offset",
                FT_INT24, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_timeslot_timeslot_time_offset, {
                "Frame timeslot time offset", "dvb-s2_table.frame_ID.timeslot.time_offset",
                FT_UINT32, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_timeslot_timeslot_id, {
                "Frame timeslot id", "dvb-s2_table.frame_ID.timeslot.id",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_frame_ID_timeslot_repeat_count, {
                "Frame timeslot repeat count", "dvb-s2_table.frame_ID.timeslot.repeat_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
/* BCT */
        {&hf_dvb_s2_table_tx_type_loop_count, {
                "Table tx type loop count", "dvb-s2_table.tx_type_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_branch, {
                "Tx type", "dvb-s2_table.tx_type",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type, {
                "Tx type", "dvb-s2_table.tx_type.id",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_tx_content_type, {
                "Tx type tx content type", "dvb-s2_table.tx_type.tx_content_type",
                FT_UINT8, BASE_HEX, VALS(table_txType_contentType), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_tx_format_class, {
                "Tx type tx format class", "dvb-s2_table.tx_type.tx_format_class",
                FT_UINT8, BASE_HEX, VALS(table_frameType_txFormatClass), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_tx_format_data_lenght, {
                "Tx type tx format data length", "dvb-s2_table.tx_type.tx_format_data_lenght",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_tx_format_data, {
                "Tx type format data", "dvb-s2_table.tx_type.format_data",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
/* BCT Common Tx*/
        {&hf_dvb_s2_table_tx_type_tx_block_size, {
                "Tx type tx block size", "dvb-s2_table.tx_type.tx_block_size",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_threshold_es_n0, {
                "Tx type threshold Es/N0", "dvb-s2_table.tx_type.threshold_es_n0",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_payload_size, {
                "Tx type payload size", "dvb-s2_table.tx_type.payload_size",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_modulation_scheme, {
                "Tx type modulation scheme", "dvb-s2_table.tx_type.modulation_scheme",
                FT_UINT8, BASE_HEX, VALS(table_txType_modulationScheme), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_p, {
                "Tx type P", "dvb-s2_table.tx_type.p",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                "Tx type P permutation parameter", HFILL}
        },
        {&hf_dvb_s2_table_tx_type_q0, {
                "Tx type Q0", "dvb-s2_table.tx_type.q0",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_QX_MASK,
                "Tx type Q0 permutation parameter", HFILL}
        },
        {&hf_dvb_s2_table_tx_type_q1, {
                "Tx type Q1", "dvb-s2_table.tx_type.q1",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_QX_MASK,
                "Tx type Q1 permutation parameter", HFILL}
        },
        {&hf_dvb_s2_table_tx_type_q2, {
                "Tx type Q2", "dvb-s2_table.tx_type.q2",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_QX_MASK,
                "Tx type Q2 permutation parameter", HFILL}
        },
        {&hf_dvb_s2_table_tx_type_q3, {
                "Tx type Q3", "dvb-s2_table.tx_type.q3",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_QX_MASK,
                "Tx type Q3 permutation parameter", HFILL}
        },
        {&hf_dvb_s2_table_tx_type_y_period, {
                "Tx type Y period", "dvb-s2_table.tx_type.y_period",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_W_Y_PERIOD_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_w_period, {
                "Tx type W period", "dvb-s2_table.tx_type.w_period",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_W_Y_PERIOD_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_y_pattern, {
                "Y patterns", "dvb-s2_table.tx_type.y_pattern",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_y_pattern_bit, {
                "Y pattern bit", "dvb-s2_table.tx_type.y_pattern_bit",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_W_Y_PATTERN_BIT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_w_pattern, {
                "W patterns", "dvb-s2_table.tx_type.w_pattern",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_w_pattern_bit, {
                "W pattern bit", "dvb-s2_table.tx_type.w_pattern_bit",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_W_Y_PATTERN_BIT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_preamble_len, {
                "Tx type preamble len", "dvb-s2_table.tx_type.preamble_len",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_postamble_len, {
                "Tx type postamble len", "dvb-s2_table.tx_type.postamble_len",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_pilot_period, {
                "Tx type pilot period", "dvb-s2_table.tx_type.pilot_period",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_PILOT_PERIOD_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_pilot_block_len, {
                "Tx type pilot block length", "dvb-s2_table.tx_type.pilot_block_len",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_pilot_sum, {
                "Tx type pilot sum", "dvb-s2_table.tx_type.pilot_sum",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_symbol, {
                "Tx type UW symbols", "dvb-s2_table.tx_type.uw_symbol",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_symbol_unit, {
                "UW symbol unit", "dvb-s2_table.tx_type.uw_symbol_unit",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_symbol_qpsk, {
                "UW symbol QPSK", "dvb-s2_table.tx_type.uw_symbol_qpsk",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_UW_SYMBOL_QPSK_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_symbol_8psk, {
                "UW symbol 8PSK", "dvb-s2_table.tx_type.uw_symbol_8psk",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_UW_SYMBOL_8PSK_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_symbol_16qam, {
                "UW symbol 16QAM", "dvb-s2_table.tx_type.uw_symbol_16qam",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_TX_TYPE_UW_SYMBOL_16QAM_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_waveform_id, {
                "Tx type waveform id", "dvb-s2_table.tx_type.waveform_id",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_tx_start_offset, {
                "Tx type tx start offset 1", "dvb-s2_table.tx_type.tx_start_offset",
                FT_UINT32, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_TX_START_OFFSET_MASK,
                NULL, HFILL}
        },
/* BCT LM Tx*/
        {&hf_dvb_s2_table_tx_type_tx_start_offset_1, {
                "Tx type tx start offset 1", "dvb-s2_table.tx_type.tx_start_offset_1",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_tx_start_offset_2, {
                "Tx type tx start offset 1", "dvb-s2_table.tx_type.tx_start_offset_1",
                FT_UINT24, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_TX_START_OFFSET_MASK,
                NULL , HFILL}
        },
/* BCT CPM Tx */
        {&hf_dvb_s2_table_tx_type_modulation_mh, {
                "Tx type modulation mh", "dvb-s2_table.tx_type.modulation_mh",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_MODULATION_MH_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_modulation_ph, {
                "Tx type modulation ph", "dvb-s2_table.tx_type.modulation_ph",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_MODULATION_PH_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_modulation_type, {
                "Tx type modulation type", "dvb-s2_table.tx_type.modulation_type",
                FT_UINT8, BASE_DEC, VALS(table_txType_modulationType), DVB_S2_TABLE_TX_TYPE_MODULATION_TYPE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_alpha_rc, {
                "Tx type alpha_rc", "dvb-s2_table.tx_type.alpha_rc",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_code_rate, {
                "Tx type code rate", "dvb-s2_table.tx_type.code_rate",
                FT_UINT8, BASE_HEX, VALS(table_txType_codeRate), DVB_S2_TABLE_TX_TYPE_CODE_RATE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_constraint_length_k, {
                "Tx type constraint length K", "dvb-s2_table.tx_type.constraint_length_k",
                FT_UINT8, BASE_HEX, VALS(table_txType_constraintLengthK), DVB_S2_TABLE_TX_TYPE_CONSTRAINT_LENGTH_K_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_length, {
                "Tx type UW length", "dvb-s2_table.tx_type.uw_length",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_nbr_uw_segments, {
                "Tx type number UW segments", "dvb-s2_table.tx_type.nbr_uw_segments",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_segment, {
                "UW segment", "dvb-s2_table.tx_type.uw_segment",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_segment_start, {
                "UW segment start", "dvb-s2_table.tx_type.uw_segment.start",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_uw_segment_length, {
                "UW segment length", "dvb-s2_table.tx_type.uw_segment.length",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_param_interleaver, {
                "Tx type parameterized interleaver", "dvb-s2_table.tx_type.param_interleaver",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_PARAM_INTERLEAVER_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_n, {
                "Tx type N", "dvb-s2_table.tx_type.n",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_N_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_s, {
                "Tx type s", "dvb-s2_table.tx_type.s",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_S_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_p_interleaver, {
                "Tx type p", "dvb-s2_table.tx_type.p",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_P_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_n1_12, {
                "Tx type N1/12", "dvb-s2_table.tx_type.n1_12",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_N1_12_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_k1_12, {
                "Tx type K1/12", "dvb-s2_table.tx_type.k1_12",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_K1_12_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_K2_12, {
                "Tx type K2/12", "dvb-s2_table.tx_type.k2_12",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_K2_12_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_K3_12, {
                "Tx type K3/12", "dvb-s2_table.tx_type.k3_12",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_TX_TYPE_K3_12_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_tx_type_pi_i, {
                "Tx type PI(i)", "dvb-s2_table.tx_type.pi_i",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
/* SPT */
        {&hf_dvb_s2_table_satellite_loop_count, {
                "Table satellite loop count", "dvb-s2_table.satellite_loop_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_satellite, {
                "Satellite", "dvb-s2_table.satellite",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_satellite_id, {
                "Satellite id", "dvb-s2_table.satellite.id",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_satellite_x_coordinate, {
                "Satellite X coordinate", "dvb-s2_table.satellite.x_coordinate",
                FT_FLOAT, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_satellite_y_coordinate, {
                "Satellite Y coordinate", "dvb-s2_table.satellite.y_coordinate",
                FT_FLOAT, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_satellite_z_coordinate, {
                "Satellite Z coordinate", "dvb-s2_table.satellite.z_coordinate",
                FT_FLOAT, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
/* NIT - RMT */
        {&hf_dvb_s2_table_network_descriptors_length, {
                "Table network descriptors length", "dvb-s2_table.network_descriptors_length",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_NETWORK_DESC_LENGTH_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_multiplex_streams_spec_length, {
                "Table multiplex streams spec length", "dvb-s2_table.multiplex_streams_spec_length",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_MULTIPLEX_STREAMS_LENGTH_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_multiplex, {
                "Multiplex stream", "dvb-s2_table.multiplex",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_multiplex_forward_multiplex, {
                "Multiplex stream forward multiplex", "dvb-s2_table.multiplex.forward_multiplex",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_multiplex_reward_multiplex, {
                "Multiplex stream reward multiplex", "dvb-s2_table.multiplex.reward_multiplex",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_multiplex_original_network_id, {
                "Multiplex stream original network id", "dvb-s2_table.multiplex.original_network_id",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_multiplex_transport_descriptors_length, {
                "Multiplex stream transport descriptors length", "dvb-s2_table.multiplex.transport_descriptors_length",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_MULTIPLEX_TRANSPORT_DESC_LENGTH_MASK,
                NULL, HFILL}
        },
/* MMT2 */
        {&hf_dvb_s2_table_svn_number, {
                "Table svn number", "dvb-s2_table.svn_number",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_svn_prefix_size, {
                "Table svn prefix size", "dvb-s2_table.svn_prefix_size",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_SVN_PREFIX_SIZE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_count, {
                "Table pt count", "dvb-s2_table.pt_count",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_protocol, {
                "Protocol", "dvb-s2_table.protocol",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_protocol_type, {
                "Protocol type", "dvb-s2_table.pt.protocol_type",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_address_size, {
                "Protocol address size", "dvb-s2_table.pt.address_size",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_mapping_sections, {
                "Protocol mapping sections", "dvb-s2_table.pt.mapping_sections",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_mapping_section, {
                "Mapping section", "dvb-s2_table.pt.mapping_sections",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_ms_inclusion_start, {
                "Mapping section inclusion start", "dvb-s2_table.pt.ms.inclusion_start",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_ms_inclusion_end, {
                "Mapping section inclusion end", "dvb-s2_table.pt.ms.inclusion_start",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_ms_exclusions, {
                "Mapping section exclusions", "dvb-s2_table.pt.ms.exclusions",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_ms_exclusion, {
                "Exclusion", "dvb-s2_table.pt.ms.exclusion",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_ms_exclusion_start, {
                "Exclusion start", "dvb-s2_table.pt.ms.exclusion.start",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_ms_exclusion_end, {
                "Exclusion end", "dvb-s2_table.pt.ms.exclusion.end",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_ms_mac24_base, {
                "Mapping section mac24 base", "dvb-s2_table.pt.ms.mac24_base",
                FT_UINT24, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_pt_ms_mcast_prefix_size, {
                "Mapping section multicast prefix size", "dvb-s2_table.pt.ms.mcast_prefix_size",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_PT_MS_MCAST_PREFIX_SIZE_MASK,
                NULL, HFILL}
        },

/* Descriptors */
        {&hf_dvb_s2_table_descriptor, {
                "Descriptor", "dvb-s2_table.descriptor",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_tag, {
                "Descriptor tag", "dvb-s2_table.desc.tag",
                FT_UINT8, BASE_HEX, VALS(table_desc_type), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_length, {
                "Descriptor length", "dvb-s2_table.desc.length",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
/* Control assign descriptor */
        {&hf_dvb_s2_table_desc_sync_achieved_time_threshold, {
                "Descriptor sync achieved time threshold", "dvb-s2_table.desc.sync_achieved_time_threshold",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_max_sync_tries, {
                "Descriptor max sync tries", "dvb-s2_table.desc.max_sync_tries",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_sync_achieved_freq_threshold, {
                "Descriptor sync achieved frequency threshold", "dvb-s2_table.desc.sync_achieved_freq_threshold",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_ctrl_start_superframe_count, {
                "Descriptor control start superframe count", "dvb-s2_table.desc.control_start_superframe_count",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_ctrl_frame_nbr, {
                "Descriptor control frame number", "dvb-s2_table.desc.control_frame_number",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_ctrl_repeat_period, {
                "Descriptor control repeat period", "dvb-s2_table.desc.control_repeat_period",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_ctrl_timeslot_nbr, {
                "Descriptor control timeslot number", "dvb-s2_table.desc.control_timeslot_number",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_DESC_CTRL_TIMESLOT_NBR_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_sync_start_superframe, {
                "Descriptor SYNC start superframe", "dvb-s2_table.desc.sync_start_superframe",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_sync_frame_nbr, {
                "Descriptor SYNC frame number", "dvb-s2_table.desc.sync_frame_number",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_DESC_SYNC_FRAME_NBR_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_sync_repeat_period, {
                "Descriptor SYNC repeat period", "dvb-s2_table.desc.sync_repeat_period",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_sync_slot_nbr, {
                "Descriptor SYNC timeslot number", "dvb-s2_table.desc.sync_timeslot_number",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_TABLE_DESC_CTRL_TIMESLOT_NBR_MASK,
                NULL, HFILL}
        },
/* Correction message descriptor */
        {&hf_dvb_s2_table_desc_time_correct_flag, {
                "Descriptor time correction flag", "dvb-s2_table.desc.time_correct_flag",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_TIME_CORRECT_FLAG_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_power_correct_flag, {
                "Descriptor power correction flag", "dvb-s2_table.desc.power_correct_flag",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_POWER_CORRECT_FLAG_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_freq_correct_flag, {
                "Descriptor frequency correction flag", "dvb-s2_table.desc.freq_correct_flag",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_FREQ_CORRECT_FLAG_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_slot_type, {
                "Descriptor slot type", "dvb-s2_table.desc.slot_type",
                FT_UINT8, BASE_HEX, VALS(table_timeslotContent), DVB_S2_TABLE_DESC_SLOT_TYPE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_burst_time_scaling, {
                "Descriptor burst time scaling", "dvb-s2_table.desc.burst_time_scaling",
                FT_UINT8, BASE_DEC, NULL, DVB_S2_TABLE_DESC_BURST_TIME_SCALING_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_burst_time_correct, {
                "Descriptor burst time correction", "dvb-s2_table.desc.burst_time_correct",
                FT_INT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_power_ctrl_flag, {
                "Descriptor power control flag", "dvb-s2_table.desc.power_ctrl_flag",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_POWER_CTRL_FLAG_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_power_correction, {
                "Descriptor power correction", "dvb-s2_table.desc.power_correction",
                FT_INT8, BASE_DEC, NULL, DVB_S2_TABLE_DESC_POWER_CORRECTION_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_power_esn0, {
                "Descriptor EsN0", "dvb-s2_table.desc.esn0",
                FT_INT8, BASE_DEC, NULL, DVB_S2_TABLE_DESC_POWER_CORRECTION_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_freq_correction, {
                "Descriptor frequency correction", "dvb-s2_table.desc.freq_correction",
                FT_INT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
/* Correction message extension descriptor */
        {&hf_dvb_s2_table_desc_slot_nbr, {
                "Descriptor slot number", "dvb-s2_table.desc.slot_number",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_sf_sequence, {
                "Superframe sequence", "dvb-s2_table.desc.cmed.sf.sequence",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_frame_number, {
                "Frame number", "dvb-s2_table.desc.cmed.frame.number",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
/* Logon response descriptor */
        {&hf_dvb_s2_table_desc_keep_id_after_logoff, {
                "Descriptor keep identifiers after logoff", "dvb-s2_table.desc.keep_id_after_logoff",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_KEEP_ID_AFTER_LOGOFF_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_power_ctrl_mode, {
                "Descriptor power control mode", "dvb-s2_table.desc.power_ctrl_mode",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_POWER_CTRLMODE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_rcst_access_status, {
                "Descriptor RCST access status", "dvb-s2_table.desc.rcst_access_status",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_RCST_ACCESS_STATUS_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_logon_id, {
                "Descriptor logon id", "dvb-s2_table.desc.logon_id",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_lowest_assign_id, {
                "Descriptor lowest_assignmant_id", "dvb-s2_table.desc.lowest_assign_id",
                FT_UINT24, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_assign_id_count, {
                "Descriptor assignment id count", "dvb-s2_table.desc.assign_id_count",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_ASSIGN_ID_COUNT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_unicast_mac24_count, {
                "Descriptor unicast_mac24_count", "dvb-s2_table.desc.unicast_mac24_count",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_UNICAST_MAC24_COUNT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_mac24, {
                "MAC24", "dvb-s2_table.desc.mac24",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_mac24_prefix_size, {
                "MAC24 prefix size", "dvb-s2_table.desc.mac24.prefix_size",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_MAC24_PREFIX_SIZE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_mac24_unicast, {
                "MAC24 unicast", "dvb-s2_table.mac24.unicast",
                FT_UINT24, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_mac24_mcast_mapping_method, {
                "MAC24 mcast mapping method", "dvb-s2_table.mac24.mcast_mapping_method",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_MAC24_MAPPING_METHOD_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_mac24_mcast_ip_version_ind_pres, {
                "MAC24 mcast ip version indicator presence", "dvb-s2_table.mac24.mcast_ip_version_ind_pres",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_MAC24_MCAST_IP_VERSION_IND_PRES_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_mac24_mcast_synthesis_field_size, {
                "MAC24 mcast synthesis field size", "dvb-s2_table.mac24.mcast_synthesis_field_size",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_MAC24_MCAST_SYNTHESIS_FIELD_SIZE_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_default_svn_number, {
                "Descriptor default svn number", "dvb-s2_table.desc.default_svn_number",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_reserved, {
                "Descriptor reserved", "dvb-s2_table.desc.reserved",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
/* Lower layer service descriptor */
        {&hf_dvb_s2_table_desc_default_ctrl_random_interval, {
                "Descriptor default control randomization interval", "dvb-s2_table.desc.default_ctrl_random_interval",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_dynamic_rate_persistence, {
                "Descriptor dynamic rate persistence", "dvb-s2_table.desc.dynamic_rate_persistence",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_volume_backlog_persistence, {
                "Descriptor volume backlog persistence", "dvb-s2_table.desc.volume_backlog_persistence",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_lls_count, {
                "Descriptor lower layer service count", "dvb-s2_table.desc.lower_layer_service_count",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_LLS_COUNT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_rc_count, {
                "Descriptor rc count", "dvb-s2_table.desc.rc_count",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_RC_COUNT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_desc_ra_ac_count, {
                "Descriptor ra_ac count", "dvb-s2_table.desc.ra_ac_count",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_DESC_RA_AC_COUNT_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls, {
                "lower layer service", "dvb-s2_table.lls.index",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_index, {
                "lls index", "dvb-s2_table.lls.index",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_LLS_INDEX_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_random_access, {
                "lls random access", "dvb-s2_table.lls.random_access",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_LLS_RANDOM_ACCESS_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_dedicated_access, {
                "lls dedicated access", "dvb-s2_table.lls.dedicated_access",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_LLS_DEDICATED_ACCESS_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_nominal_rc_index, {
                "lls nominal rc index", "dvb-s2_table.lls.nominal_rc_index",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_LLS_NOMINAL_RC_INDEX_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_nominal_da_ac_index, {
                "lls nominal da_ac index", "dvb-s2_table.lls.nominal_da_ac_index",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_LLS_NOMINAL_DA_AC_INDEX_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_conditional_demand_rc_map, {
                "lls conditional demand rc map", "dvb-s2_table.lls.conditional_demand_rc_map",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_conditional_scheduler_da_ac_map, {
                "lls conditional scheduler da ac map", "dvb-s2_table.lls.conditional_scheduler_da_ac_map",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_nominal_ra_ac_index, {
                "lls nominal ra_ac index", "dvb-s2_table.lls.nominal_ra_ac_index",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_LLS_NOMINAL_RA_AC_INDEX_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_lls_conditional_scheduler_ra_ac_map, {
                "lls nominal ra_ac map", "dvb-s2_table.lls.nominal_ra_ac_map",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc, {
                "request class", "dvb-s2_table.rc",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc_index, {
                "rc index", "dvb-s2_table.rc.index",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_RC_INDEX_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc_constant_assignment_provided, {
                "rc constant assignment provided", "dvb-s2_table.rc.constant_assignment_provided",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_RC_CONSTANT_ASSIGN_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc_volume_allowed, {
                "rc volume allowed", "dvb-s2_table.rc.volume_allowed",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_RC_VOLUME_ALLOWED_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc_rbdc_allowed, {
                "rc rbdc allowed", "dvb-s2_table.rc.rbdc_allowed",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_RC_RBDC_ALLOWED_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc_maximum_service_rate, {
                "rc maximum service rate", "dvb-s2_table.rc.maximum_service_rate",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc_minimum_service_rate, {
                "rc minimum service rate", "dvb-s2_table.rc.minimum_service_rate",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc_constant_service_rate, {
                "rc minimum constant rate", "dvb-s2_table.rc.constant_service_rate",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_rc_maximum_backlog, {
                "rc maximum_backlog", "dvb-s2_table.rc.maximum_backlog",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_ra_ac, {
                "random access allocation channel", "dvb-s2_table.ra_ac",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_ra_ac_index, {
                "ra_ac index", "dvb-s2_table.ra_ac.index",
                FT_UINT8, BASE_HEX, NULL, DVB_S2_TABLE_RA_AC_INDEX_MASK,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_ra_ac_max_unique_payload_per_block, {
                "ra_ac max unique payload per block", "dvb-s2_table.ra_ac.max_unique_payload_per_block",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_ra_ac_max_consecutive_block_accessed, {
                "ra_ac max consecutive block accessed", "dvb-s2_table.ra_ac.max_consecutive_block_accessed",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_ra_ac_min_idle_block, {
                "ra_ac min idle block", "dvb-s2_table.ra_ac.min_idle_block",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_ra_ac_defaults_field_size, {
                "ra_ac defaults field size", "dvb-s2_table.ra_ac.default_field_size",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_ra_ac_defaults_for_ra_load_control, {
                "ra_ac defaults for ra load control", "dvb-s2_table.ra_ac.defaults_for_ra_load_control",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_table_crc32, {
                "Table crc32", "dvb-s2_table.crc32",
                FT_UINT32, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        }
    };

    static gint *ett_table[] = {
        &ett_dvb_s2_hdr_table,
        &ett_dvb_s2_hdr_table_sf,
        &ett_dvb_s2_hdr_table_sf_frame,
        &ett_dvb_s2_hdr_table_desc,
        &ett_dvb_s2_hdr_table_frame,
        &ett_dvb_s2_hdr_table_frame_assign,
        &ett_dvb_s2_hdr_table_entry,
        &ett_dvb_s2_hdr_table_frametype,
        &ett_dvb_s2_hdr_table_frametype_section,
        &ett_dvb_s2_hdr_table_frame_ID,
        &ett_dvb_s2_hdr_table_frame_ID_timeslot,
        &ett_dvb_s2_hdr_table_mac24,
        &ett_dvb_s2_hdr_table_lls,
        &ett_dvb_s2_hdr_table_rc,
        &ett_dvb_s2_hdr_table_raac,
        &ett_dvb_s2_hdr_table_txmode,
        &ett_dvb_s2_hdr_table_txtype,
        &ett_dvb_s2_hdr_table_txtype_uwsegment,
        &ett_dvb_s2_hdr_table_txtype_ypattern,
        &ett_dvb_s2_hdr_table_txtype_wpattern,
        &ett_dvb_s2_hdr_table_txtype_uwsymbol,
        &ett_dvb_s2_hdr_table_satellite,
        &ett_dvb_s2_hdr_table_multiplex,
        &ett_dvb_s2_hdr_table_pt,
        &ett_dvb_s2_hdr_table_pt_ms,
        &ett_dvb_s2_hdr_table_pt_ms_exclusion,
    };

    static const enum_val_t rcs_version[] = {
        { "RCS",  "RCS protocol" , DVB_S2_RCS_TABLE_DECODING },
        { "RCS2", "RCS2 protocol", DVB_S2_RCS2_TABLE_DECODING },
        { NULL, NULL, 0 }
    };

    proto_dvb_s2_table = proto_register_protocol("DVB-S2 Signalization Table", "DVB-S2-TABLE", "dvb-s2_table");
    register_dissector("dvb-s2_table", dissect_dvb_s2_table, proto_dvb_s2_table);

    proto_register_field_array(proto_dvb_s2_table, hf_table, array_length(hf_table));
    proto_register_subtree_array(ett_table, array_length(ett_table));

    dvb_s2_table_module = prefs_register_protocol(proto_dvb_s2_table, proto_reg_handoff_dvb_s2_table);

    prefs_register_enum_preference(dvb_s2_table_module, "rcs_protocol",
                                   "defines RCS protocol version",
                                   "defines the RCS protocol version used in table dissection",
                                   &dvb_s2_rcs_version,
                                   rcs_version, FALSE);
}

void proto_reg_handoff_dvb_s2_table(void)
{

}

/*
 * Editor modelines  -  http://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
