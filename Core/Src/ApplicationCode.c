/*
 * ApplicationCode.c
 *
 *  Created on: Apr 3, 2024
 *      Author: aaronvan
 */


#include "ApplicationCode.h"
#include <stdbool.h>

static bool buttonPressed = false;

int16_t globalXPosition = 294;
int16_t globalYPosition = 143;

bool mazeDisplayed = false;

static StaticTimer_t tcb;
const osTimerAttr_t timers_attribute ={
		.name = "timey",
		.cb_mem = &(tcb),
		.cb_size = sizeof(tcb)
};

const osEventFlagsAttr_t event_attr1 ={
		.name = "event_name1"
};

const osEventFlagsAttr_t event_attr2 ={
		.name = "event_name2"
};

const osSemaphoreAttr_t sema_atrr ={
		.name = "sema"
};

const osMutexAttr_t mutexx_atrr1 ={
		.name = "mut1"
};

const osMutexAttr_t mutexx_atrr2 ={
		.name = "mut2"
};

static StaticTask_t tcb1;
static StaticTask_t tcb2;
static StaticTask_t tcb3;
static StaticTask_t tcb4;
static StaticTask_t tcb5;

static uint32_t stackTasks_physics[SIZE_OF_ARRAY];
static uint32_t stackTasks_lcd[SIZE_OF_ARRAY];
static uint32_t stackTasks_vehicleMon[SIZE_OF_ARRAY];
static uint32_t stackTasks_dir[SIZE_OF_ARRAY];
static uint32_t stackTasks_led[SIZE_OF_ARRAY];

int static_maze[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 1, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 0, 1, 1, 1, 1, 0, 0, 0, 1},
	{1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
	{1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 1, 0, 1, 0, 0, 0, 1},
	{1, 0, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 1}
};

const osThreadAttr_t task_Attr_physics ={
	.name = "phys1110",
	.cb_mem = &(tcb1),
	.cb_size = sizeof(tcb1),
	.stack_mem = &(stackTasks_physics),
	.stack_size = sizeof(stackTasks_physics),
	.priority = osPriorityNormal1
};

const osThreadAttr_t task_Attr_LCDdisplay ={
	.name = "lcd",
	.cb_mem = &(tcb2),
	.cb_size = sizeof(tcb2),
	.stack_mem = &(stackTasks_lcd),
	.stack_size = sizeof(stackTasks_lcd),
	.priority = osPriorityNormal1
};

const osThreadAttr_t task_Attr_gameMonitor ={
	.name = "gamee",
	.cb_mem = &(tcb3),
	.cb_size = sizeof(tcb3),
	.stack_mem = &(stackTasks_vehicleMon),
	.stack_size = sizeof(stackTasks_vehicleMon),
	.priority = osPriorityNormal1
};

const osThreadAttr_t task_Attr_buttonDisrupt ={
	.name = "buttonDis",
	.cb_mem = &(tcb4),
	.cb_size = sizeof(tcb4),
	.stack_mem = &(stackTasks_dir),
	.stack_size = sizeof(stackTasks_dir),
	.priority = osPriorityNormal1
};

const osThreadAttr_t task_Attr_LEDoutput ={
	.name = "led",
	.cb_mem = &(tcb5),
	.cb_size = sizeof(tcb5),
	.stack_mem = &(stackTasks_led),
	.stack_size = sizeof(stackTasks_led),
	.priority = osPriorityNormal1
};



static osEventFlagsId_t event_flag_id1;
static osEventFlagsId_t event_flag_id2;
static osSemaphoreId_t sema_id;
static osMutexId_t mut_id1;
static osMutexId_t mut_id2;
static osTimerId_t timeID;
static uint32_t tick = 100;

static osThreadId_t taskID_physics;
static osThreadId_t taskID_lcd;
//static osThreadId_t taskID_gameMonitor;
//static osThreadId_t taskID_buttonDisrupt;
static osThreadId_t taskID_led;


void ApplicationInit(void)
{
	LTCD__Init();
    LTCD_Layer_Init(0);
    Gyro_Init();

    event_flag_id1 = osEventFlagsNew(&event_attr1);
    event_flag_id2 = osEventFlagsNew(&event_attr2);
    sema_id = osSemaphoreNew(MAX_COUNT, INIT_COUNT, &(sema_atrr));
    mut_id1 = osMutexNew (&mutexx_atrr1);
    mut_id2 = osMutexNew (&mutexx_atrr2);

	timeID = osTimerNew(timers_callback,osTimerPeriodic,NULL,&timers_attribute);
	if(timeID == NULL){
		for(;;);
	}

	taskID_physics = osThreadNew(physicsTask, NULL, &task_Attr_physics);
	if(taskID_physics == NULL){
		for(;;);
	}

	taskID_lcd = osThreadNew(LCD_Display_Task, NULL, &task_Attr_LCDdisplay);
	if(taskID_lcd == NULL){
		for(;;);
	}

	taskID_led = osThreadNew(LED_output_task, NULL, &task_Attr_buttonDisrupt);
	if(taskID_led == NULL){
		for(;;);
	}
//	taskID_gameMonitor = osThreadNew(gameMonitorTask, NULL, &task_Attr_gameMonitor);
//	if(taskID_gameMonitor == NULL){
//		for(;;);
//	}

//	taskID_buttonDisrupt = osThreadNew(buttonDisruptTask, NULL, &task_Attr_buttonDisrupt);
//	if(taskID_buttonDisrupt == NULL){
//		for(;;);
//	}
//
	osTimerStart(timeID,tick);

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	osEventFlagsSet(event_flag_id1, 1);
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);

	osSemaphoreRelease(sema_id);
}

