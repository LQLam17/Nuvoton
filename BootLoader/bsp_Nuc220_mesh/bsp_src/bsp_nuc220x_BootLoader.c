#include "bsp_nuc220x_BootLoader.h"
#include "bsp_nuc220x_UART.h"

uint8_t RX_BUFFER[Buffer_Len] = {0};
uint8_t BL_Temp = 0;
uint32_t ssr_operating_fw = 0;
uint32_t ssr_fw1_status = 0;
uint32_t ssr_fw2_status = 0;
uint8_t bl_option = 0;

void (*ResetFunc)(void);

void BL_Process()
{
	for (uint8_t i = 0; i < Buffer_Len; i++)
		RX_BUFFER[Buffer_Len] = 0;
	bsp_nuc220x_UART_receive_data(UART0, RX_BUFFER, 1);
	bsp_nuc220x_UART_receive_data(UART0, (RX_BUFFER + 1), RX_BUFFER[0]);
	switch (RX_BUFFER[1])
	{
	case BL_CMD_MEM_WRITE:
		BL_Cmd_Mem_Write_Handle(RX_BUFFER);
		break;
	case BL_CMD_UPDATE_FIRMWARE2:
		bsp_nuc220x_UART_send_data(UART1, "UpdateFW2\n", 11);
		BL_Cmd_Update_Firmware_Handle(FLASH_SECTOR2_ADDR);
		break;
	case BL_CMD_UPDATE_FIRMWARE1:
		bsp_nuc220x_UART_send_data(UART1, "UpdateFW1\n", 11);
		BL_Cmd_Update_Firmware_Handle(FLASH_SECTOR1_ADDR);
		break;
	default:
		break;
	}
}

uint32_t BL_CalCheckSum(uint8_t *buffer, uint8_t len)
{
	uint32_t sum = 0;
	for (uint8_t i = 0; i < len; i++)
	{
		sum += *(buffer + i);
	}
	return sum;
}

void BL_Mem_Write(uint32_t mem_addr, uint8_t *buffer, uint8_t len)
{
	uint32_t temp = 0;
	uint8_t sub_buffer[20];
	if (BL_Temp == 0)
		FMC_Erase(mem_addr);
	BL_Temp = (BL_Temp + 1) % 4;

	uint8_t real_len = len / 4;
	for (uint8_t i = 0; i < len; i += 4)
	{
		temp = (*(buffer + i + 3) << 24) | (*(buffer + i + 2) << 16) | (*(buffer + i + 1) << 8) | (*(buffer + i));
		FMC_Write(mem_addr + i, temp);
	}
}

void BL_Cmd_Mem_Write_Handle(uint8_t *buffer)
{
	uint8_t total_len = *(buffer) + 1;
	uint32_t mem_addr = (*(buffer + 5) << 24) | (*(buffer + 4) << 16) | (*(buffer + 3) << 8) | (*(buffer + 2));
	uint32_t host_check_sum = (*(buffer + total_len - 1) << 24) | (*(buffer + total_len - 2) << 16) | (*(buffer + total_len - 3) << 8) | (*(buffer + total_len - 4));
	if (BL_CalCheckSum(buffer + 6, total_len - 10) == host_check_sum)
	{
		BL_Mem_Write(mem_addr, (buffer + 6), total_len - 10);
	}
}

void BL_Cmd_Update_Firmware_Handle(uint32_t mem_addr)
{
	uint32_t ssr_operating_fw_temp = 0, ssr_fw1_status_temp = 0, ssr_fw2_status_temp = 0;
	uint32_t ssr_operating_fw_temp1 = 0, ssr_fw1_status_temp1 = 0, ssr_fw2_status_temp1 = 0;

	FMC->ISPCON |= FMC_ISPCON_ISPEN_Msk;

	// Read System Status Register
	ssr_operating_fw_temp = FMC_Read(DATA_FLASH_BASE_ADDR);
	ssr_fw1_status_temp = FMC_Read(DATA_FLASH_BASE_ADDR + 4);
	ssr_fw2_status_temp = FMC_Read(DATA_FLASH_BASE_ADDR + 8);

	ssr_operating_fw_temp1 = ssr_operating_fw_temp;
	ssr_fw1_status_temp1 = ssr_fw1_status_temp;
	ssr_fw2_status_temp1 = ssr_fw2_status_temp;

	// If Want To Jump To App1
	if (mem_addr == FLASH_SECTOR1_ADDR)
	{
		ssr_fw1_status_temp = 1;
		ssr_operating_fw_temp = 1;
	}
	// If Want To Jump To App2
	else if (mem_addr == FLASH_SECTOR2_ADDR)
	{
		ssr_fw2_status_temp = 1;
		ssr_operating_fw_temp = 2;
	}
	// Update System Status Register in DATAFLASH if there is any change
	if ((ssr_fw1_status_temp != ssr_fw1_status_temp1) || (ssr_fw2_status_temp != ssr_fw2_status_temp1) || (ssr_operating_fw_temp != ssr_operating_fw_temp1))
	{
		FMC_Erase(DATA_FLASH_BASE_ADDR);
		FMC_Write(DATA_FLASH_BASE_ADDR, ssr_operating_fw_temp);
		FMC_Write(DATA_FLASH_BASE_ADDR + 4, ssr_fw1_status_temp);
		FMC_Write(DATA_FLASH_BASE_ADDR + 8, ssr_fw2_status_temp);
	}

	void (*ResetHandler)(void);

	SYS->IPRSTC2 = 0xFFFFFFFF;
	SYS->IPRSTC2 = 0;

	/* Set vector table page address */
	FMC_SetVectorPageAddr(mem_addr);

	/* Set function pointer to execute LDROM */
	ResetFunc = (void (*)(void))M32(mem_addr + 4);

	/* Set main stack Pointer */
	__set_MSP(mem_addr);

	/* Reset handfauler */
	ResetFunc();
}

