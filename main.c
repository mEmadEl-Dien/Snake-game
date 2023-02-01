
 /*--------------------------------- INCLUDES---------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "uart.h"
#include "defs.h"
#include "game.h"

/*-------------------------- FUNCTION PROTOTYPES----------------------------*/
void vTaskScreenPrint( void *pvParameters );
void vProcessTask( void *pvParameters );
void vTimerCallbackFunction(TimerHandle_t xTimer );


/*--------------------- Global Variables ---------------*/
/*the input Queue handle which buffers input from user keyboard*/
xQueueHandle input_queue;
/*A mutex that is used to synch between the printing and processing tasks*/
xSemaphoreHandle xMutex;
/*A handle for print task, used later to suspend task from printing in case of a gameover*/
xTaskHandle printTaskHandle;
volatile uint32 timer_ticks = 0;
xTimerHandle timer;
/*variables that control the speed of the game*/
uint8 process_task_delay = INITIAL_PROCESS_TASK_DELAY;
uint8 print_task_delay = INITIAL_PRINT_SCREEN_DELAY;
uint8 max_time = MAX_TIME_INTIAL; 

/*---------------------- Main Function---------------------------------*/
int main( void )
{
	 /*Initialization*/
	init_game();
	uart0_init(CLK_SPEED , BAUD_RATE);
	input_queue = xQueueCreate(KEYBOARD_INPUT_QUEUE_LEN , sizeof(uint8));
	xMutex = xSemaphoreCreateMutex();
	timer = xTimerCreate("Timer1" , (TIMER_PERIOD_MS / portTICK_RATE_MS),pdTRUE,NULL,vTimerCallbackFunction);
	if(input_queue != NULL){
		xTaskCreate(vTaskScreenPrint, "Printing Task", 100, NULL, 2, &printTaskHandle);
		xTaskCreate(vProcessTask , "Processing Task" , 100 , NULL , 2 , NULL);
		vTaskStartScheduler();
	}
	for( ;; );
}
/*-----------------------------------------------------------*/

/***PRINTING TASK****/
void vTaskScreenPrint( void *pvParameters )
{
	/*looping variables that will be used later*/
	uint8 i,j;
	for( ;; )
	{
		/*fill the array with spaces and borders*/
		initalize_screen(); 
		/*Draw the snake on screen*/ 
		xSemaphoreTake(xMutex , portMAX_DELAY); /*Lock Here so the processing taks and the drawing task don't access the snake at the same time*/ 
		draw_snake(); 
		xSemaphoreGive(xMutex);
	  /*Draw Fruits special and normal*/ 
		draw_fruits(); 
		/*Draw obstacle on screen*/ 
		 draw_obstacle(); 
		/*check on state of the game and take action*/ 
		
		switch(state_of_game){
			case GAME_WON:/*in case player won by score or time*/ 
			  /*update the score if he broke the high score*/ 
				if(current_score > highest_score)
				{
					print("Level Passed!!!\r\n"); 
					highest_score = current_score;
				}
				print("Well Done!!");
				print("Press G to advance to the next level\r\n");
				/*Increase the speed of the game by reducing the delay time of the printing task*/ 
				if(print_task_delay - GAME_SPEED_UP_FACTOR != 0)
				{
						print_task_delay -= GAME_SPEED_UP_FACTOR;
						process_task_delay -= GAME_SPEED_UP_FACTOR;
				}
				/*Increase the required score*/ 
				target_score+=TARGET_SCORE_INCREMENT_FACTOR;
				/*increase time available for player*/ 
				max_time += TIME_INCREASE_FACTOR;
				/*increase number of obstacles*/ 
				obstacle_count++; 
				/*wait for user to press G*/ 
				vTaskSuspend(NULL);
				break;
			case JUST_STARTED:
				print("                        Welcome To Snake Game\r\n");
				print("Control Keys:\r\nW: Move Snake Up\r\nS: Move Snake Down\r\nA: Move Snake Left\r\nD: Move Snake Right\r\n");
				print("Press G to start the game\r\n");
				vTaskSuspend(NULL);
				break;
			case GAME_OVER:
				/*print the current score*/
				print("score : ");
				print_idec(current_score);
				if(current_score > highest_score)
				{
					highest_score = current_score;
				}
				/*Reseting the speed of the game*/
				print_task_delay = INITIAL_PRINT_SCREEN_DELAY;
				process_task_delay = INITIAL_PROCESS_TASK_DELAY;
				target_score = INITIAL_TARGET_SCORE;
				max_time = MAX_TIME_INTIAL;
				obstacle_count = 1;
				/*print the current score*/
				print(", Highest score : ");
				print_idec(highest_score);
				print("\r\n");
				print("Game over!!\r\nPress G to restart the game");
				vTaskSuspend(NULL);
				break;

			case CURRENTLY_PLAYING:
				print(CLEAR_TERMINAL_MACRO_STRING);				
				/*Draw the screen*/ 
				draw_screen(); 
			  print("Level Score: ");
			  print_idec(target_score);
				print("     My Score: ");
				print_idec(current_score);
				print("     Highest Score: "); 
				print_idec(highest_score);
				print("     time: ");
				print_idec(max_time);
				print("\r\n");
				/*otherwise send the screen to be printed on the serial terminal using UART*/
				break;
		}
		/*Delay this task to make it periodic, this controls the speed of priting*/
		vTaskDelay(print_task_delay/portTICK_RATE_MS);
	}
}

