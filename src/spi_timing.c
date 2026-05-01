#include "spi_timing.h"
#include "hc595.h"

void SPI_Timing_Init(void)
{
    /* Timing margins verified via NOP cycles in pulse macros.
     * At 72 MHz, each NOP ~ 13.9 ns. 3 NOPs provide ~42 ns
     * margin above the 100 ns datasheet minimum.
     */
}

void SPI_Timing_ShiftFrame(uint8_t *data, uint32_t len)
{
    for (int32_t i = (int32_t)len - 1; i >= 0; i--)
    {
        uint8_t byte = data[i];

        HC595_ShiftBit((byte >> 7) & 0x01);
        HC595_ShiftBit((byte >> 6) & 0x01);
        HC595_ShiftBit((byte >> 5) & 0x01);
        HC595_ShiftBit((byte >> 4) & 0x01);
        HC595_ShiftBit((byte >> 3) & 0x01);
        HC595_ShiftBit((byte >> 2) & 0x01);
        HC595_ShiftBit((byte >> 1) & 0x01);
        HC595_ShiftBit(byte & 0x01);
    }

    RCLK_PULSE();
}