/**
  * @brief function to determine if the button is pressed
  * @param none
  * @retval if pressed the button this funciton will be used if called.
  */
GPIO_PinState button_state(){
	GPIO_PinState pressed = HAL_GPIO_ReadPin(USER_PORT, USER_PIN);
	return pressed;
}

void timers_callback(void *arg){
	(void)&arg;

	if(button_state()){
		if(buttonPressed == 10){
			buttonPressed = 0;
		}
	}
}

void buttonTimerCallback(void *argument){
	buttonPressed = true;
	osEventFlagsSet(event_flag_id2,2);
}

/**
  * @brief task that pull gyro data and display the movement of the ball
  * @param argument- void pointer
  * @retval None
  */
void physicsTask(void *argument) {
    (void)&argument;

    int16_t x_position = 294;
    int16_t y_position = 143;
    int16_t prev_x = x_position;
    int16_t prev_y = y_position;

    startScreen();
    LCD_Clear(0,LCD_COLOR_WHITE);

    for (;;) {
        osDelay(50);
        osEventFlagsSet(event_flag_id1, physics_update_flag);

/* WITHOUT COLLISIONS CODE */

		int16_t gyro_x = Gyro_Get_x();
		int16_t gyro_y = Gyro_Get_Velocity();
		x_position += gyro_x;
		y_position += gyro_y;

		// lcd screen boundries
		if (x_position < 5) x_position = 5;
		if (x_position > 314) x_position = 314;
		if (y_position < 5) y_position = 5;
		if (y_position > 234) y_position = 234;

		LCD_Draw_Circle_Fill(prev_y, prev_x, 4, LCD_COLOR_WHITE);
		LCD_Draw_Circle_Fill(y_position, x_position, 4, LCD_COLOR_RED);

		prev_x = x_position;
		prev_y = y_position;

/* END OF NO COLLISIONS CODE */

/* WITH COLLISIONS CODE */
//			int16_t gyro_x = Gyro_Get_x();
//			int16_t gyro_y = Gyro_Get_Velocity();
//
//			x_position += gyro_x;
//			y_position += gyro_y;
//
//	        int16_t new_x_position = x_position + gyro_x;
//	        int16_t new_y_position = y_position + gyro_y;
//
//	        if (new_x_position < 5) new_x_position = 5;
//	        if (new_x_position > 314) new_x_position = 314;
//	        if (new_y_position < 5) new_y_position = 5;
//	        if (new_y_position > 234) new_y_position = 234;
//
//
//	        int cell_x = new_x_position / CELL_WIDTH;
//	        int cell_y = new_y_position / CELL_HEIGHT;
//
//	        if (path(cell_x, cell_y)) {
//	            x_position = new_x_position;
//	            y_position = new_y_position;
//
//	            LCD_Draw_Circle_Fill(prev_y, prev_x, 4, LCD_COLOR_WHITE);
//	            LCD_Draw_Circle_Fill(y_position, x_position, 4, LCD_COLOR_RED);
//
//
//	            prev_x = x_position;
//	            prev_y = y_position;
//	        }

/* END OF COLLISIONS CODE */


		LCD_Draw_Circle_Fill(33, 12, 8, LCD_COLOR_GREY);
		if (distance(x_position, y_position, 33, 15) <= 24){
			LCD_Clear(0,LCD_COLOR_WHITE);
			passGame();
			break;
		}

		LCD_Draw_Circle_Fill(195, 150, 8, LCD_COLOR_BLUE);
		if (distance(x_position, y_position, 150, 195) <= 7){
			LCD_Clear(0,LCD_COLOR_WHITE);
			gameOver();
			break;
		}
    }
}

/**
  * @brief helper function used to detect collisions with points on the screen that would win you the game or lose you the game
  * @param x1- x-axis position of the gyro ball, y1- y-axis position of the gyro ball, x2- x-axis position of the stationary pin, y2- y-axis position of the stationary pin
  * @retval Using the built in sqrt function, I perform the Pythagorean theorem to find the distance between the gyro ball and the stationary pin.
  */
int distance(int x1, int y1, int x2, int y2){
    return (int)sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}


