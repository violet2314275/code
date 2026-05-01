#include "delay.h"

static volatile uint32_t us_tick;

void SysTick_Handler(void)
{
    us_tick++;
}

void delay_init(void)
{
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksCmd(&clocks);
    uint32_t sysclk = clocks.HCLK_Frequency;
    SysTick_Config(sysclk / 1000000);
}

void delay_us(uint32_t us)
{
    uint32_t start = us_tick;
    while ((us_tick - start) < us);
}

void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
        delay_us(1000);
}
