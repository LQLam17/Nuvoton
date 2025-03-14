#include "bsp_nuc220x_RTC.h"

volatile uint32_t g_u32RTCTickINT;
S_RTC_TIME_DATA_T sWriteRTC, sReadRTC;
uint32_t u32Sec;

void RTC_IRQHandler(void)
{
    /* To check if RTC Tick interrupt occurred */
    if (RTC_GET_TICK_INT_FLAG() == 1)
    {
        /* Clear RTC tick interrupt flag */
        RTC_CLEAR_TICK_INT_FLAG();

        // g_u32RTCTickINT++;

        // PB8 ^= 1;
    }
}

void bsp_nuc220x_RTC_init(void)
{
    printf("# Showing RTC Date/Time on UART-0.\n\n");

    /* Enable RTC module clock */
    CLK_EnableModuleClock(RTC_MODULE);

    /* Open RTC and start counting */
    sWriteRTC.u32Year = 2014;
    sWriteRTC.u32Month = 2;
    sWriteRTC.u32Day = 6;
    sWriteRTC.u32DayOfWeek = RTC_THURSDAY;
    sWriteRTC.u32Hour = 15;
    sWriteRTC.u32Minute = 30;
    sWriteRTC.u32Second = 30;
    sWriteRTC.u32TimeScale = RTC_CLOCK_24;

    /* Enable RTC tick interrupt, one RTC tick is 1/4 second */
    // NVIC_EnableIRQ(RTC_IRQn);
    RTC_EnableInt(RTC_RIER_TIER_Msk);
    RTC_SetTickPeriod(RTC_TICK_1_4_SEC);

    printf("Date/Time is: (Use PB.8 to check tick period time is 1/4 second) \n");
    RTC_Open(&sWriteRTC);

    /* Use PB.8 to check tick period time */
    // PB->PMD = (PB->PMD & ~0x00030000) | (0x00010000);

    u32Sec = 0;
    g_u32RTCTickINT = 0;
}

void bsp_nuc220x_RTC_test(void)
{
    RTC_GetDateAndTime(&sReadRTC);
    printf("    %d/%02d/%02d %02d:%02d:%02d\r",
           sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);
}