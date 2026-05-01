#ifndef __HC595_H
#define __HC595_H

#include "stm32f10x.h"

/* Cascaded 74HC595 configuration
 * 6 chips in cascade = 48-bit shift register
 * Chip 1-2: Column anode control (16 bits)
 * Chip 3:   Layer cathode control (4 bits)
 * Chip 4-6: Reserved / expansion
 */
#define HC595_CASCADE_DEPTH    6
#define HC595_TOTAL_BITS       (HC595_CASCADE_DEPTH * 8)

#define HC595_PORT             GPIOB
#define HC595_SER_PIN          GPIO_Pin_0
#define HC595_SRCLK_PIN        GPIO_Pin_1
#define HC595_RCLK_PIN         GPIO_Pin_2
#define HC595_OE_PIN           GPIO_Pin_3
#define HC595_CLR_PIN          GPIO_Pin_4

#define HC595_SER_HIGH()       GPIO_SetBits(HC595_PORT, HC595_SER_PIN)
#define HC595_SER_LOW()        GPIO_ResetBits(HC595_PORT, HC595_SER_PIN)
#define HC595_SRCLK_HIGH()     GPIO_SetBits(HC595_PORT, HC595_SRCLK_PIN)
#define HC595_SRCLK_LOW()      GPIO_ResetBits(HC595_PORT, HC595_SRCLK_PIN)
#define HC595_RCLK_HIGH()      GPIO_SetBits(HC595_PORT, HC595_RCLK_PIN)
#define HC595_RCLK_LOW()       GPIO_ResetBits(HC595_PORT, HC595_RCLK_PIN)
#define HC595_OE_DISABLE()     GPIO_SetBits(HC595_PORT, HC595_OE_PIN)
#define HC595_OE_ENABLE()      GPIO_ResetBits(HC595_PORT, HC595_OE_PIN)
#define HC595_CLR_ASSERT()     GPIO_ResetBits(HC595_PORT, HC595_CLR_PIN)
#define HC595_CLR_DEASSERT()   GPIO_SetBits(HC595_PORT, HC595_CLR_PIN)

typedef struct {
    uint8_t buf[HC595_CASCADE_DEPTH];
} Hc595Frame;

void HC595_Init(void);
void HC595_ShiftBit(uint8_t bit);
void HC595_ShiftByte(uint8_t data);
void HC595_Latch(void);
void HC595_Flush(Hc595Frame *frame);
void HC595_FlushBlocking(Hc595Frame *frame);
void HC595_Clear(void);
void HC595_SetOutput(uint8_t enabled);

#endif
