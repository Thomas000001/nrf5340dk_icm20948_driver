// // /*
// //  * Copyright (c) 2012-2014 Wind River Systems, Inc.
// //  *
// //  * SPDX-License-Identifier: Apache-2.0
// //  */

// #include <zephyr/drivers/i2c.h>
// #include <zephyr/sys/printk.h>
// #include <zephyr/kernel.h>
// #include "imu.h"
// #include "hal.h"

// // 定义 I2C 节点和设备
// // #define I2C_NODE DT_NODELABEL(mysensor)
// #define I2C_NODE DT_NODELABEL(icm20948)
// static const struct i2c_dt_spec dev_icm20948 = I2C_DT_SPEC_GET(I2C_NODE);

// // 定义 ICM20948 和 AK09916 的寄存器地址和配置值
// #define IMU_REG_BANK_SEL       0x7F
// #define AK09916_I2C_ADDR       0x0C
// #define AK09916_HXL            0x11
// #define AK09916_CNTL2          0x31
// #define AK09916_CNTL3          0x32
// #define AK09916_CONT_MODE_100HZ 0x08
// #define AK09916_ST1            0x10

// // 向 ICM20948 的寄存器写入数据的函数
// static int icm20948_write_register(uint8_t reg, uint8_t value) {
//     uint8_t buffer[2] = { reg, value };
//     return i2c_write_dt(&dev_icm20948, buffer, sizeof(buffer));
// }

// // 从 ICM20948 的寄存器读取数据的函数
// static int icm20948_read_register(uint8_t reg, uint8_t *data, size_t len) {
//     return i2c_write_read_dt(&dev_icm20948, &reg, 1, data, len);
// }

// // 选择 ICM20948 寄存器组的函数
// static int select_register_bank(uint8_t bank) {
//     return icm20948_write_register(IMU_REG_BANK_SEL, bank << 4);
// }
















// // 验证 AK09916 工作模式
// int verify_ak09916_mode(void) {
//     int ret;
//     uint8_t i2c_mst_status;
    
//     // 1. 先对 AK09916 进行软复位
//     ret = select_register_bank(3);
//     if (ret < 0) return ret;
    
//     // 配置写入 CNTL3 寄存器进行复位
//     ret = icm20948_write_register(0x03, AK09916_I2C_ADDR << 1);    // 写模式
//     if (ret < 0) return ret;
    
//     ret = icm20948_write_register(0x04, AK09916_CNTL3);           // CNTL3 寄存器
//     if (ret < 0) return ret;
    
//     ret = icm20948_write_register(0x06, 0x01);                     // 软复位值
//     if (ret < 0) return ret;
    
//     // ret = icm20948_write_register(0x05, 0x81);                     // 启用写入
//     // if (ret < 0) return ret;
    
//     printk("AK09916 soft reset initiated\n");
//     k_sleep(K_MSEC(100));  // 等待复位完成
//     ret = icm20948_write_register(0x03, (AK09916_I2C_ADDR << 1) | 0x80);  // 读模式
//     if (ret < 0) return ret;

//     ret = icm20948_write_register(0x04, AK09916_CNTL3);           // CNTL3 寄存器
//     if (ret < 0) return ret;

//     ret = icm20948_write_register(0x05, 0x81);                            // 启用读取
//     if (ret < 0) return ret;
    
//     ret = select_register_bank(0);
//     uint8_t cntl3;
//     ret = icm20948_read_register(0x3B, &cntl3, 1);
//     printk("After soft reset - cntl3: 0x%02X\n", cntl3);

//     // 检查传输状态
//     ret = select_register_bank(0);
//     ret = icm20948_read_register(0x17, &i2c_mst_status, 1);
//     printk("Soft reset - I2C_MST_STATUS: 0x%02X\n", i2c_mst_status);
    
//     // 2. 设置连续模式
//     ret = select_register_bank(3);
//     if (ret < 0) return ret;
    
//     ret = icm20948_write_register(0x03, AK09916_I2C_ADDR << 1);    // 写模式
//     if (ret < 0) return ret;
    
