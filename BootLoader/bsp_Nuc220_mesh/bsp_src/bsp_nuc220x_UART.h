#ifndef _BSP_NUC220LEAN_UART_
#define _BSP_NUC220LEAN_UART_

#include "bsp_nuc220x_system.h"

#define SERIAL_PORT_REV_BUFFER_SIZE 64

extern uint8_t sp_host_rx[SERIAL_PORT_REV_BUFFER_SIZE];
extern uint8_t sp_host_rx_cnt;
extern uint8_t sp_host_rx_flag;

void bsp_nuc220x_UART_init(UART_T *uart, uint32_t baundrate);
void bsp_nuc220x_UART_send_data(UART_T *uart, uint8_t *send_uart_data, uint8_t len);
void bsp_nuc220x_UART_receive_data(UART_T *uart, uint8_t *receive_uart_data, uint8_t len);
void bsp_nuc220x_UART_reset_data(UART_T *uart);

#endif