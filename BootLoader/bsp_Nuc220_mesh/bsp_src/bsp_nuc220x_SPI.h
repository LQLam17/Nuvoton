#ifndef _BSP_NUC220X_SPI_
#define _BSP_NUC220X_SPI_
#include "bsp_nuc220x_system.h"

void bsp_nuc220x_SPI_init(void);

void bsp_nuc220x_SPI_get_version(void);

uint8_t bsp_nuc220x_read_write(uint8_t data);

#endif