//     ret = icm20948_write_register(0x04, AK09916_CNTL2);           // CNTL2 寄存器
//     if (ret < 0) return ret;
    
//     ret = icm20948_write_register(0x06, AK09916_CONT_MODE_100HZ); // 连续模式
//     if (ret < 0) return ret;
    
//     ret = icm20948_write_register(0x05, 0x81);                     // 启用写入
//     if (ret < 0) return ret;
    
//     printk("Set continuous mode completed\n");
//     k_sleep(K_MSEC(100));  // 等待模式切换完成
    
//     // 3. 验证模式设置
//     ret = icm20948_write_register(0x03, (AK09916_I2C_ADDR << 1) | 0x80);  // 读模式
//     if (ret < 0) return ret;
    
//     ret = icm20948_write_register(0x04, AK09916_CNTL2);                   // CNTL2 寄存器
//     if (ret < 0) return ret;
    
//     ret = icm20948_write_register(0x05, 0x81);                            // 启用读取
//     if (ret < 0) return ret;

//     ret = select_register_bank(0);

    
//     k_sleep(K_MSEC(10));
    
//     // 检查传输状态
//     ret = select_register_bank(0);
//     if (ret < 0) return ret;
    
//     ret = icm20948_read_register(0x17, &i2c_mst_status, 1);
//     printk("Mode verify - I2C_MST_STATUS: 0x%02X\n", i2c_mst_status);
    
//     uint8_t mode;
//     ret = icm20948_read_register(0x3B, &mode, 1);
//     if (ret < 0) return ret;
    
//     printk("AK09916 mode after setup: 0x%02X\n", mode);
    
//     if (mode != AK09916_CONT_MODE_100HZ) {
//         printk("Failed to set AK09916 mode\n");
//         return -EIO;
//     }
    
//     printk("AK09916 mode setup successful\n");

//     //确认data是否都已更新且准备好
//     uint8_t int_status_1;
//     ret = icm20948_read_register(0x3B, &int_status_1, 1);
//     printk("Data verify - INT_STATUS_1: 0x%02X\n", int_status_1);

//     return 0;
// }

// int init_ak09916(void) {
//     int ret;
    
//     // 1. 复位 ICM20948
//     ret = select_register_bank(0);
//     if (ret < 0) return ret;

//     ret = icm20948_write_register(0x03, 0x00);//先禁用i2c master
//     if (ret < 0) return ret;
//     k_sleep(K_MSEC(100));
    
//     ret = icm20948_write_register(0x06, 0x80);  // PWR_MGMT_1 复位
//     if (ret < 0) return ret;
//     k_sleep(K_MSEC(100));
    
//     // 清除睡眠模式
//     ret = icm20948_write_register(0x06, 0x01);
//     if (ret < 0) return ret;
//     k_sleep(K_MSEC(100));
    
//     // 2. 禁用 bypass
//     ret = icm20948_write_register(0x0F, 0x00);
//     if (ret < 0) return ret;
//     k_sleep(K_MSEC(10));

//     //为了避免当接收到nack时直接中断
//     ret = icm20948_write_register(0x10, 0x00);  //禁用INT_ENABLE
    
//     // 3. 配置并启用 I2C 主机
//     ret = select_register_bank(3);
//     if (ret < 0) return ret;
    
//     // 配置 I2C 主机时钟为 100kHz
//     ret = icm20948_write_register(0x01, 0x08);
//     if (ret < 0) return ret;
    
//     ret = select_register_bank(0);
//     if (ret < 0) return ret;
    
//     // 启用 I2C 主机
//     ret = icm20948_write_register(0x03, 0x20);
//     if (ret < 0) return ret;
//     k_sleep(K_MSEC(100));
    
//     // 4. 验证 AK09916 模式
//     ret = verify_ak09916_mode();
//     if (ret < 0) {
//         printk("Failed to verify AK09916 mode\n");
//         return ret;
//     }
    
//     return 0;
// }









// // 读取 ST1 寄存器的数据函数
// int read_ak09916_st1(uint8_t *st1_val) {
//     int ret;
    
//     // 先验证工作模式
//     ret = verify_ak09916_mode();
//     if (ret < 0) {
//         return ret;
//     }

