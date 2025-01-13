/*
 * Copyright(c) 2021 - Jim Newman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/***********************************************************************/
/*                                                                     */
/* HAL -- These are the callback functions required by the InvenSense  */
/*        IMC-20948 driver.                                            */
/*                                                                     */
/***********************************************************************/

// #include "nrf_delay.h"
// #include "nrf_drv_rtc.h"
#include <zephyr/kernel.h>
#include "imu.h"
// #include "ens210.h"
#include "hal.h"
// #include "i2c.h"
#include <zephyr/drivers/i2c.h>
#include <nrfx.h>
#include <time.h>


//static bool verbose = false;	// For debugging I2C issues.

//const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

//uint32_t inv_icm20948_get_time_us(void)
//{
//    return nrf_drv_rtc_counter_get(&rtc);
//}



/*int inv_icm20948_i2c_read_reg(uint8_t reg, uint8_t *value)
{
    if (verbose)
        printf("Executing %s(0x%02x)\r\n", __func__, reg);

    *value = twi_read_register(IMU_ADDR, reg);

    if (verbose)
    {
        printf("0x%02x\r\n", *value);
    }

    return 0;
}

int inv_icm20948_i2c_read_reg_block(uint8_t reg, uint8_t * rbuffer, uint32_t rlen)
{
    int i;

    if (verbose)
        printf("Executing %s(0x%02x, %ld)\r\n", __func__, reg, rlen);

    twi_read_register_block(IMU_ADDR, reg, rbuffer, rlen);

    if (verbose)
    {
        for (i = 0; i < rlen; i++)
            printf(" val[%d]:0x%02x%s", i, rbuffer[i], (i%4==3?"\r\n":", "));
        printf("\r\n");
    }

    return 0;
}

int inv_icm20948_i2c_write_reg(uint8_t reg, uint8_t value)
{
    if (verbose)
    {
        printf("Executing %s(0x%02x, 0x%02x)\r\n", __func__, reg, value);
    }

    twi_write_register(IMU_ADDR, reg, value);

    return 0;
}

int inv_icm20948_i2c_write_reg_block(uint8_t reg, uint8_t *wbuffer, uint32_t wlen)
{
    int i;

    if (verbose)
    {
        printf("Executing %s(0x%02x, 0x%02x, %ld)", __func__, reg, wbuffer[0], wlen);
        if (wlen > 1)
        {
            printf("\r\n");
            for (i = 0; i < wlen; i++)
                printf(" val[%d]:0x%02x%s", i, wbuffer[i], (i%4==3?"\r\n":((i==(wlen-1)?"\r\n":","))));
        }
    }

    twi_write_register_block(IMU_ADDR, reg, wbuffer, wlen);

    return 0;
}*/




/* I2C 设备定义 */
#define I2C_NODE DT_NODELABEL(icm20948)
static const struct i2c_dt_spec dev_icm20948 = I2C_DT_SPEC_GET(I2C_NODE);

/* 获取系统时间（微秒） */
uint32_t inv_icm20948_get_time_us(void)
{
    return k_uptime_get() * 1000;  // 转换毫秒到微秒
}

/* 微秒延时 */
void inv_icm20948_sleep_us(uint32_t us)
{
    k_usleep(us);
}

/* I2C 初始化 */
int inv_icm20948_i2c_init(void)
{
    /* 检查 I2C 设备是否就绪 */
    if (!i2c_is_ready_dt(&dev_icm20948)) {
        return -ENODEV;
    }

    /* 配置 I2C 为快速模式（400kHz） */
    int ret = i2c_configure(dev_icm20948.bus, I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_CONTROLLER);
    if (ret != 0) {
        return ret;
    }

    return 0;
}

/* 读取单个寄存器 */
int inv_icm20948_i2c_read_reg(uint8_t reg, uint8_t *value)
{
    return i2c_reg_read_byte_dt(&dev_icm20948, reg, value);
}

/* 读取多个寄存器 */
int inv_icm20948_i2c_read_reg_block(uint8_t reg, uint8_t *buffer, uint32_t len)
{
    struct i2c_msg msgs[2];

    /* 设置寄存器地址 */
    msgs[0].buf = &reg;
    msgs[0].len = 1;
    msgs[0].flags = I2C_MSG_WRITE;

    /* 读取数据 */
    msgs[1].buf = buffer;
    msgs[1].len = len;
    msgs[1].flags = I2C_MSG_READ | I2C_MSG_RESTART;

    return i2c_transfer_dt(&dev_icm20948, msgs, 2);
}

/* 写入单个寄存器 */
int inv_icm20948_i2c_write_reg(uint8_t reg, uint8_t value)
{
    return i2c_reg_write_byte_dt(&dev_icm20948, reg, value);
}

/* 写入多个寄存器 */
int inv_icm20948_i2c_write_reg_block(uint8_t reg, uint8_t *buffer, uint32_t len)
{
    uint8_t tmp_buffer[len + 1];
    struct i2c_msg msg;

    /* 组合寄存器地址和数据 */
    tmp_buffer[0] = reg;
    memcpy(&tmp_buffer[1], buffer, len);

    msg.buf = tmp_buffer;
    msg.len = len + 1;
    msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

    return i2c_transfer_dt(&dev_icm20948, &msg, 1);
}
