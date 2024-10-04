#include "board.h"
#include "i2c_func.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "stm32f100xb.h"

#define DS3231_ADDRESS 0x68 //<< 1   // Адрес DS3231 (с учетом смещения)

I2C_HandleTypeDef hi2c1;

static uint8_t hours, minutes, seconds;

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

void I2C_GetTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    HAL_I2C_Master_Transmit(&hi2c1, DS3231_ADDRESS << 1, 0x00, 1, HAL_MAX_DELAY);
    uint8_t data[3];

    HAL_I2C_Master_Receive(&hi2c1, DS3231_ADDRESS << 1, data, sizeof(data), HAL_MAX_DELAY);

    *seconds = data[0];
    *minutes = data[1];
    *hours = data[2];

    // uint8_t data[7];

    // // Чтение времени из регистров DS3231 (секунды, минуты, часы)
    // HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS << 1, 0x00, I2C_MEMADD_SIZE_8BIT, data, 7, HAL_MAX_DELAY);

    // // Преобразование из BCD
    // *seconds = (data[0] & 0x0F) + ((data[0] >> 4) & 0x07) * 10;
    // *minutes = (data[1] & 0x0F) + ((data[1] >> 4) & 0x07) * 10;
    // *hours = (data[2] & 0x0F) + ((data[2] >> 4) & 0x01) * 10; // Учитываем 24-часовой формат

}

void I2C_SetTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    uint8_t data[4];

    data[0] = 0x00;
    data[1] = *seconds;
    data[2] = *minutes;
    data[3] = *hours;

    HAL_I2C_Master_Transmit(&hi2c1, DS3231_ADDRESS << 1, data, sizeof(data), HAL_MAX_DELAY);

}

uint8_t dec_to_bcd(uint8_t num)
{
    // return ((bcd / 10) << 4) | (bcd % 10);
    uint8_t bcd = 0;
    uint8_t shift = 0;

    while (num > 0)
    {
        uint8_t digit = num % 10;
        bcd |= (digit << shift);
        shift += 4;
        num /= 10;
    }
    return bcd;
    
}

uint8_t bcd_to_dec(uint8_t bcd)
{
    // return (bcd / 16) * 10 + (bcd % 16);
    uint8_t dec = 0;
    dec += (bcd >> 4) * 10;
    dec += (bcd & 0x0F);
    return dec;
}

static cat_return_state i2c_set_time(const struct cat_command *cmd,
    const uint8_t *data, const size_t data_size, const size_t args_num)
{
	print_to_UART("i2c set test in\r\n", &huart2);
    MX_I2C1_Init();
    osDelay(100);

    hours = dec_to_bcd(hours);
    minutes = dec_to_bcd(minutes);
    seconds = dec_to_bcd(seconds);
    I2C_SetTime(&hours, &minutes, &seconds);

    osDelay(100);
    I2C_GetTime(&hours, &minutes, &seconds);
    hours = bcd_to_dec(hours);
    minutes = bcd_to_dec(minutes);
    seconds = bcd_to_dec(seconds);
    char* text = pvPortMalloc(30*sizeof(char));
    snprintf(text, 30, "Time: %02u:%02u:%02u\r\n", hours, minutes, seconds);
    osDelay(100);
    print_to_UART(text, &huart2);
    vPortFree(text);

    HAL_I2C_DeInit(&hi2c1);
    print_to_UART("i2c set test end\r\n", &huart2);

    osDelay(1);
    parser_buf_reset();
    return 0;
}

cat_return_state i2c_get_time(const struct cat_command *cmd)
{
	print_to_UART("i2c get test in\r\n", &huart2);
    // uint8_t hours, minutes, seconds;
    MX_I2C1_Init();
    // while(1)
    // {

    osDelay(100);
    I2C_GetTime(&hours, &minutes, &seconds);
    seconds = bcd_to_dec(seconds & 0x7F);
    minutes = bcd_to_dec(minutes & 0x7F);
    hours = bcd_to_dec(hours & 0x7F);

    char* text = pvPortMalloc(30*sizeof(char));
    snprintf(text, 30, "Time: %02u:%02u:%02u\r\n", hours, minutes, seconds);
    osDelay(100);
    print_to_UART(text, &huart2);
    vPortFree(text);
    // }
    HAL_I2C_DeInit(&hi2c1);
    print_to_UART("i2c get test end\r\n", &huart2);
    osDelay(1);
    parser_buf_reset();
    return 0;
}

static struct cat_variable time_set_vars[]={
    {
        .type = CAT_VAR_UINT_DEC,
        .data = &hours,
        .data_size = sizeof(hours),
    },
    {
        .type = CAT_VAR_UINT_DEC,
        .data = &minutes,
        .data_size = sizeof(minutes),
    },
    {
        .type = CAT_VAR_UINT_DEC,
        .data = &seconds,
        .data_size = sizeof(seconds),
    }
};

static struct cat_command i2c_cmd[] = {
        {
            .name = "+I2C_GET_TIME",
            .run = i2c_get_time,
        },
        {
            .name = "+I2C_SET_TIME",
            .run = i2c_set_time,
            .var = time_set_vars,
            .var_num = DIM(time_set_vars),
            .need_all_vars = true,
            .write = i2c_set_time
        },
};

struct cat_command_group i2c_group = {
    .cmd = i2c_cmd,
    .cmd_num = DIM(i2c_cmd)
};
