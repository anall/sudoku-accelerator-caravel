# SPDX-FileCopyrightText: 2020 Efabless Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# SPDX-License-Identifier: Apache-2.0

set script_dir [file dirname [file normalize [info script]]]

#set :env(GLB_RESIZER_TIMING_OPTIMIZATIONS) 0
set ::env(DESIGN_NAME) sudoku_accelerator_wrapper

set ::env(BASE_SDC_FILE) "$script_dir/base.sdc"

set VERILOG_FILES_I [ glob $script_dir/../../verilog/rtl/sudoku_accelerator/src/*.v ]
set ::env(VERILOG_FILES) "\
	$::env(CARAVEL_ROOT)/verilog/rtl/defines.v \
  $script_dir/../../verilog/rtl/sudoku_accelerator_wrapper.v \
  $VERILOG_FILES_I"

set ::env(CLOCK_PORT) "wb_clk_i"
#set ::env(CLOCK_NET) "counter.clk"
set ::env(CLOCK_PERIOD) "40"
set ::env(SYNTH_STRATEGY) "DELAY 2"

#set ::env(PL_RESIZER_ALLOW_SETUP_VIOS) 1
#set ::env(GLB_RESIZER_ALLOW_SETUP_VIOS) 1

#set ::env(FP_SIZING) absolute
#set ::env(DIE_AREA) "0 0 900 600"

set ::env(FP_PIN_ORDER_CFG) $script_dir/pin_order.cfg

set ::env(FP_CORE_UTIL) 45

set ::env(PL_RESIZER_HOLD_SLACK_MARGIN) .3
set ::env(GLB_RESIZER_HOLD_SLACK_MARGIN) .3

# Maximum layer used for routing is metal 4.
# This is because this macro will be inserted in a top level (user_project_wrapper) 
# where the PDN is planned on metal 5. So, to avoid having shorts between routes
# in this macro and the top level metal 5 stripes, we have to restrict routes to metal4.  
set ::env(DESIGN_IS_CORE) 0
set ::env(FP_PDN_CORE_RING) 0
set ::env(GLB_RT_MAXLAYER) 5

#set ::env(GLB_RT_ADJUSTMENT) 0.10
set ::env(CELL_PAD) 2

# You can draw more power domains if you need to 
set ::env(VDD_NETS) [list {vccd1}]
set ::env(GND_NETS) [list {vssd1}]

set ::env(DIODE_INSERTION_STRATEGY) 3
# If you're going to use multiple power domains, then disable cvc run.
#set ::env(RUN_CVC) 0
