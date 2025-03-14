#ifndef BSP_NUC220X_BOOTLOADER_H
#define BSP_NUC220X_BOOTLOADER_H

#include <stdio.h>
#include "NUC200Series.h"
#include <string.h>

// Memory Address Assignment
#define FLASH_SECTOR0_ADDR 0xC000UL  // Memory To Store BootLoader	16K
#define FLASH_SECTOR1_ADDR 0x00UL    // Memory To Store App1				48K 0x4000
#define FLASH_SECTOR2_ADDR 0x18000UL // Memory To Store App2				48K	0x10000
#define DATA_FLASH_BASE_ADDR 0x1C000 // Memory To Store Data				16K

// BootLoader Command
#define BL_CMD_MEM_WRITE 0x50        // lệnh ghi vào bộ nhớ
#define BL_CMD_UPDATE_FIRMWARE2 0x52 // lệnh cập nhật phần mềm vào vùng nhớ thứ 2
#define BL_CMD_UPDATE_FIRMWARE1 0x53 // lệnh cập nhật phần mềm vào vùng nhớ thứ 1

// System Status Resister Stored In DataFlash Define By Developer
#define SSR_OPERATING_FW DATA_FLASH_BASE_ADDR   // Current Running Firmware 0:No Firmware, 1:FW1, 2:FW2
#define SSR_FW1_STATUS DATA_FLASH_BASE_ADDR + 4 // Firmware 1's Status 0:No Firmware, 1:Normal, 2:Error
#define SSR_FW2_STATUS DATA_FLASH_BASE_ADDR + 8 // Firmware 2's Status 0:No Firmware, 1:Normal, 2:Error

#define Buffer_Len 200 // Kích thước buffer nhận dữ liệu từ C#

// Hàm tiền xử lý
void BL_PreProcess();

// Hàm xử lý các lệnh từ C#
void BL_Process();

// Hàm xử lý lệnh ghi dữ liệu vào bộ nhớ
void BL_Cmd_Mem_Write_Handle(uint8_t *buffer);

// Hàm ghi dữ liệu vào bộ nhớ
void BL_Mem_Write(uint32_t mem_addr, uint8_t *buffer, uint8_t len);

// Hàm nhảy đến chương trình mới
void BL_Cmd_Update_Firmware_Handle();

// Hàm cho phép chỉnh sửa APROM
void BL_Enable_APROM_Update();

void BL_PreProcess();

#endif