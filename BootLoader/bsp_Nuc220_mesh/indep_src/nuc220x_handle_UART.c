#include "nuc220x_handle_UART.h"
#include "bsp_nuc220x_UART.h"

// #define _LED_CONTROL_0 PA0
// #define _LED_CONTROL_1 PA1

#define _LED_CONTROL_0 PA12
#define _LED_CONTROL_1 PA1

void nuc220x_handle_UART1(void)
{
    if (sp_host_rx_flag && sp_host_rx_cnt)
    {
        int8_t number0 = 0;
        int8_t number1 = 0;
        int8_t number2 = 0;

        char data[2][10] = {0};

        if (strstr(sp_host_rx, "reset"))
            bsp_nu220x_WDT_reset();

        sscanf(sp_host_rx, "%3s,%d,%3s#", data[0], &number0, data[1]);
        // bat / tat chan PA0
        // led,0,off##
        if (strstr(data[0], "led"))
        {
            if (number0 == 0)
            {
                if (strstr(data[1], "on"))
                    _LED_CONTROL_0 = 1;
                else if (strstr(data[1], "off"))
                    _LED_CONTROL_0 = 0;
                else
                    printf(" NOT on/off \n");
            }
            else if (number0 == 1)
            {
                if (strstr(data[1], "on"))
                    _LED_CONTROL_1 = 1;
                else if (strstr(data[1], "off"))
                    _LED_CONTROL_1 = 0;
                else
                    printf(" NOT on/off \n");
            }
            else
                printf(" NOT Led!!! \n");
        }
        else
        {
            printf(" NOT format!!! \n");
        }

        bsp_nuc220x_UART1_reset_data();
    }
}
