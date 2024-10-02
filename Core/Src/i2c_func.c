#include "board.h"
#include "i2c_func.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "stm32f100xb.h"

#define DS3231_ADDRESS 0x68 //<< 1   // Адрес DS3231 (с учетом смещения)

I2C_HandleTypeDef hi2c1;

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
   
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

uint8_t bcd_to_dec(uint8_t bcd)
{
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}


void I2C_GetTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    uint8_t data[7];

    // Чтение времени из регистров DS3231 (секунды, минуты, часы)
    HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS << 1, 0x00, I2C_MEMADD_SIZE_8BIT, data, 7, HAL_MAX_DELAY);

    // Преобразование из BCD
    // *seconds = (data[0] & 0x0F) + ((data[0] >> 4) & 0x07) * 10;
    // *minutes = (data[1] & 0x0F) + ((data[1] >> 4) & 0x07) * 10;
    // *hours = (data[2] & 0x0F) + ((data[2] >> 4) & 0x01) * 10; // Учитываем 24-часовой формат

    *seconds = bcd_to_dec(data[0]);
    *minutes = bcd_to_dec(data[1]);
    *hours = bcd_to_dec(data[2]);
}



void i2cGetTimeTask(void const * argument)
{
    print_to_UART("i2c test in\r\n", &huart2);
    uint8_t hours, minutes, seconds;

    MX_I2C1_Init();
    osDelay(100);
    I2C_GetTime(&hours, &minutes, &seconds);
    // osDelay(100);

    // char* text[80];
    // sprintf(text, "Time: %u.%u.%u\r\n", hours, minutes, seconds);
    // osDelay(100);
    // print_to_UART(text, &huart2);

    HAL_I2C_DeInit(&hi2c1);
    
    // osDelay(1);
    // BaseType_t status;
    // status = xQueueSend(queue, (void*)&i2cDefaultTaskHandle, portMAX_DELAY);
    // vTaskDelete(NULL);
}