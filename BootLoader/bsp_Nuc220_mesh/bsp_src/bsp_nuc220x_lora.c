#include "bsp_nuc220x_lora.h"

uint8_t Mode = 0;
unsigned char RF_RX_buf[_LORA_REV_BUFFER_SIZE] = {0};

void nuc220x_lora_init(void)
{
    uint8_t version;
    unsigned char CheckConfigErr = 0;
    unsigned int TimeOnAir = 0;

    Mode = SX1276SetMode(LORA);
    SX1276Init();
    if (Mode == LORA)
    {
        version = SX1276LoRaGetVersion();
        CheckConfigErr = SX1276CheckLoRaConfigError();
        while (version != 0x12 && CheckConfigErr != 0x0)
        {
            SX1276Init();
            // printf("\rLoRa Init Fail !!!!!\n");
        }

        // SX1276LoRaSetRFPower(20);        // Thiết lập công suất phát 20 dBm
        // SX1276LoRaSetSignalBandwidth(1); // Thiết lập băng thông 250 kHz
        // SX1276LoRaSetSpreadingFactor(8); // Thiết lập spreading factor 8
        // SX1276LoRaSetErrorCoding(1);     // Thiết lập Error Coding Rate 4/5

        // printf("\r\nLoRa Init Done !!!!!\n");
        TimeOnAir = SX1276GetTimeOnAir();
        // printf(" \rTimeOnAir : %d[ms] \n", TimeOnAir);
        SX1276StartCad();
    }
}

void nuc220x_handle_lora_rx()
{
    switch (SX1276Process()) // xử lý dữ liệu RF
    {
    case RF_CHANNEL_ACTIVITY_DETECTED:
        // printf("Channel Active !!! \n");
        break;

    case RF_CHANNEL_EMPTY:
        // printf("Channel Empty !!! \n");
        SX1276StartRx();
        break;

    case RF_RX_RUNNING:
        // printf("RX Running !!! \n");
        break;

    case RF_RX_TIMEOUT:
        // printf("RX Timeout !!!\n ");
        SX1276StartCad();
        break;

    case RF_RX_DONE:
        SX1276GetRxPacket(RF_RX_buf, _LORA_REV_BUFFER_SIZE); // nhận dữ liệu RF từ Master.
        if (RF_RX_buf > 0)
        {
            printf(" RX =>\n");
            for (uint8_t i = 0; i < _LORA_REV_BUFFER_SIZE; i++)
            {
                printf("0x%02x ", RF_RX_buf[i]);
            }
            printf("\n");
        }
        else
        {
            // printf("RX RECEIVE ERR !!!!!\n");
        }

        break;

    case RF_TX_RUNNING:
        // printf("Tx Running !!! \n");
        break;

    case RF_TX_DONE:
        printf("Tx Done !!! \n");
        if (Mode == LORA)
        {
            SX1276StartRx();
        }
        if (Mode == FSK)
        {
            SX1276StartRx();
        }
        break;

    default:
        break;
    }
}

void nuc220x_handle_lora_tx(uint8_t *data_tx, uint8_t data_len)
{
    SX1276SetTxPacket(data_tx, data_len);
}

void test_lora_tx(void)
{
    uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    nuc220x_handle_lora_tx(data, 10);
}
