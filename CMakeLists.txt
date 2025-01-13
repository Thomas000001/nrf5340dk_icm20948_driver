# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mysensor_icm20948)

# 添加源文件
target_sources(app PRIVATE 
    src/main.c
    src/imu.c
    src/hal.c
)

# 添加头文件路径
# target_include_directories(app PRIVATE
#     src
#     ${ZEPHYR_BASE}/include
# )


# 添加包含目录
target_include_directories(app PRIVATE
    src
    ${ZEPHYR_BASE}/include
    ${ZEPHYR_BASE}/drivers
)

zephyr_library_include_directories(${ZEPHYR_BASE}/include)