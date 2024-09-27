#ifndef CAT_PARSER_H
#define CAT_PARSER_H

#include "../cat_lib/cat.h"
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart2;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void CatParserTask(void const * argument);

#endif /* CAT_PARSER_H */