//     // 读取前的状态
//     ret = select_register_bank(0);
//     uint8_t status_check;
//     ret = icm20948_read_register(0x3B, &status_check, 1);
//     printk("Before ST1 read - EXT_SENS_DATA_00: 0x%02X\n", status_check);
    
//     ret = select_register_bank(3);
//     // 检查当前 SLV0 配置
//     uint8_t slv0_addr, slv0_reg, slv0_ctrl;
//     ret = icm20948_read_register(0x03, &slv0_addr, 1);
//     ret = icm20948_read_register(0x04, &slv0_reg, 1);
//     ret = icm20948_read_register(0x05, &slv0_ctrl, 1);
//     printk("ST1 read config - ADDR: 0x%02X, REG: 0x%02X, CTRL: 0x%02X\n", 
//            slv0_addr, slv0_reg, slv0_ctrl);
    
//     // 配置读取 ST1
//     ret = icm20948_write_register(0x03, (AK09916_I2C_ADDR << 1) | 0x80); // 读操作
//     ret = icm20948_write_register(0x04, AK09916_ST1);                     // ST1 寄存器
//     ret = icm20948_write_register(0x05, 0x81);                            // 启用读取 1 字节
    
//     // 检查传输状态
//     ret = select_register_bank(0);
//     uint8_t i2c_mst_status;
//     ret = icm20948_read_register(0x17, &i2c_mst_status, 1);
//     printk("I2C_MST_STATUS during ST1 read: 0x%02X\n", i2c_mst_status);
    
//     // 读取 ST1 值
//     ret = icm20948_read_register(0x3B, st1_val, 1);
//     printk("ST1 raw value: 0x%02X, DRDY bit: %d\n", *st1_val, (*st1_val & 0x01));
    
//     return 0;
// }

// int read_ak09916_xyz(int16_t *x, int16_t *y, int16_t *z) {
//     // uint8_t st1_val;
//     uint8_t buffer[6];
//     int ret;
//     uint8_t st1_val;

//      // 读取 ST1 寄存器以检查数据是否准备好
//     ret = read_ak09916_st1(&st1_val);
//     if (ret < 0 || !(st1_val & 0x01)) { // DRDY 位为 1 表示数据准备好
//         // printk("ST1 data not ready or read failure, error: %d, ST1: 0x%02X\n", ret, st1_val);
//         return -EAGAIN; // 返回 EAGAIN 表示数据尚未准备好
//     }


//     // 1. 切换到 Bank 3，配置 I2C 主机
//     ret = select_register_bank(3);
//     if (ret < 0) {
//         printf("switch to bank3 failure\n");
//         return ret;
//     }
//     // 配置 I2C_SLV0_ADDR (AK09916 地址，读操作)
//     ret = icm20948_write_register(0x03, (AK09916_I2C_ADDR << 1) | 0x80); // 0x80 表示读
//     if (ret < 0) {
//         printf("configure I2C_SLV0_ADDR failure\n");
//         return ret;
//     }
//     // 配置 I2C_SLV0_REG (目标寄存器 HXL 地址)
//     ret = icm20948_write_register(0x04, AK09916_HXL);
//     if (ret < 0) {
//         printf("configure I2C_SLV0_REG failure\n");
//         return ret;
//     }
//     // 配置 I2C_SLV0_CTRL (启用读取并设置读取 6 字节)
//     ret = icm20948_write_register(0x05, 0x86); // 0x86 = 0x10000110，启用读取，长度 6 字节
//     if (ret < 0) {
//         printf("configure I2C_SLV0_CTRL failure\n");
//         return ret;
//     }
//     ret = select_register_bank(0);
//     if (ret < 0) {
//         printf("switch to bank0 failure\n");
//         return ret;
//     }
//     // 读取扩展传感器数据寄存器 (EXT_SLV_SENS_DATA_00 - 05)
//     ret = icm20948_read_register(0x3B, buffer, 6); // 0x3B 是 EXT_SLV_SENS_DATA_00 的地址，读取HXL-HZH的数据

