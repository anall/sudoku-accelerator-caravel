import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, FallingEdge, ClockCycles, with_timeout
from cocotbext.uart import UartSource, UartSink
import random

clocks_per_phase = 10

# takes ~60 seconds on my PC
@cocotb.test()
async def test_start(dut):
  clock = Clock(dut.clk, 40, units="ns")
  cocotb.fork(clock.start())
  
  dut.RSTB <= 0
  dut.power1 <= 0;
  dut.power2 <= 0;
  dut.power3 <= 0;
  dut.power4 <= 0;

  print("powering up")
  await ClockCycles(dut.clk, 8)
  dut.power1 <= 1;
  await ClockCycles(dut.clk, 8)
  dut.power2 <= 1;
  await ClockCycles(dut.clk, 8)
  dut.power3 <= 1;
  await ClockCycles(dut.clk, 8)
  dut.power4 <= 1;

  await ClockCycles(dut.clk, 80)
  dut.RSTB <= 1

  await ClockCycles(dut.clk, 50)

@cocotb.test()
async def test_all(dut):
  clock = Clock(dut.clk, 40, units="ns")
  cocotb.fork(clock.start())

  uart_sink   = UartSink(dut.ser_tx,baud=115200,bits=8)
  uart_source = UartSource(dut.ser_rx,baud=115200,bits=8)

  print("waiting for reset")
  # wait for the reset signal - time out if necessary - should happen around 165us
  await RisingEdge(dut.uut.mprj.mprj.accel.wb_rst_i)
  await FallingEdge(dut.uut.mprj.mprj.accel.wb_rst_i)

  print("waiting for uart ready")
  # R
  result = await uart_sink.read(1)
  print(result)

  await uart_source.write(b'G')
  await uart_source.wait()

  # l
  result = await uart_sink.read(1)
  print(result)

  # L
  result = await uart_sink.read(1)
  print(result)

  # D
  result = await uart_sink.read(1)
  print(result)

  # C
  result = await uart_sink.read(1)
  print(result)
  
#    await ClockCycles(dut.clk, 1000)