void vProcessTask( void *pvParameters )
{
	char receivedCmd; 
	unsigned int loopCtr = 0;
	for(;;)
	{
		if(uxQueueMessagesWaiting(input_queue) == 0)
		{
			xSemaphoreTake(xMutex , portMAX_DELAY);
			processInput(currentDirection);
			xSemaphoreGive(xMutex);
		}
		else
		{
			xQueueReceive(input_queue,&(receivedCmd),(TickType_t )0);
			xSemaphoreTake(xMutex , portMAX_DELAY);
			switch (receivedCmd)
			{
				case 'w':
				case 'W':
					if(currentDirection != 'D')
					{
						processInput('w');
					}
					else
					{
						processInput(currentDirection);
					}
					
					break;
				case 'a':
				case 'A':
					if(currentDirection != 'R')
					{
						processInput('a');
					}
					else
					{
						processInput(currentDirection);
					}
					break;
					
				case 's':
				case 'S':
					if(currentDirection != 'U')
					{
						processInput('s');
					}
					else
					{
						processInput(currentDirection);
					}
					break;
					
				case 'd':
				case 'D':
					if(currentDirection != 'L')
					{
						processInput('d');
					}
					else
					{
						processInput(currentDirection);
					}
					break;
				case 'G':
				case 'g':
					/*Change Direction Restart*/ 
					init_game();
					/*change obstacle position*/ 
				  update_obstacle_pos(); 
					state_of_game = CURRENTLY_PLAYING;
					currentDirection = 'R'; 
					xQueueReset(input_queue);
					timer_ticks = 0;
					xTimerReset(timer , 0);
					xTimerStart(timer , 0);
					vTaskResume(printTaskHandle);
					break;
				default : 
					processInput(currentDirection);
					break;
			}
			xSemaphoreGive(xMutex);
		}
		vTaskDelay(process_task_delay/portTICK_PERIOD_MS);
	}
}

void vTimerCallbackFunction(TimerHandle_t xTimer ){
	++timer_ticks;
	max_time--; 
	if(timer_ticks % 6 == 0){
			special_power_up_location.x = xor_shift() % (columns - 3) + 1;
			special_power_up_location.y = xor_shift() % (rows - 3) + 1;
	}
	if(max_time == 0) 
	{
		state_of_game = GAME_OVER; 
	}
}

void UART0_Handler(void){
	BaseType_t xHigherPriorityTaskAwaken = pdFALSE;
	static uint8 received_char;
	received_char = UART0_DR_R;
	UART0_ICR_R &= ~(1 << 4);
	xQueueSendFromISR(input_queue , &received_char , &xHigherPriorityTaskAwaken );
	xHigherPriorityTaskAwaken = pdFALSE;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
}