//     // 3. 合并高低字节数据
//     *x = (buffer[1] << 8) | buffer[0];
//     *y = (buffer[3] << 8) | buffer[2];
//     *z = (buffer[5] << 8) | buffer[4];

//     return 0;
// }

// int main(void) {
//     int ret;
//     int16_t x, y, z;

//     // 检查 I2C 设备是否准备就绪
//     if (!device_is_ready(dev_icm20948.bus)) {
//         printk("I2C bus %s was not ready!\n", dev_icm20948.bus->name);
//         return -1;
//         k_sleep(K_MSEC(100)); 
//     }
//     printk("initialize AK09916 mag...\n");
//     // 配置 AK09916 为连续模式
//     ret = init_ak09916();
//     while (ret != 0) {
//         printk("init AK09916 failure\n");
//         ret = init_ak09916();
//         k_sleep(K_MSEC(100)); 
//     }

//     printk("start reading AK09916 mag data...\n");

//     // 主循环，持续读取数据
//     while (1) {
//         ret = read_ak09916_xyz(&x, &y, &z);
//         if (ret == -EAGAIN) {
//             // 等待一段时间后再试
//             k_sleep(K_MSEC(10));
//             continue;
//         } 
//         else if (ret < 0) {
//             printk("Error reading data: %d\n", ret);
//         } 
//         else {
//             printk("mag data - X: %d, Y: %d, Z: %d\n", x, y, z);
//         }
//         k_sleep(K_MSEC(100));  // 控制读取频率
//     }
// }
/* main.c - ICM20948 demo application for nRF5340 */

/* main.c - ICM20948 demo application for nRF5340 using provided drivers */

/* main.c - ICM20948 demo application for nRF5340 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include "imu.h"
#include "hal.h"

/* I2C 设备定义 */
#define I2C_NODE DT_NODELABEL(icm20948)
static const struct i2c_dt_spec dev_icm20948 = I2C_DT_SPEC_GET(I2C_NODE);

/* 传感器数据刷新时间 */
#define SENSOR_SAMPLE_INTERVAL K_MSEC(10)

/* 全局变量 */
static IMU_DATA imu_data;
extern inv_icm20948_state st;

int main(void)
{
    int ret;
    int16_t mx, my, mz;
    
    printk("Starting ICM20948 application\n");

    /* 检查 I2C 设备是否就绪 */
    if (!i2c_is_ready_dt(&dev_icm20948)) {
        printk("I2C bus %s is not ready!\n", dev_icm20948.bus->name);
        return -1;
    }

    /* 初始化 ICM20948 */
    ret = inv_check_and_setup_chip(&st);
    if (ret != 0) {
        printk("Failed to initialize ICM20948: %d\n", ret);
        return ret;
    }

    /* 初始化磁力计 */
    inv_icm20948_init_magnetometer();
    printk("ICM20948 initialized successfully\n");

    /* 设置传感器工作模式 */
    inv_icm20948_set_sample_frequency(100); // 100Hz
    inv_icm20948_set_gyro_dlpf(INV_ICM20948_GYRO_FILTER_197HZ);
    inv_icm20948_set_accel_dlpf(INV_ICM20948_ACCEL_FILTER_246HZ);

    /* 主循环 */
    while (1) {
        /* 读取传感器数据 */
        inv_icm20948_read_imu(&imu_data);
        
        /* 单独读取磁力计数据 */
        inv_icm20948_read_magn_xyz(&mx, &my, &mz);

        /* 打印传感器数据 */
        // printk("\n=== Sensor Data ===\n");
        // printk("Accel:  X: %6d Y: %6d Z: %6d\n", 
        //        imu_data.ax, imu_data.ay, imu_data.az);
        // printk("Gyro:   X: %6d Y: %6d Z: %6d\n", 
        //        imu_data.gx, imu_data.gy, imu_data.gz);
        // printk("Mag:    X: %6d Y: %6d Z: %6d\n", 
        //        mx, my, mz);
        printk("%6d\n", mz);
        // printk("Temp:   %d\n", imu_data.temperature);

        /* 等待下一次采样 */
        k_sleep(SENSOR_SAMPLE_INTERVAL);
    }



    return 0;
}