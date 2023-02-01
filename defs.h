
#ifndef DEFS_H
#define DEFS_H

/*--------------------- Definitions and configurations ----------------------*/
/*Number of rows and columns for the screen of the game*/
#define rows 													(12U)	
#define columns 											(35U)
/*The maximum possible length for the snake body*/
#define MAX_BODY_LEN 									(rows - 2)*(columns - 2)
/*a string that if sent to terminal on pc clears it and reset the cursor to the home position*/
#define CLEAR_TERMINAL_MACRO_STRING  	"\033[2J\033[H"
/*The delay values for tasks in ms*/
#define INITIAL_PROCESS_TASK_DELAY 						(100U)
#define INITIAL_PRINT_SCREEN_DELAY 						(100U)
/*Symbols for different game elements*/
#define SNAKE_BODY_SYMBOL 						'O'
#define FRUIT_SYMBOL 									'@'
#define POWER_UP_SYMBOL								'$'
#define BORDER_SYMBOL									'#'
#define OBSTACLE_SHAPE								'X'
/*Clock speed for tiva and the baud rate for UART*/
#define CLK_SPEED             			 	(16000000UL)
#define BAUD_RATE               			(230400)
/*the keyboard input queue length*/
#define KEYBOARD_INPUT_QUEUE_LEN			(3U)
#define INITIAL_TARGET_SCORE					(3U)
#define GAME_SPEED_UP_FACTOR   				(15U)
#define TARGET_SCORE_INCREMENT_FACTOR	(2U)
#define TIMER_PERIOD_MS 							(1000U)
#define POWER_UP_SCORE_INCREMENT 			(2U)
#define LEFT_DIR										(-1)
#define RIGHT_DIR										(1)
#define UP_DIR											(-1) 
#define DOWN_DIR										(1)
#define MAX_OBSTACLE_COUNT					(10) 
#define MAX_TIME_INTIAL							(12)
#define TIME_INCREASE_FACTOR				(5)

#endif