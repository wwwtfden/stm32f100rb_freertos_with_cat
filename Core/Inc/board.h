#ifndef INC_BOARD_H_
#define INC_BOARD_H

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include "cat.h"

#define DIM(x) (sizeof(x) / sizeof((x)[0]))

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

#define BUILD_YEAR (__DATE__[7] - '0' + 10 * (__DATE__[8] - '0') + 100 * (__DATE__[9] - '0') + 1000 * (__DATE__[10] - '0'))
#define BUILD_MONTH (__DATE__[0] == 'J' ? ((__DATE__[1] == 'a') ? 1 : ((__DATE__[2] == 'n') ? 6 : 7)) : \
                    (__DATE__[0] == 'F') ? 2 : \
                    (__DATE__[0] == 'M') ? ((__DATE__[1] == 'a') ? 3 : 5) : \
                    (__DATE__[0] == 'A') ? ((__DATE__[1] == 'p') ? 4 : 8) : \
                    (__DATE__[0] == 'S') ? 9 : \
                    (__DATE__[0] == 'O') ? 10 : \
                    (__DATE__[0] == 'N') ? 11 : 12)
#define BUILD_DAY ((__DATE__[4] - '0') + 10 * (__DATE__[3] - '0'))

void UART3_Init(uint32_t baudrate);
void UART3_DeInit();


void print_to_UART(const char* str, UART_HandleTypeDef *uart);

size_t checkHeapSpace(void);

cat_return_state print_fw_ver(UART_HandleTypeDef *uart);

extern QueueHandle_t queue;

#endif