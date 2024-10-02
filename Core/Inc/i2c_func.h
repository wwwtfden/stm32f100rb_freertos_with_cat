#ifndef INC_I2C_FUNC_H_
#define INC_I2C_FUNC_H_

extern osThreadId i2cDefaultTaskHandle;

static void MX_I2C1_Init(void);

void i2cGetTimeTask(void const * argument);

#endif