/**
  * @brief task to display the maze and the ball moving at the same time
  * @param none
  * @retval None
  */
void LCD_Display_Task(void *argument) {
    (void)&argument;

    uint32_t flags;
    for (;;) {
        flags = osEventFlagsGet(event_flag_id1);
        if(flags & physics_update_flag){
        	draw_maze();
        }
    }
}

/**
  * @brief function to draw maze given the 2D array and the cell height and width of each pixel of the walls.
  * @param none
  * @retval None
  */
void draw_maze() {
	uint16_t wall_color = LCD_COLOR_BLACK;
	uint16_t path_color = LCD_COLOR_WHITE;
	for (int y = 0; y < MAZE_HEIGHT; y++) {
		for (int x = 0; x < MAZE_WIDTH; x++) {
			uint16_t color = static_maze[y][x] == 1 ? wall_color : path_color;
			for (int py = 0; py < CELL_HEIGHT; py++) {  // Draw cell as 22x22
				for (int px = 0; px < CELL_WIDTH; px++) {
					LCD_Draw_Pixel(x * CELL_WIDTH + px, y * CELL_HEIGHT + py, color);
				}
			}
		}
	}
}

/**
  * @brief task to handle and run the led for both green and red and also implement when the button is pressed.
  * @param argument - void pointer
  * @retval None
  */
void LED_output_task(void *argument){
	(void)&argument;
	for(;;){
		HAL_GPIO_WritePin(BOTH_LED_PORT, GREEN_PIN, LED_ON);
		if(button_state()){
			HAL_GPIO_WritePin(BOTH_LED_PORT, GREEN_PIN, LED_OFF);
			HAL_GPIO_WritePin(BOTH_LED_PORT, RED_PIN, LED_ON);
        }
	}
}

/**
  * @brief clear and display 'On Your Mark, Get Set, Go' on three different screens
  * @param none
  * @retval None
  */
void startScreen(){
	LCD_Clear(0,LCD_COLOR_MAGENTA);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(40,140,'O');
	LCD_DisplayChar(55,140,'N');
	LCD_DisplayChar(65,140,' ');
	LCD_DisplayChar(80,140,'Y');
	LCD_DisplayChar(95,140,'O');
	LCD_DisplayChar(110,140,'U');
	LCD_DisplayChar(125,140,'R');
	LCD_DisplayChar(135,140,' ');
	LCD_DisplayChar(150,140,'M');
	LCD_DisplayChar(165,140,'A');
	LCD_DisplayChar(180,140,'R');
	LCD_DisplayChar(195,140,'K');


	HAL_Delay(1000);

	LCD_Clear(0,LCD_COLOR_BLUE2);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(60,140,'G');
	LCD_DisplayChar(75,140,'E');
	LCD_DisplayChar(90,140,'T');
	LCD_DisplayChar(110,140,'S');
	LCD_DisplayChar(125,140,'E');
	LCD_DisplayChar(140,140,'T');

	HAL_Delay(1000);

	LCD_Clear(0,LCD_COLOR_GREEN);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(75,140,'G');
	LCD_DisplayChar(95,140,'O');

	HAL_Delay(1000);
}

/**
  * @brief clear and display 'Game Over' screen
  * @param none
  * @retval None
  */
void gameOver(){
	LCD_Clear(0,LCD_COLOR_RED);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(60,140,'G');
	LCD_DisplayChar(75,140,'A');
	LCD_DisplayChar(90,140,'M');
	LCD_DisplayChar(105,140,'E');
	LCD_DisplayChar(115,140,' ');
	LCD_DisplayChar(125,140,'O');
	LCD_DisplayChar(140,140,'V');
	LCD_DisplayChar(155,140,'E');
	LCD_DisplayChar(170,140,'R');
}

/**
  * @brief clear and display 'You Win' screen
  * @param none
  * @retval None
  */
void passGame(){
	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(40,80,'Y');
	LCD_DisplayChar(55,80,'O');
	LCD_DisplayChar(70,80,'U');
	LCD_DisplayChar(85,80,' ');
	LCD_DisplayChar(100,80,'W');
	LCD_DisplayChar(110,80,'I');
	LCD_DisplayChar(120,80,'N');
}

/* HELPER FUNCTIONS FOR COLLISIONS CODE */
/**
  * @brief Check the static maze array for path represented as 0 or wall represented as 1
  * @param x- index of x in the 2D array, y - index of y in the 2D array
  * @retval true - if the ball is in a path, false - if ball collides with wall
  */
bool path(int x, int y){
    if (static_maze[y][x] == 0){
        return true;
    }
    return false;
}

/* END OF HELPER FUNCTIONS FOR COLLISION CODE */


//void buttonDisruptTask(void *argument){
//	(void)&argument;
//	for(;;){
//	}
//}


//void gameMonitorTask(void *argument){
//	(void)&argument;
//	for(;;){
//	}
//}


