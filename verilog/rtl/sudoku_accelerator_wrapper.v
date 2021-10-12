/* SPDX-FileCopyrightText: 2021 Andrea Nall

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   SPDX-License-Identifier: Apache-2.0
*/
`default_nettype none
module sudoku_accelerator_wrapper (
`ifdef USE_POWER_PINS
  inout vccd1,	// User area 1 1.8V supply
  inout vssd1,	// User area 1 digital ground
`endif

  input wire wb_clk_i,
  input wire wb_rst_i,

  input [31:0]  wb_adr_i,
  input [31:0]  wb_dat_i,
  input [3:0]   wb_sel_i,
  input         wb_we_i,
  input         wb_cyc_i,
  input         wb_stb_i,

  output        wb_ack_o,
  output [31:0] wb_dat_o,

  // Logic Analyzer Signals
  input  [127:0] la_data_in,
  output [127:0] la_data_out,
  input  [127:0] la_oenb,

  // IOs
  input  [`MPRJ_IO_PADS-1:0] io_in,
  output [`MPRJ_IO_PADS-1:0] io_out,
  output [`MPRJ_IO_PADS-1:0] io_oeb,

  // Independent clock (on independent integer divider)
  input   user_clock2,

  // User maskable interrupt signals
  output [2:0] user_irq
);

wire uart_en;
assign io_oeb[15] = ~uart_en;
assign la_data_out[0] = uart_en;

wire int_sudoku;
wire int_uart;

assign user_irq[0] = int_sudoku;
assign user_irq[1] = int_uart;

assign la_data_out[32] = int_sudoku;
assign la_data_out[33] = int_uart;

assign la_oenb[33:32] = 0;

// expose some la pins to GPIO, both ways
assign io_out[25:17] = la_data_in[104:96];
assign io_oeb[25:17] = 0;
assign la_data_out[113:105] = io_in[34:28];

sudoku_accelerator accel (
    .wb_clk_i(wb_clk_i),
    .wb_rst_i(la_data_in[0]),

    // MGMT SoC Wishbone Slave
    .wb_adr_i(wb_adr_i),
    .wb_dat_i(wb_dat_i),
    .wb_sel_i(wb_sel_i),
    .wb_we_i(wb_we_i),
    .wb_cyc_i(wb_cyc_i),
    .wb_stb_i(wb_stb_i),

    .wb_ack_o(wb_ack_o),
    .wb_dat_o(wb_dat_o),

    .uart_enabled(uart_en),
    .ser_tx(io_out[15]),
    .ser_rx(io_in[16]),

    .interrupt_sudoku(int_sudoku),
    .interrupt_uart(int_uart)
);

endmodule
