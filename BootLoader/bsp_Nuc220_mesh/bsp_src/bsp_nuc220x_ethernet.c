#include "bsp_nuc220x_ethernet.h"


uint32_t wizchip_read()
{
    return bsp_nuc220x_read_write(0x00);
}

void wizchip_write(uint32_t data)
{
    bsp_nuc220x_read_write(data);
}

void wizchip_select(){
	SPI0->SSR |= 1;
}

void wizchip_deselect(){
	SPI0->SSR &= ~1UL;
}

void wizchip_readburst(uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        buf[i] = wizchip_read();
    }
}

void wizchip_writeburst(uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        wizchip_write(buf[i]);
    }
}

void W5500_Init()
{
	uint8_t tmp;
	uint8_t mem_size[2][8] = {{2,2,2,2,2,2,2,2}, {2,2,2,2,2,2,2,2}};
	
	GPIO_SetMode(PA, BIT0, GPIO_PMD_OUTPUT);
	
	PA0 = 0;
	uint8_t temp = 0xFF;
	while(temp--);
	PA0 = 1;
	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);
		
	if(ctlwizchip(CW_INIT_WIZCHIP, (void *)mem_size) == -1)
		bsp_nuc220x_UART1_send_data("Init Failed", 11);
	else
		bsp_nuc220x_UART1_send_data("Init Success", 12);
}