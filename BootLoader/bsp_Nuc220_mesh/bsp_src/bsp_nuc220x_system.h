#ifndef _BSP_NUC220LEAN_SYSTEM_
#define _BSP_NUC220LEAN_SYSTEM_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NUC200Series.h"

extern uint16_t tick_timer1;
extern uint8_t u8data_UID[3];

void bsp_nu220x_systems_init(void);
uint32_t bsp_systick_get(void);
void bsp_nu220x_systick_enable(void);
void bsp_delay_ms(uint32_t ms);

void SB_Timer1_Init(void);
void Timer1_SetTickMs(void);
uint16_t Timer1_GetTickMs(void);
void Timer1_ResetTickMs(void);

void bsp_nu220x_WDT_enable(void);
void bsp_nu220x_WDT_clear(void);
void bsp_nu220x_WDT_reset(void);
void bsp_nu220x_read_UID(void);

#endif