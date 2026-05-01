#include "hc595.h"
#include "delay.h"

void HC595_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    gpio.GPIO_Pin = HC595_SER_PIN | HC595_SRCLK_PIN |
                    HC595_RCLK_PIN | HC595_OE_PIN | HC595_CLR_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HC595_PORT, &gpio);

    HC595_OE_DISABLE();
    HC595_CLR_ASSERT();
    delay_us(1);
    HC595_CLR_DEASSERT();
    HC595_SER_LOW();
    HC595_SRCLK_LOW();
    HC595_RCLK_LOW();

    HC595_Latch();
    HC595_OE_ENABLE();
}

void HC595_ShiftBit(uint8_t bit)
{
    if (bit)
        HC595_SER_HIGH();
    else
        HC595_SER_LOW();

    delay_us(1);
    HC595_SRCLK_HIGH();
    delay_us(1);
    HC595_SRCLK_LOW();
}

void HC595_ShiftByte(uint8_t data)
{
    for (int8_t i = 7; i >= 0; i--)
        HC595_ShiftBit((data >> i) & 0x01);
}

void HC595_Latch(void)
{
    delay_us(1);
    HC595_RCLK_HIGH();
    delay_us(1);
    HC595_RCLK_LOW();
}

void HC595_Flush(Hc595Frame *frame)
{
    for (int8_t i = HC595_CASCADE_DEPTH - 1; i >= 0; i--)
        HC595_ShiftByte(frame->buf[i]);
    HC595_Latch();
}

void HC595_FlushBlocking(Hc595Frame *frame)
{
    HC595_Flush(frame);
    delay_ms(1);
}

void HC595_Clear(void)
{
    HC595_CLR_ASSERT();
    delay_us(1);
    HC595_CLR_DEASSERT();
}

void HC595_SetOutput(uint8_t enabled)
{
    if (enabled)
        HC595_OE_ENABLE();
    else
        HC595_OE_DISABLE();
}
