/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Author: Le Quang Lam
 * $Date: 11/2/2025
 * @brief
 *     Chương trình BSP có các API hỗ trợ giao tiếp với phần cứng
 *
 * @note
 * Copyright (C) 2014 Nuvoton BSP.
 *
 ******************************************************************************/
// #include <stdio.h>
// #include "NUC200Series.h"

#include "bsp_nuc220x_system.h"
// #include "bsp_nuc220x_SPI.h"
#include "bsp_nuc220x_UART.h"
#include "bsp_nuc220x_BootLoader.h"
#include "gpio.h"

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
  /* Init systems */
  bsp_nu220x_systems_init();

  /* Init UART0  */
  bsp_nuc220x_UART_init(UART0, 38400);

  /* Init UART1  */
  bsp_nuc220x_UART_init(UART1, 38400);

  /* Init GPIO */
  GPIO_SetMode(PA, BIT0, GPIO_PMD_INPUT);
  // SPI
  // bsp_nuc220x_SPI_init();

  bsp_nuc220x_UART_send_data(UART1, "BootLoader\n", 13);

  BL_Enable_APROM_Update();

  BL_PreProcess();

  while (1)
  {
    BL_Process();
  }
}
