/*
 * SPDX-FileCopyrightText: 2021 Andrea Nall
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "verilog/dv/caravel/defs.h"
#define UART_BASE (size_t)0x30800000

bool uart_seen;
bool puzzle_done;

uint32_t *irq() {
  if ( ! uart_seen ) {
    volatile uint32_t *UART_DATA    = (uint32_t*)(UART_BASE+0x4);
    uint32_t value = (*UART_DATA);

    if ( value == 'G' ) {
      uart_seen = true;
    }
  } else {
    reg_mprj_irq = 0b0000; // need to shut off the interrupt
    puzzle_done = true; // hopefully?
  }
}

void main() {
  volatile uint32_t *UART_CLK_DIV = (uint32_t*)UART_BASE;
  volatile uint32_t *UART_DATA    = (uint32_t*)(UART_BASE+0x4);
  volatile uint32_t *UART_CONFIG  = (uint32_t*)(UART_BASE+0x8);
  volatile uint32_t *PUZZLE_CTRL  = (uint32_t*)((size_t)0x30000000);
  volatile uint32_t *PUZZLE_IE    = (uint32_t*)((size_t)0x30000008);

  const uint32_t UART_ENABLE = 0b1;
  const uint32_t UART_IE_HAS_DATA = 0b10;
  const uint32_t UART_FIFO_HAS_DATA = 0x100;

  const uint32_t CLOCK_FREQ = 25000000;
  const uint32_t BAUD = 115200;

  // enable UART pins
	reg_mprj_io_15 =   GPIO_MODE_USER_STD_OUTPUT;
	reg_mprj_io_16 =   GPIO_MODE_USER_STD_INPUT_NOPULL;

  // Apply configuration
  reg_mprj_xfer = 1;
  while (reg_mprj_xfer == 1);

  // reset design with 0bit of 1st bank of LA
  reg_la0_oenb = 0;
  reg_la0_iena = 0;
  reg_la0_data = 1;
  reg_la0_data = 0;

  // enable UART
  *UART_CLK_DIV = (CLOCK_FREQ/BAUD);
  *UART_CONFIG  = UART_ENABLE | UART_IE_HAS_DATA;

  reg_mprj_irq = 0b0010;

  // send ready signal
  *UART_DATA = (uint32_t)'R';

  uart_seen = false;
  while ( ! uart_seen ) {}
  *UART_CONFIG  = UART_ENABLE;
  reg_mprj_irq = 0b0000;

  *UART_DATA = (uint32_t)'l';

  uint32_t tmp = 0;
  uint32_t tct = 0;
  uint32_t row = 0;
  uint32_t sub = 0;
  
  (*(uint32_t*)((size_t)0x30001200)) = 0x010005;
  (*(uint32_t*)((size_t)0x30001204)) = 0x000600;
  (*(uint32_t*)((size_t)0x30001208)) = 0x040200;
  (*(uint32_t*)((size_t)0x30001210)) = 0x000600;
  (*(uint32_t*)((size_t)0x30001214)) = 0x000004;
  (*(uint32_t*)((size_t)0x30001218)) = 0x030700;
  (*(uint32_t*)((size_t)0x30001220)) = 0x000700;
  (*(uint32_t*)((size_t)0x30001224)) = 0x000000;
  (*(uint32_t*)((size_t)0x30001228)) = 0x050001;
  (*(uint32_t*)((size_t)0x30001230)) = 0x000000;
  (*(uint32_t*)((size_t)0x30001234)) = 0x070000;
  (*(uint32_t*)((size_t)0x30001238)) = 0x080002;
  (*(uint32_t*)((size_t)0x30001240)) = 0x020008;
  (*(uint32_t*)((size_t)0x30001244)) = 0x000903;
  (*(uint32_t*)((size_t)0x30001248)) = 0x070405;
  (*(uint32_t*)((size_t)0x30001250)) = 0x000003;
  (*(uint32_t*)((size_t)0x30001254)) = 0x040802;
  (*(uint32_t*)((size_t)0x30001258)) = 0x000900;
  (*(uint32_t*)((size_t)0x30001260)) = 0x050000;
  (*(uint32_t*)((size_t)0x30001264)) = 0x000006;
  (*(uint32_t*)((size_t)0x30001268)) = 0x000004;
  (*(uint32_t*)((size_t)0x30001270)) = 0x000200;
  (*(uint32_t*)((size_t)0x30001274)) = 0x000000;
  (*(uint32_t*)((size_t)0x30001278)) = 0x000103;
  (*(uint32_t*)((size_t)0x30001280)) = 0x060409;
  (*(uint32_t*)((size_t)0x30001284)) = 0x010000;
  (*(uint32_t*)((size_t)0x30001288)) = 0x000007;

  /*for ( uint32_t i = 0; i < 81; ++i ) {
    char ch = puzzle[i];
    if ( ch >= '1' && ch <= '9' ) {
      tmp = ((ch-'0')<<24|tmp)>>8;
    } else {
      tmp = tmp>>8;
    }
    if ( ++tct == 3 ) {
      (*(volatile uint32_t*)((size_t)0x30001200|row<<4|sub<<2)) = tmp;
      tct = 0;
      tmp = 0;
      if ( ++sub == 3 ) {
        sub = 0;
        ++row;
      }
    }
  }*/

  *UART_DATA = (uint32_t)'L';

  puzzle_done = false;
  
  *PUZZLE_CTRL = 0b1;
  *PUZZLE_IE = 0b1;

  reg_mprj_irq = 0b0001;

  while ( ! puzzle_done ) {}
  *PUZZLE_IE = 0b0; 
  
  if ( ((*PUZZLE_CTRL) & 0b1111) == 0b0100 ) {
    *UART_DATA = (uint32_t)'D';
  } else {
    *UART_DATA = (uint32_t)'?';
    *UART_DATA = *PUZZLE_CTRL;
    while (1);
  }

  int result = 1;
  result = (*(uint32_t*)((size_t)0x30001200)) == 0x010805 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001204)) == 0x030607 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001208)) == 0x040209 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001210)) == 0x090602 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001214)) == 0x050104 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001218)) == 0x030708 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001220)) == 0x030704 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001224)) == 0x080209 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001228)) == 0x050601 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001230)) == 0x040906 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001234)) == 0x070501 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001238)) == 0x080302 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001240)) == 0x020108 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001244)) == 0x060903 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001248)) == 0x070405 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001250)) == 0x070503 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001254)) == 0x040802 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001258)) == 0x010906 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001260)) == 0x050301 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001264)) == 0x020706 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001268)) == 0x090804 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001270)) == 0x080207 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001274)) == 0x090405 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001278)) == 0x060103 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001280)) == 0x060409 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001284)) == 0x010308 ? result : 0;
  result = (*(uint32_t*)((size_t)0x30001288)) == 0x020507 ? result : 0;

  if ( result ) {
    *UART_DATA = (uint32_t)'C';
  } else {
    *UART_DATA = (uint32_t)'I';
  }
  
  //while ( ct < 81 ) {
  //while ( (*UART_CONFIG & UART_FIFO_HAS_DATA) != UART_FIFO_HAS_DATA );
  //}
}
