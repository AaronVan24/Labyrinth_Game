/*
 * Gyro_Driver.h
 *
 *  Created on: Nov 22, 2023
 *      Author: Ryan
 */

#ifndef INC_GYRO_DRIVER_H_
#define INC_GYRO_DRIVER_H_

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "cmsis_os.h"

#define USE_MX_INIT 1

#define SENSITIVITY (float) 17.50f
//#define I3G425OD_SENSITIVITY_245DPS ((float)8.75f)
#define DT (float) 0.01f


//Gyro Command bits
#define GYRO_READ (1 << 7)
#define GYRO_WRITE (0 << 7)
#define MS_BIT (1<<6)


//Gyro Config Regs
#define WHO_AM_I_REG 0x0F
#define	CTRL_REG1 0x20
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define FIFO_CTRL_REG 0x2E




//Gyro Data Regs
//#define OUT_TEMP_REG 0x26
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
//#define OUT_Z_L 0x2C
//#define OUT_Z_H 0x2D


//CS Info
#define CS_PORT GPIOC
#define CS_PIN GPIO_PIN_1

#define TIMEOUT 20000 //20000 ms = 20s

#define NUM_SAMPLES 10


void Gyro_Init();
void Gyro_Power_On();
void Gyro_Reboot();
int16_t Gyro_Get_Velocity_raw();
int16_t Gyro_Get_x_raw();
void Gyro_Config_Regs();
void Gyro_HAL_Check();
void Gyro_Slave_Enable();
void Gyro_Slave_Disable();

void Gyro_Receive_Data(uint8_t *rx_buff);
void Gyro_Transmit_Data(uint8_t *tx_buff);

int16_t Gyro_Get_x();
int16_t Gyro_Get_Velocity();

void calibrate_gyro();
float filter_x(float x);
float filter_y(float y);



#endif /* INC_GYRO_DRIVER_H_ */
