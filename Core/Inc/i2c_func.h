#ifndef INC_I2C_FUNC_H_
#define INC_I2C_FUNC_H_

// extern osThreadId i2cDefaultTaskHandle;

extern struct cat_command_group i2c_group;

static void MX_I2C1_Init(void);
// cat_return_state i2c_get_time(const struct cat_command *cmd);
// cat_return_state i2c_set_time(const struct cat_command *cmd);

// void i2cGetTimeTask(void const * argument);

#endif