/*
 * Gyro_Driver.c
 *
 *  Created on: Jan 23, 2024
 *      Author: Ryan
 */

#include <Gyro_Driver.h>
#include <stdlib.h>



extern SPI_HandleTypeDef hspi5;
static HAL_StatusTypeDef HAL_Status;

float x_data;
float y_data;

/**
  * @brief Initialise the gyro for Lab2
  * @retval None
  */
void Gyro_Init(){
	Gyro_Power_On();
	osDelay(100);
	Gyro_Config_Regs();
	Gyro_Reboot();


}

/**
  * @brief Power on the Gyro
  * @retval None
  */

void Gyro_Power_On(){
	//get data from register
	uint8_t cmd = (GYRO_READ | CTRL_REG1);
	uint8_t rx_buff = 0;
	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&cmd);
	Gyro_Receive_Data(&rx_buff);
	Gyro_Slave_Disable();
	cmd = (GYRO_WRITE | CTRL_REG1);
	uint8_t tx_buff = (rx_buff | (1 << 3));
	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&cmd);
	Gyro_Transmit_Data(&tx_buff);
	Gyro_Slave_Disable();


	if(HAL_Status != HAL_OK)
	{
		while(1);
	}
	}


/**
  * @brief Perform a Reboot on the Gyro
  * @retval None
  */

void Gyro_Reboot(){
	uint8_t cmd = (GYRO_READ | CTRL_REG5);
	uint8_t rx_buff = 0;
	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&cmd);
	Gyro_Receive_Data(&rx_buff);
	Gyro_Slave_Disable();
	cmd = (GYRO_WRITE | CTRL_REG5);
	uint8_t tx_buff = (rx_buff | (1 << 7));
	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&cmd);
	Gyro_Transmit_Data(&tx_buff);
	Gyro_Slave_Disable();

}

/**
  * @brief Sample the Velocity Rate of the Gyro and return
  * @retval 16 bit integer representing the velocity of the gyro
  */

int16_t Gyro_Get_Velocity_raw(){
	int16_t yval = 0;

	uint8_t cmd = (GYRO_READ| MS_BIT | OUT_Y_L);
	uint8_t rxbuff_yl = 0;
	uint8_t rxbuff_yh = 0;

	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&cmd);
	Gyro_Receive_Data(&rxbuff_yl);
	Gyro_Receive_Data(&rxbuff_yh);
	Gyro_Slave_Disable();


	yval = (int16_t) ((rxbuff_yh << 8) | rxbuff_yl);
	return yval;
}

int16_t Gyro_Get_x_raw(){
	int16_t xval = 0;

	uint8_t cmd = (GYRO_READ| MS_BIT | OUT_X_L);
	uint8_t rxbuff_xl = 0;
	uint8_t rxbuff_xh = 0;

	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&cmd);
	Gyro_Receive_Data(&rxbuff_xl);
	Gyro_Receive_Data(&rxbuff_xh);
	Gyro_Slave_Disable();


	xval = (int16_t) ((rxbuff_xh << 8) | rxbuff_xl);
	return xval;
}

int16_t Gyro_Get_x(){
	float xval = Gyro_Get_x_raw();
	float x_val = (float)xval * SENSITIVITY;
	x_data = x_data + (x_val * DT * 0.005);
	return (int16_t)x_data;
}

int16_t Gyro_Get_Velocity(){
    float yval = Gyro_Get_Velocity_raw();
    float y_val = (float)yval * SENSITIVITY;
    y_data = y_data + (y_val * DT * 0.005);
    return (int16_t)y_data;
}

/**
  * @brief Configure the registers for the gyro
  * @retval None
  */
void Gyro_Config_Regs(){

	//CR1
	uint8_t addr_cmd = (GYRO_WRITE | CTRL_REG1);
	uint8_t reg_cmd = 0x1B; // 0001 1010 - Gyro Data rate is 100hz, gyro enabled on y-axis only
	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&addr_cmd);
	Gyro_Transmit_Data(&reg_cmd);
	Gyro_Slave_Disable();




	//CR4
	addr_cmd = (GYRO_WRITE | CTRL_REG4);
	reg_cmd = 0x10; //0001 0000 - Full scale rate of 500dps
	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&addr_cmd);
	Gyro_Transmit_Data(&reg_cmd);
	Gyro_Slave_Disable();


	//CR5
	addr_cmd = (GYRO_WRITE | CTRL_REG5);
	reg_cmd = 0xC0; //1100 0000 - Reboot initially and enable FIFO
	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&addr_cmd);
	Gyro_Transmit_Data(&reg_cmd);
	Gyro_Slave_Disable();


	//FIFO_CTRL
	addr_cmd = (GYRO_WRITE | FIFO_CTRL_REG);
	reg_cmd = 0x00; //0100 0000 FIFO in stream mode
	Gyro_Slave_Enable();
	while(HAL_GPIO_ReadPin(CS_PORT, CS_PIN) != GPIO_PIN_RESET);
	Gyro_Transmit_Data(&addr_cmd);
	Gyro_Transmit_Data(&reg_cmd);
	Gyro_Slave_Disable();




}

/**
  * @brief Check the status of the HAL
  * @retval None
  */
void Gyro_HAL_Check(){
	if(HAL_Status != HAL_OK){
		while(1);
	}
}

/**
  * @brief Enable the Chip Select pin
  * @retval None
  */
void Gyro_Slave_Enable(){
	HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_RESET);
}

/**
  * @brief Disable the Chip Select pin
  * @retval None
  */
void Gyro_Slave_Disable(){
	HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_SET);
}


/**
  * @brief Recieve one bit of data from the Gyro
  * @param rx_buff- pointer to a data buffer, recieved data will appear here
  * @retval None
  */
void Gyro_Receive_Data(uint8_t *rx_buff){

	HAL_Status = HAL_SPI_Receive(&hspi5, rx_buff, 1, TIMEOUT);
	Gyro_HAL_Check();

}

/**
  * @brief Send one bit of data to the Gyro
  * @param tx_buff- pointer to a data buffer.
  * @retval None
  */
void Gyro_Transmit_Data(uint8_t *tx_buff){

	HAL_Status = HAL_SPI_Transmit(&hspi5,tx_buff, 1, TIMEOUT);
	Gyro_HAL_Check();
}