void BL_Enable_APROM_Update()
{
	FMC_Open();
	FMC->ISPCON |= FMC_ISPCON_APUEN_Msk;
}

void BL_PreProcess()
{
	uint32_t fw1_read = 0, fw2_read = 0;
	uint32_t ssr_operating_fw_temp = 0, ssr_fw1_status_temp = 0, ssr_fw2_status_temp = 0;
	ssr_operating_fw = FMC_Read(DATA_FLASH_BASE_ADDR);
	ssr_fw1_status = FMC_Read(DATA_FLASH_BASE_ADDR + 4);
	ssr_fw2_status = FMC_Read(DATA_FLASH_BASE_ADDR + 8);
	ssr_operating_fw_temp = ssr_operating_fw;
	ssr_fw1_status_temp = ssr_fw1_status;
	ssr_fw2_status_temp = ssr_fw2_status;

	// If SSR Haven't Initialized ==> Initialize It
	if (ssr_operating_fw > 100)
		ssr_operating_fw = 0;
	if (ssr_fw1_status > 100)
		ssr_fw1_status = 0;
	if (ssr_fw2_status > 100)
		ssr_fw2_status = 0;

	/**/
	// Read FW1 and FW2 at the offset of 0x200
	fw1_read = FMC_Read(FLASH_SECTOR1_ADDR + 0x200);
	fw2_read = FMC_Read(FLASH_SECTOR2_ADDR + 0x200);

	// Determine if microcontroller has FW1
	if ((fw1_read != 0xFFFFFFFF) && (ssr_fw1_status != 2))
		ssr_fw1_status = 1;
	else if ((fw1_read == 0xFFFFFFFF))
		ssr_fw1_status = 0;

	// Determine if microcontroller has FW2
	if ((fw2_read != 0xFFFFFFFF) && (ssr_fw2_status != 2))
		ssr_fw2_status = 1;
	else if ((fw2_read == 0xFFFFFFFF))
		ssr_fw2_status = 0;

	/**/
	// bl_option = 0 ==> run current app, bl_option = 1 ==> get data from GUI
	if (PA0 == 0)
	{
		bl_option = 1;
		bsp_nuc220x_UART_send_data(UART1, "Nut nhan = 0\n", 14);
	}

	if (bl_option == 0)
	{
		bsp_nuc220x_UART_send_data(UART1, "Nut nhan = 1\n", 14);
		// If controller has FW2 in sector2 and has no error
		if (ssr_fw2_status == 1)
		{
			bsp_nuc220x_UART_send_data(UART1, "Co FW2\n", 8);
			ssr_operating_fw = 2;
		}

		// If controller has FW1 in sector1 and FW2 has error or has no FW2
		if ((ssr_fw1_status == 1) && (ssr_fw2_status != 1))
		{
			bsp_nuc220x_UART_send_data(UART1, "FW2 loi\n", 9);
			ssr_operating_fw = 1;
		}

		// When has no FW1 and FW2
		if ((ssr_fw1_status != 1) && (ssr_fw2_status != 1))
		{
			bsp_nuc220x_UART_send_data(UART1, "Khong co ca 2 FW\n", 18);
			ssr_operating_fw = 0;
		}

		// Update SSR when has changes
		if ((ssr_operating_fw != ssr_operating_fw_temp) || (ssr_fw1_status != ssr_fw1_status_temp) || (ssr_fw2_status != ssr_fw2_status_temp))
		{
			FMC_Erase(DATA_FLASH_BASE_ADDR);
			FMC_Write(DATA_FLASH_BASE_ADDR, ssr_operating_fw);
			FMC_Write(DATA_FLASH_BASE_ADDR + 4, ssr_fw1_status);
			FMC_Write(DATA_FLASH_BASE_ADDR + 8, ssr_fw2_status);
		}

		if (ssr_operating_fw == 1)
		{
			bsp_nuc220x_UART_send_data(UART1, "Chay chuong trinh 1\n", 21);
			BL_Cmd_Update_Firmware_Handle(FLASH_SECTOR1_ADDR);
		}
		else if (ssr_operating_fw == 2)
		{
			bsp_nuc220x_UART_send_data(UART1, "Chay chuong trinh 2\n", 21);
			BL_Cmd_Update_Firmware_Handle(FLASH_SECTOR2_ADDR);
		}
	}
}