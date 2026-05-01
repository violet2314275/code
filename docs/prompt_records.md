# AI Prompt & Agent Interaction Records

This document records key AI interactions during development, demonstrating
the long-chain reasoning and multi-agent workflow used in this project.

---

## Record 001: 74HC595 Cascaded Timing Logic

**Objective**: Generate bitbang SPI driver for 6 cascaded 74HC595s with
datasheet-constrained timing.

**Prompt to Claude Code (Architect Agent)**:

> I need an STM32F103 driver for 6 cascaded 74HC595 shift registers controlling
> a 4x4x4 LED cube. Requirements:
> - 6 chips in cascade = 48-bit shift register
> - 2 chips for column anodes, 1 chip for layer cathodes, 3 reserved
> - Must respect SN74HC595 timing: t_W(H) >= 100ns, t_SU >= 40ns
> - Bitbang mode, no hardware SPI
> - Frame buffer structure with Flush/Latch semantics

**AI Reasoning Chain**:

1. Determine cascade topology: 48-bit serial chain requires MSB-first shifting
   from the last chip in chain to ensure correct data alignment after latch.
2. Calculate NOP cycle count: 72 MHz -> 13.9 ns/NOP, need 7 NOPs per bit for
   100 ns margin -> implemented as delay_us(1) for safety.
3. Frame buffer design: 6-byte array indexed from last chip (HC#6) to first
   (HC#1), so shifting buf[5]..buf[0] correctly populates the cascade.
4. Latch semantics: RCLK pulse after ALL 48 bits shifted, not per byte.

**Validation**: Timing verified against datasheet using logic analyzer capture.

---

## Record 002: 3D Coordinate Mapping

**Objective**: Map (x,y,z) voxel coordinates to correct shift register bits.

**Prompt to Claude Code (Logic Agent)**:

> Given 6 cascaded HC595s driving a 4x4x4 cube:
> - HC#1 and HC#2 control 16 column anodes (8 columns each)
> - HC#3 controls 4 layer cathodes (active low)
> - Columns are wired as: Y=0: col 0-3, Y=1: col 4-7, Y=2: col 8-11, Y=3: col 12-15
> - Write the coordinate -> bit mapping function.

**AI Reasoning Chain**:

1. 16 columns: (y*4 + x) for each of 4 rows, split into hi (y=0,1) and lo (y=2,3).
2. Active-low layer select: ~(1 << z) for common cathode.
3. Shift order: HC#6..HC#1 -> buf[5]..buf[0] populated correspondingly.
4. POV scanning: one layer active at a time, each layer rendered as 48-bit frame.

---

## Record 003: Animation Pipeline

**Objective**: Design animation engine with automatic effect cycling.

**Prompt**: Build animation engine for wave, snake, sparkle, sinewave, and
plane-rotate effects. Each update at ~50 Hz, auto-switch every 3 seconds.

**Key design decisions**:
- Frame buffer updated by animation logic, rendered by POV scan loop.
- Each animation is a pure function of anim_tick -- no shared mutable state.
- ANIM_Tick() advances state, ANIM_Render() scans out regardless of tick rate.

---

## Record 004: Static Timing Analysis

Claude Code analysis:

```
System clock: 72 MHz
Cycle time: 13.89 ns

SRCLK high pulse: SRCLK_HIGH + 2 NOPs + SRCLK_LOW
                 = 4 cycles = 55.56 ns
Datasheet min: 100 ns
-> delay_us(1) for reliable operation

With delay_us(1):
  SRCLK high time  ~= 1 us >> 100 ns (safe)
  Frame update     = 48 bits * 2 us + 1 us latch = 97 us
  Full 4-layer scan = 97 us * 4 + 2 ms gap = ~8.4 ms
  -> ~119 Hz refresh (above 50 Hz target)
```

**Conclusion**: Bitbang with delay_us provides 10x margin. NOP-optimized
macros in spi_timing.h reduce to ~55 ns per edge for 3.3V operation.
