#include "board.h"

void print_to_UART(const char* str, UART_HandleTypeDef *uart)
{
    const char* print = str;
    size_t len = strlen(print);
    HAL_UART_Transmit(uart, (uint8_t*)str, len, HAL_MAX_DELAY);
}

size_t checkHeapSpace(void)
{
    // size_t xAvailableHeapSpaceInBytes = xPortGetFreeHeapSize();
    // return xAvailableHeapSpaceInBytes;
    return xPortGetMinimumEverFreeHeapSize();
}

void print_fw_ver(UART_HandleTypeDef *uart)
{
    uint8_t major, minor, patch;
    major = BUILD_YEAR - 2000;
    minor = BUILD_MONTH;
    patch = BUILD_DAY;
    char text[30];
    sprintf("Firmware ver: %u.%u.%u", major, minor, patch);
    print_to_UART(text, &huart2);
}