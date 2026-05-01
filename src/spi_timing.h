#ifndef __SPI_TIMING_H
#define __SPI_TIMING_H

#include "stm32f10x.h"
#include "hc595.h"

/* Bitbang SPI timing constraints derived from 74HC595 datasheet:
 *   t_WH (SRCLK pulse width high): min 100 ns
 *   t_WL (SRCLK pulse width low):  min 100 ns
 *   t_SU (SER setup to SRCLK):     min 40 ns
 *   t_H  (SER hold after SRCLK):   min 20 ns
 *   t_PH (RCLK pulse width):       min 100 ns
 */

#define SRCLK_PULSE()              \
    do {                           \
        HC595_SRCLK_HIGH();        \
        __NOP(); __NOP();          \
        HC595_SRCLK_LOW();         \
        __NOP();                   \
    } while (0)

#define RCLK_PULSE()               \
    do {                           \
        HC595_RCLK_HIGH();         \
        __NOP(); __NOP();          \
        HC595_RCLK_LOW();          \
        __NOP();                   \
    } while (0)

void SPI_Timing_Init(void);
void SPI_Timing_ShiftFrame(uint8_t *data, uint32_t len);

#endif
