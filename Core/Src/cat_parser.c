#include "cat_parser.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "cmsis_os.h"
// #include "fw_version.h"

#include "board.h"

// osThreadId printHelpTaskHandle;

// void printHelpTask(void const *argument);

extern UART_HandleTypeDef huart2;

static uint8_t rx_byte;
static uint8_t rx_data[128];
static uint8_t rx_index = 0;

static int a = 0;

static bool quit_flag;

uint32_t reinit_time = 0;

static uint8_t recv_char;
static volatile uint8_t command_ready = 0;
static volatile uint8_t cmd_read_pos = 0;

void parser_buf_reset()
{
	rx_index = 0;
	memset(rx_data, 0, sizeof(rx_data));
	HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
}

static cat_return_state show_fw_version(const struct cat_command *cmd)
{
	char str[128];
	float ver = 0.01;
	memset(str, 0, sizeof(str));
    print_fw_ver(&huart2);
    return 0;
}

static cat_return_state test_run(const struct cat_command *cmd)
{
    parser_buf_reset();
    int i;
    for (i = 0; i < 5; i++)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
        osDelay(100);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
        osDelay(100);
    }
    
    return 0;
}

static cat_return_state print_cmd_list(const struct cat_command *cmd);

static struct cat_command cmds[] = {
        {
			.name = "+HELP",
			.run = print_cmd_list,
        },
        {
        	.name = "+SHOW_FW_VER",
			.run = show_fw_version,
        },
                {
        	.name = "#TEST",
			.run = test_run,
        }
};

static cat_return_state print_cmd_list(const struct cat_command *cmd)
{
	// osThreadDef(helpTask, printHelpTask, osPriorityNormal, 0, 128);
	// printHelpTaskHandle = osThreadCreate(osThread(helpTask), NULL);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    osDelay(1000);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);

	// char help_tsk_str[64];

    for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
    {
    	// sprintf((char *) help_tsk_str, "cmd[%d]: AT%s \r\n", i, cmds[i].name);
        // osDelay(100);
        char* help_tsk_str = pvPortMalloc(128*sizeof(char));
        // help_tsk_str = "cmd[0]: AT \r\n";
        memset(help_tsk_str, 0, sizeof(help_tsk_str));
        sprintf((char *) help_tsk_str, "cmd[%d]: AT%s \r\n", i, cmds[i].name);
        print_to_UART(help_tsk_str, &huart2);
        osDelay(100);
        vPortFree(help_tsk_str);
    }

    osDelay(1);

    parser_buf_reset();
    return 0;
}


static char buf[128];

static struct cat_command_group cmd_group = {
        .cmd = cmds,
        .cmd_num = sizeof(cmds) / sizeof(cmds[0]),
};

static struct cat_command_group *cmd_desc[] = {
        &cmd_group
};

static struct cat_descriptor desc = {
        .cmd_group = cmd_desc,
        .cmd_group_num = sizeof(cmd_desc) / sizeof(cmd_desc[0]),

        .buf = (uint8_t *) &buf[0],
        .buf_size = sizeof(buf)
};

static int write_char(char ch)
{
        // HAL_UART_Transmit(&huart2, (unsigned char*) &ch, 1, 1);
        return 1;
}

static int read_char(char *ch)
{
        if (command_ready) 
        {
        	char received = rx_data[cmd_read_pos];
            *ch = received;
            cmd_read_pos++;
            if (received == '\n') // '\r'
            {
                command_ready = 0;
                // return 0;
            }
            return 1;
        }
        return 0;
}

static struct cat_io_interface iface = {
        .read = read_char,
        .write = write_char
};

struct cat_object at;
void CatParserTask(void const * argument)
{
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

    // char dbg[10];
    // int ab;

    /* Launch AT terminal */
    cat_init(&at, &desc, &iface, NULL);
    while (1)
    {
        if(quit_flag) break;

            // a = checkHeapSpace();
            // if (a != ab)
            // {
            //     ab = a;
            //     sprintf(dbg,"%d\r\n", a);
            //     print_to_UART(dbg, &huart2); // debug var for storing free space of heap
            // }
            
        cat_service(&at);
    }
}


// void printHelpTask(void const *argument)
// {
//     // osDelay(1);
//     // vTaskDelete(NULL);
//     // BaseType_t status;
//     // status = xQueueSend(queue, (void*)&printHelpTaskHandle, portMAX_DELAY);
//     // vTaskDelete(NULL);
// }



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    if (huart->Instance == USART2) 
    { // Current UART
        recv_char = rx_byte;    // Add data to Rx_Buffer
        rx_data[rx_index++] = recv_char;

        if (rx_byte == '\n') { // изначально принимает \n
      //      rx_data[rx_index-1] == '\n';
     //       rx_byte = '\n';
            command_ready = 1;
            cmd_read_pos = 0;
            rx_index = 0;
         //   parser_buf_reset();
        }
        else
        {
            HAL_UART_Receive_IT(huart, &rx_byte, 1);
        }
    }
}



