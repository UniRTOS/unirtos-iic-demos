/*****************************************************************/ /**
* @file iic_demo.c
* @brief
* @author bronson.zhan@quectel.com
* @date 2025-05-08
*
* @copyright Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.
* All Rights Reserved. Quectel Wireless Solution Proprietary and Confidential.
*
* @par EDIT HISTORY FOR MODULE
* <table>
* <tr><th>Date <th>Version <th>Author <th>Description
* <tr><td>2025-05-08 <td>1.0 <td>Bronson.Zhan <td> Init
* </table>
**********************************************************************/
#include "qosa_sys.h"
#include "qosa_iic.h"
#include "qosa_gpio.h"
#include "qosa_def.h"
#include "qosa_log.h"
#include "iic_demo.h"
#include "qosa_pinctrl.h"
#include "unirtos_app_init_registry.h"
/*===========================================================================
 *  Macro Definition
 ===========================================================================*/
#define QOS_LOG_TAG LOG_TAG_DEMO

/*===========================================================================
 *  Variate
 ===========================================================================*/
static qosa_task_t g_unir_iic_demo_task = QOSA_NULL;

/*===========================================================================
 *  Static API Functions
 ===========================================================================*/
/**
 * @brief I2C Demo function for initializing I2C interface and performing read/write tests in a loop.
 *
 * This function first configures I2C pin functions, initializes the I2C channel in standard mode,
 * then enters a loop to continuously write data to the specified slave device's register and read back results,
 * used to verify if I2C communication is working properly.
 *
 * @param ctx Task context pointer, reserved parameter, unused
 * @return No return value
 */
static void unir_iic_demo_process(void *ctx)
{
    int           ret = 0;
    unsigned char wrtBuff[2] = {UNIR_IIC_DEMO_WRITE_VALUE, 0};
    unsigned char rdBuff[2] = {0};

    // Delay 3 seconds to prevent log loss
    qosa_task_sleep_ms(3000);

    // Configure I2C pin functions (SCL and SDA)
    qosa_pin_set_func(UNIR_EC800ZCNLC_I2C0_SCL_PIN, UNIR_EC800ZCNLC_I2C0_SCL_FUNC);
    qosa_pin_set_func(UNIR_EC800ZCNLC_I2C0_SDA_PIN, UNIR_EC800ZCNLC_I2C0_SDA_FUNC);

    QLOGV("I2C INIT !!!");
    // Initialize I2C channel in standard mode
    ret = qosa_i2c_init(UNIR_IIC_DEMO_CHANNEL, QOSA_IIC_STANDARD_MODE);
    if (ret != QOSA_I2C_SUCCESS)
    {
        QLOGD("I2C INIT FAILED[%d]", ret);
    }

    QLOGV("I2C DEMO TESTING...");
    // Loop to perform I2C read/write tests
    while (1)
    {
        // Write 2 bytes of data to the specified slave device's register address
        ret = qosa_i2c_write(UNIR_IIC_DEMO_CHANNEL, UNIR_IIC_DEMO_SLAVE_ADDR, UNIR_IIC_DEMO_REG_02, wrtBuff, 2);
        QLOGD("< write i2c value=0x%x, ret=%d >\n", UNIR_IIC_DEMO_WRITE_VALUE, ret);
        qosa_task_sleep_ms(15);
        // Read data from the specified slave device's register address
        ret = qosa_i2c_read(UNIR_IIC_DEMO_CHANNEL, UNIR_IIC_DEMO_SLAVE_ADDR, UNIR_IIC_DEMO_REG_02, rdBuff, 2);
        QLOGD("< read i2c ret=%d, value=0x%x%x>\n", ret, rdBuff[1], rdBuff[0]);

        // Delay 2 seconds after each complete read/write operation before next cycle
        qosa_task_sleep_ms(2000);
    }
}

/*===========================================================================
 *  Public API Functions
 ===========================================================================*/

void unir_iic_demo_init(void)
{
    QLOGV("enter UniRTOS IIC DEMO !!!");
    if (g_unir_iic_demo_task == QOSA_NULL)
    {
        qosa_task_create(
            &g_unir_iic_demo_task,
            CONFIG_UNIRTOS_IIC_DEMO_TASK_STACK_SIZE,
            UNIR_IIC_DEMO_TASK_PRIO,
            "iic_demo",
            unir_iic_demo_process,
            QOSA_NULL,
            1
        );
    }
}

UNIRTOS_APP_EXPORT(321, "iic_demo", unir_iic_demo_init);
