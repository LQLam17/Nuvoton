#include "bsp_nuc220x_UART.h"

uint8_t sp_host_rx[SERIAL_PORT_REV_BUFFER_SIZE] = {0};
uint8_t sp_host_rx_cnt = 0;
uint8_t sp_host_rx_flag = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 1 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/

void bsp_nuc220x_UART_init(UART_T *uart, uint32_t baundrate)
{
    if (uart == UART0)
        CLK_EnableModuleClock(UART0_MODULE);
    else if (uart == UART1)
        CLK_EnableModuleClock(UART1_MODULE);

    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART_S_Msk;
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART_S_HXT;

    if (uart == UART0)
    {
        SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
        SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
    }

    else if (uart == UART1)
    {
        SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB4_Msk | SYS_GPB_MFP_PB5_Msk);
        SYS->GPB_MFP |= SYS_GPB_MFP_PB4_UART1_RXD | SYS_GPB_MFP_PB5_UART1_TXD;
    }

    UART_Open(uart, baundrate);
}

void bsp_nuc220x_UART_send_data(UART_T *uart, uint8_t *send_uart_data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        uart->THR = *(send_uart_data + i);
        while ((uart->ISR & UART_ISR_THRE_IF_Msk) == 0)
            ;
    }
}

void bsp_nuc220x_UART_receive_data(UART_T *uart, uint8_t *receive_uart_data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        while ((uart->ISR & UART_ISR_RDA_IF_Msk) == 0)
            ;
        *(receive_uart_data + i) = uart->RBR;
    }
}

void bsp_nuc220x_UART_reset_data(UART_T *uart)
{
    UART_ClearIntFlag(uart, UART_ISR_RDA_INT_Msk | UART_ISR_TOUT_INT_Msk);
    for (uint8_t i = 0; i < SERIAL_PORT_REV_BUFFER_SIZE; i++)
    {
        sp_host_rx[i] = 0;
    }
    sp_host_rx_cnt = 0;
    sp_host_rx_flag = 0;
}