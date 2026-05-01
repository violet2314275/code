/**
 * AI-Driven Embedded Architecture: STM32 & 74HC595 Light Cube
 * Main entry point. Initializes hardware and runs animation loop.
 */

#include "stm32f10x.h"
#include "hc595.h"
#include "delay.h"
#include "animation.h"
#include "spi_timing.h"

static void SystemClock_Config(void);
static void GPIO_Config(void);

int main(void)
{
    SystemClock_Config();
    delay_init();
    GPIO_Config();
    HC595_Init();
    SPI_Timing_Init();
    ANIM_Init();

    uint32_t last_tick = 0;
    AnimationId anim_seq[] = {
        ANIM_WAVE_UP,
        ANIM_SNAKE_3D,
        ANIM_PLANE_ROTATE,
        ANIM_RANDOM_SPARKLE,
        ANIM_SINEWAVE
    };
    uint8_t anim_idx = 0;

    while (1)
    {
        if ((GetTickCount() - last_tick) > 3000)
        {
            ANIM_SetAnimation(anim_seq[anim_idx]);
            anim_idx = (anim_idx + 1) % (sizeof(anim_seq) / sizeof(anim_seq[0]));
            last_tick = GetTickCount();
        }

        ANIM_Tick();
        ANIM_Render();
    }
}

static void SystemClock_Config(void)
{
    ErrorStatus HSEStartUpStatus;

    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);

        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while (RCC_GetSYSCLKSource() != 0x08);
    }
    else
    {
        while (1);
    }
}

static void GPIO_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}
