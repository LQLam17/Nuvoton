#ifndef _BSP_NUC220LEAN_LORA_
#define _BSP_NUC220LEAN_LORA_

#include "bsp_nuc220x_system.h"
#include "SX1278.h"
#include "SX1278_LoRa.h"
#include "SX1278LoRa_Misc.h"
#include "radio.h"

#define _LORA_REV_BUFFER_SIZE 64

void nuc220x_lora_init(void);
void nuc220x_handle_lora_rx(void);

void test_lora_tx(void);

#endif