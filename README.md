this is a driver of nrf5340dk and icm20948(using nrf connect in vscode)
Steps:
1. put imu.c/hal.c/imu.h/hal.h into yourproject/src (where is the same place as main.c)
2. setting yourproject/nrf5340dk_nrf5340_cpuapp.overlay
3. setting yourproject/CMakeLists.txt
4. setting yourproject/prj.conf
5. setting yourproject/src/main.c
6. build your project and flash it to your board then you can read the data of accel/gyro/mag/temp
