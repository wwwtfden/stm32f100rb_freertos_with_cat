#include "board.h"
// #include "cat.h"

void print_to_UART(const char* str, UART_HandleTypeDef *uart)
{
    NVIC_DisableIRQ(USART2_IRQn);
    const char* print = str;
    size_t len = strlen(print);
    HAL_UART_Transmit(uart, (uint8_t*)str, len, HAL_MAX_DELAY);
    NVIC_EnableIRQ(USART2_IRQn);
}

size_t checkHeapSpace(void)
{
    // size_t xAvailableHeapSpaceInBytes = xPortGetFreeHeapSize();
    // return xAvailableHeapSpaceInBytes;
    return xPortGetMinimumEverFreeHeapSize();
}

cat_return_state print_fw_ver(UART_HandleTypeDef *uart)
{
    uint8_t major, minor, patch;
    major = BUILD_YEAR - 2000;
    minor = BUILD_MONTH;
    patch = BUILD_DAY;
    char* text = pvPortMalloc(80*sizeof(char));
    snprintf(text, 80, "Firmware ver: %d.%d.%d\r\n", major, minor, patch);
    osDelay(100);
    print_to_UART(text, &huart2);
    vPortFree(text);

    osDelay(1);
    parser_buf_reset();
    return 0;
}

QueueHandle_t queue;