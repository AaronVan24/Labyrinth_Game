/*
 * ApplicationCode.h
 *
 *  Created on: Apr 3, 2024
 *      Author: aaronvan
 */
#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#include "LCD_Driver.h"
#include "Gyro_Driver.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define USER_PIN GPIO_PIN_0
#define USER_PORT GPIOA

#define RED_PIN GPIO_PIN_14
#define BOTH_LED_PORT GPIOG
#define GREEN_PIN GPIO_PIN_13

#define GREEN_LED 0
#define RED_LED 1

#define BUTTON_PRESSED 1
#define BUTTON_NOT_PRESSED 0

#define LED_OFF 0
#define LED_ON 1

#define CLOCKWISE_MAX 16000
#define CLOCKWISE_BT 11000
#define CLOCKWISE_MIN 6000
#define COUNTERCLOCKWISE_MAX -16000
#define COUNTERCLOCKWISE_BT -11000
#define COUNTERCLOCKWISE_MIN -6000

#define SIZE_OF_ARRAY 150


#define EVENT_FLAG_BUTTON 1
#define MAX_COUNT 2
#define INIT_COUNT 0
#define MSG_COUNT 15

#define MAZE_WIDTH  10
#define MAZE_HEIGHT 14
#define CELL_WIDTH  22
#define CELL_HEIGHT 22
#define WALL_COLOR  0x0000
#define PATH_COLOR  0xFFFF


typedef enum{
	CLOCKWISE_FAST,
	CLOCKWISE_SLOW,
	ZERO,
	COUNTER_CLOCKWISE_FAST,
	COUNTER_CLOCKWISE_SLOW,

}Rotation_Gyro;

void ApplicationInit(void);

typedef enum {
	physics_update_flag = 1,
	button_update_flag =2,
	game_event_flag1 =4,
	game_event_flag2=8
}flag_state;

typedef struct {
    int x;
    int y;
} Point;

void physicsTask();
void buttonDisruptTask();
void gameMonitorTask();
void LCD_Display_Task();
void LED_output_task();
void timers_callback();

void draw_maze();

void passGame();
void startScreen();
void gameOver();

bool path(int x, int y);

int distance(int x1, int y1, int x2, int y2);

#endif /* INC_APPLICATIONCODE_H_ */
