#include "bsp_nuc220x_system.h"
#include "sys.h"
#include "bsp_nuc220x_UART.h"

#define PLL_CLOCK 50000000
// volatile uint8_t g_u8IsWDTTimeoutINT = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock and internal 10 kHz */
    CLK_EnableXtalRC((CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_OSC10K_EN_Msk));

    /* Waiting for clock ready */
    CLK_WaitClockReady((CLK_CLKSTATUS_XTL12M_STB_Msk | CLK_CLKSTATUS_OSC10K_STB_Msk));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
}

// tick timer
static uint16_t tick_Timer_reload_value = 0;
static uint16_t tick_Timer_hz = 0;
static uint32_t tick_cnt = 0;

void SB_Timer1_Init(void);

void bsp_nuc220x_TIMER0_init()
{

    /* Enable Timer 0~3 module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);

    /* Select Timer 0~3 module clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, (uint32_t)NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2_S_HCLK, (uint32_t)NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3_S_HXT, (uint32_t)NULL);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD, TM0, TM2, TM3 and TM2_EXT */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk |
                      SYS_GPB_MFP_PB8_Msk | SYS_GPB_MFP_PB10_Msk | SYS_GPB_MFP_PB11_Msk |
                      SYS_GPB_MFP_PB2_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD |
                    SYS_GPB_MFP_PB8_TM0 | SYS_GPB_MFP_PB10_TM2 | SYS_GPB_MFP_PB11_TM3 |
                    SYS_GPB_MFP_PB2_TM2_EXT;

    SYS->ALT_MFP &= ~SYS_ALT_MFP_PB2_Msk;
    SYS->ALT_MFP |= SYS_ALT_MFP_PB2_TM2_EXT;

    SYS->ALT_MFP2 = SYS_ALT_MFP2_PB2_TM2_EXT;
}

void bsp_nu220x_systick_enable(void)
{

    bsp_nuc220x_TIMER0_init();
    SB_Timer1_Init();

    /* Enable Timer 0~3 module clock */
    CLK_EnableModuleClock(TMR0_MODULE);

    /* Select Timer 0~3 module clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HIRC, (uint32_t)NULL);
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);

    // 1ms ngat TIMEr 1 lan
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0x17);
    TIMER_SET_CMP_VALUE(TIMER0, 0x3E8);

    TIMER_EnableInt(TIMER0);

    /* Enable Timer2 NVIC */
    NVIC_SetPriority(TMR0_IRQn, 1);
    NVIC_EnableIRQ(TMR0_IRQn);

    /* Start Timer0,counting */
    TIMER_Start(TIMER0);
    TIMER_Start(TIMER1);
}

void TMR0_IRQHandler(void)
{
    /* Clear Timer0 time-out interrupt flag */
    if (TIMER_GetIntFlag(TIMER0) == 1)
    {
        // printf(" abc = %d\n", bsp_systick_get());
        TIMER_ClearIntFlag(TIMER0);
        tick_cnt++;
				//bsp_nuc220x_UART_send_data(UART1, "f", 1);
    }
}

uint32_t bsp_systick_get(void)
{
    return tick_cnt;
}

void bsp_delay_ms(uint32_t ms)
{
    uint32_t time_latch = 0;
    time_latch = bsp_systick_get();
    while (bsp_systick_get() - time_latch <= ms)
        ;
}

uint16_t tick_timer1 = 0;

void SB_Timer1_Init(void)
{
    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HIRC, MODULE_NoMsk);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(TMR1_MODULE);
    SYS_ResetModule(TMR1_RST);
    TIMER1->TCSR &= ~TIMER_TCSR_CRST_Msk;       // Reset TIMER1
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1); // 1tick
    TIMER_SET_PRESCALE_VALUE(TIMER1, 0x17);     // 23
    TIMER_SET_CMP_VALUE(TIMER1, 0x3E8);         // 1ms
    TIMER_EnableInt(TIMER1);
    tick_timer1 = 0;
    NVIC_EnableIRQ(TMR1_IRQn);
}

void TMR1_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER1) == 1)
    {
        tick_timer1++;
        TIMER_ClearIntFlag(TIMER1);
    }
}

void Timer1_SetTickMs(void)
{
    TIMER_Start(TIMER1);
    tick_timer1 = 0;
}

uint16_t Timer1_GetTickMs(void)
{
    return tick_timer1;
}

void Timer1_ResetTickMs(void)
{
    TIMER_Stop(TIMER1);
    tick_timer1 = 0;
}

// WDT
void bsp_nu220x_WDT_enable(void)
{
    /* Enable WDT module clock */
    CLK_EnableModuleClock(WDT_MODULE);

    /* Select WDT module clock source */
    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDT_S_LIRC, (uint32_t)NULL);

    /* Enable WDT time-out reset function and select time-out interval to 2^14 * WDT clock then start WDT counting */
    WDT_Open(WDT_TIMEOUT_2POW16, WDT_RESET_DELAY_1026CLK, TRUE, FALSE); // 5 giay
    // WDT_Open(WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_1026CLK, TRUE, FALSE); // 1 giay

    /* Enable WDT interrupt function */
    WDT_EnableInt();

    /* Enable WDT NVIC */
    NVIC_EnableIRQ(WDT_IRQn);
}

void WDT_IRQHandler(void)
{
    if (WDT_GET_TIMEOUT_INT_FLAG() == 1)
    {
        /* Clear WDT time-out interrupt flag */
        WDT_CLEAR_TIMEOUT_INT_FLAG();

        // g_u8IsWDTTimeoutINT = 1;

        printf("WDT time-out interrupt occurred.\n");
    }
}

void bsp_nu220x_WDT_clear(void)
{
    // g_u8IsWDTTimeoutINT = 0;
    WDT_RESET_COUNTER();
}

void bsp_nu220x_WDT_reset(void)
{
    SYS_ResetChip();
}

void bsp_nu220x_systems_init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    // bsp_nu220x_WDT_enable();

        // Khoi dong systick
    bsp_nu220x_systick_enable();
		
		CLK_EnableModuleClock(ISP_MODULE);
    /* Lock protected registers */
    // SYS_LockReg();
}

uint8_t u8data_UID[3] = {0};
void bsp_nu220x_read_UID(void)
{
    /* Enable FMC ISP function */
    FMC_Open();

    for (uint8_t i = 0; i < 3; i++)
    {
        u8data_UID[i] = FMC_ReadUID(i);
        printf("  Unique ID %d ........................... [0x%02x]\n", i, u8data_UID[i]);
    }

    /* Disable FMC ISP function */
    FMC_Close();
}