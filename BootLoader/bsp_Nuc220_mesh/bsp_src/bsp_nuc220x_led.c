#include "bsp_nuc220x_led.h"

// #define _LED_BINK_3 PA13
#define _LED_BINK_3 PA3

#define _LED_NUMBER 8

uint8_t ledPins[_LED_NUMBER] = {0, 1, 2, 3, 4, 5, 12, 13};

void bsp_nu220x_led_init(void)
{
    /* Configure PA.2 as Output mode  */
    GPIO_SetMode(PA, BIT0, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT1, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT2, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT3, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT4, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT5, GPIO_PMD_OUTPUT);

    GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);

    PA0 = 0;
    PA1 = 0;
    PA2 = 0;
    PA3 = 0;
    PA4 = 0;
    PA5 = 0;
    PA12 = 0;
    PA13 = 0;
}

void bsp_nu220x_led_blink(void)
{
    _LED_BINK_3 = ~_LED_BINK_3;
}

void nhapNhayLEDs(uint32_t delayTime)
{
    for (uint32_t i = 0; i < _LED_NUMBER; i++)
    {
        // Bật đèn LED (LED ON)
        PA->DOUT |= (1 << ledPins[i]);      // Đặt chân GPIO ở mức cao (LED sáng)
        CLK_SysTickDelay(delayTime * 1000); // Đợi một khoảng thời gian

        // Tắt đèn LED (LED OFF)
        PA->DOUT &= ~(1 << ledPins[i]);     // Đặt chân GPIO ở mức thấp (LED tắt)
        CLK_SysTickDelay(delayTime * 1000); // Đợi một khoảng thời gian trước khi chuyển sang LED tiếp theo
    }
}
