#include "bsp_nuc220x_SPI.h"

#define TEST_COUNT 16

uint32_t g_au32SourceData[TEST_COUNT];
uint32_t g_au32DestinationData[TEST_COUNT];
volatile uint32_t g_u32TxDataCount;
volatile uint32_t g_u32RxDataCount;

void bsp_nuc220x_SPI_init(void)
{
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /*Update System Core Clock * /
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Setup SPI0 multi-function pins */
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC0_Msk | SYS_GPC_MFP_PC1_Msk | SYS_GPC_MFP_PC2_Msk | SYS_GPC_MFP_PC3_Msk);
    SYS->GPC_MFP |= (SYS_GPC_MFP_PC0_SPI0_SS0 | SYS_GPC_MFP_PC1_SPI0_CLK | SYS_GPC_MFP_PC2_SPI0_MISO0 | SYS_GPC_MFP_PC3_SPI0_MOSI0);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC0_Msk | SYS_ALT_MFP_PC1_Msk | SYS_ALT_MFP_PC2_Msk | SYS_ALT_MFP_PC3_Msk);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PC0_SPI0_SS0 | SYS_ALT_MFP_PC1_SPI0_CLK | SYS_ALT_MFP_PC2_SPI0_MISO0 | SYS_ALT_MFP_PC3_SPI0_MOSI0);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 1 MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 1000000);

    SPI_SET_DATA_WIDTH(SPI0, 8);
    SPI_SET_MSB_FIRST(SPI0);

    /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);
    // SPI_DisableAutoSS(SPI0);
}

void SPI0_IRQHandler(void)
{
    /* Check RX EMPTY flag */
    while (SPI_GET_RX_FIFO_EMPTY_FLAG(SPI0) == 0)
    {
        /* Read RX FIFO */
        g_au32DestinationData[g_u32RxDataCount++] = SPI_READ_RX0(SPI0);
    }
    /* Check TX FULL flag and TX data count */
    while ((SPI_GET_TX_FIFO_FULL_FLAG(SPI0) == 0) && (g_u32TxDataCount < TEST_COUNT))
    {
        /* Write to TX FIFO */
        SPI_WRITE_TX0(SPI0, g_au32SourceData[g_u32TxDataCount++]);
    }
    if (g_u32TxDataCount >= TEST_COUNT)
        SPI_DisableInt(SPI0, SPI_FIFO_TX_INT_MASK); /* Disable TX FIFO threshold interrupt */

    /* Check the RX FIFO time-out interrupt flag */
    if (SPI_GetIntFlag(SPI0, SPI_FIFO_TIMEOUT_INT_MASK))
    {
        /* If RX FIFO is not empty, read RX FIFO. */
        while ((SPI0->STATUS & SPI_STATUS_RX_EMPTY_Msk) == 0)
            g_au32DestinationData[g_u32RxDataCount++] = SPI_READ_RX0(SPI0);
    }
}

void bsp_nuc220x_SPI_write(uint8_t add)
{
    SPI0->SSR = 1;

    /* Write to TX FIFO */
    SPI_WRITE_TX0(SPI0, add | 0x80);

    SPI_TRIGGER(SPI0);

    while (SPI_IS_BUSY(SPI0))
        ;

    SPI0->SSR = 0;
}

uint8_t bsp_nuc220x_SPI_read(uint8_t add)
{
    uint8_t data = 0;

    // Write lan 1
    SPI0->SSR = 1;

    /* Write to TX FIFO */
    SPI_WRITE_TX0(SPI0, add & 0x7F);
    SPI_TRIGGER(SPI0);
    while (SPI_IS_BUSY(SPI0))
        ;

    SPI_WRITE_TX0(SPI0, 0x00);
    SPI_TRIGGER(SPI0);
    while (SPI_IS_BUSY(SPI0))
        ;

    data = SPI_READ_RX0(SPI0);

    SPI0->SSR = 0;

    return data;
}

uint8_t bsp_nuc220x_read_write(uint8_t data)
{
    SPI0->SSR |= 1;

    while (SPI_IS_BUSY(SPI0))
        ;

    SPI_WRITE_TX0(SPI0, data);

    while (SPI_IS_BUSY(SPI0))
        ;

    uint32_t data_read = SPI_READ_RX0(SPI0);

    SPI0->SSR &= ~1;
    return data_read;
}

void bsp_nuc220x_SPI_get_version(void)
{
    uint8_t data_read = 0;

    data_read = bsp_nuc220x_SPI_read(0x42);

    printf(" Version Lora SX1276: 0x%2X \n", data_read);
}
