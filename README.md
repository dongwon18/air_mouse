---
finished_date: 2022-05-04
tags:
    - STM32F103RB
    - Nucleo-F103RB
    - ARM_Cortex-M3
    - STM_CubeIDE
    - MPU6080
    - C
    - Windows
---
# Air Mouse
- implement air mouse with STM board and MPU6080 sensor(a 3-axis gyroscope, a 3-axis accelerometer and thermometer)
- with an internal tack switch and external tack switch, implement a mouse's buttons

## NOTICE
- *serial.h* used to show accuracy result is not implemented by myself.

## Nucleo 
- baud rate: 115200 bits/sec
### PIN setting
|GPIO pin|for|  
|:---:|:---:|
|PB8|I2C SCL|
|PB9|I2C SDA|
|PA2|UART TX|
|PA3|UART RX|
|PC13|left button|
|PB2|right button|

### source code
1. overwrite *_write* to transmit value using UART.
2. read MPU6080 sensor value
3. send sensor value to Windows desptop
4. sleep for 100 ms

## Windows app
### source code
1. UART connect with STM board
2. get data from the board(sensor value, buttons value)
3. parse the data and store in DATA data
4. set move_y, move_x according to the sensor value
5. move cursor
6. if button is clicked, generate click event
7. sleep for 100 ms

### user can change
- THRES_X/Y : if value is large, user should tilt the sensor more
- ONE_MOVE: user can change DPI with this value

### reference
- [Win32 API Docs](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mouse_event)

## File structure
- except *main.c*, files in Nucleo directory are generated automatically by CubeIDE
- for MPU6050, *sd_hal_mpu6050.h* and *sd_hal_mpu6050.c* are included
- *windows/serial.h* is written by Mincoding
```
|-- Nucleo
    |-- Inc
        |-- gpio.h
        |-- i2c.h
        |-- main.h
        |-- sd_hal_mpu6050.h
        |-- stm32f1xx_hal_conf.h
        |-- stm32f1xx_it.h
        |-- usart.h
    |-- Src
        |-- gpio.c
        |-- i2c.c
        |-- main.c
        |-- sd_hal_mpu6050.c
        |-- stm32flxx_hal_msp.c
        |-- stm32f1xx_it.c
        |-- syscalls.c
        |-- sysmem.c
        |-- system_stm32flxx.c
|-- windows
    |-- main.cpp
    |-- serial.h
|-- Nucleo.jpg
```

## Result

<p align="center">
    <img src="/Nucleo.jpg", alt="circult"><br/>
    Nucleo board
</p>

## 배운 점
- STMicroelectronics의 board를 라즈베리파이만큼 능숙하게 다룰 수 있게 되었다.
- UART 통신 library를 이용하여 board와 통신할 수 있다.
- I2C 통신을 이용하여 센서에서 board로 값을 읽어올 수 있다.

## 한계점
- 실제 마우스처럼 3D 모델링을 이용하여 케이스를 제작할 수 있을 것이다.
