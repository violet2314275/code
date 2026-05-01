# Hardware Schematic: 4x4x4 LED Cube with 6x74HC595

## System Overview

```
                    +-------------+
                    |   STM32F103 |
                    |  (Cortex-M3)|
                    |  72 MHz     |
                    +------+------+
                           | GPIOB[0:4]
                           |
             +-------------+----------------------+
             |             |                      |
        +----+----+  +----+----+            +----+----+
        | HC #1   |  | HC #2   |   ...      | HC #6   |
        |(col_hi) |  |(col_lo) |            |(reserve)|
        | Q0-Q7   |  | Q0-Q7   |            | Q0-Q7   |
        +----+----+  +----+----+            +---------+
             |            |
        +----+------------+----+
        |  4x4x4 LED Matrix   |
        |  64 LEDs, common    |
        |  cathode layer scan |
        +---------------------+
```

## Pin Assignment

| STM32 Pin | Signal      | Function                    |
|-----------|-------------|-----------------------------|
| PB0       | HC595_SER   | Serial data input           |
| PB1       | HC595_SRCLK | Shift register clock        |
| PB2       | HC595_RCLK  | Storage (latch) clock       |
| PB3       | HC595_OE    | Output enable (active low)  |
| PB4       | HC595_CLR   | Master clear (active low)   |

## Cascaded Configuration

- 6 chips in series: Q7' of HC#n feeds SER of HC#n+1
- Total outputs: 48
- HC#1: Column anode high (columns 0-7)
- HC#2: Column anode low (columns 8-15)
- HC#3: Layer cathode select (layers 0-3)
- HC#4-6: Reserved / expansion

## Timing Constraints (SN74HC595)

| Parameter | Description          | Min    |
|-----------|----------------------|--------|
| t_W(H)    | SRCLK pulse width H  | 100 ns |
| t_W(L)    | SRCLK pulse width L  | 100 ns |
| t_SU      | SER setup time       | 40 ns  |
| t_H       | SER hold time        | 20 ns  |
| t_PH      | RCLK pulse width     | 100 ns |

## Layer Scanning (POV)

Layers scanned at ~5 ms each -> 20 ms full frame -> 50 Hz refresh.
Persistence of vision creates the illusion of all layers lit simultaneously.
