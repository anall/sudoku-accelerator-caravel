[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)


This project contains a two sudoku accelerator modules on a wishbone bus.

Each solver module is capable of performing both a simple 'only candidate' method, as well as finding row-only 'naked singles'.

A single 'only candidate' pass on the whole puzzle completes in 23 cycles, and a 'naked singles' pass completes in 108 cycles.

The solver will exit early during a run if the puzzle becomes solved, or if a single cell ends up with no remaining valid values (do note that this doesn't mean the solver can detect all illegal puzzles).

This design also contains a modified FIFO-enabled version of simpleuart (from caravel) to make external communication easier.
