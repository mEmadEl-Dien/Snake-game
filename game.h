
#ifndef GAME_H
#define GAME_H

/*--------------------------------- INCLUDES---------------------------------*/
#include "std_types.h"
#include "defs.h"

/*-------------------------- Structures and unions --------------------------*/
/*A struct that represents a point on the screen*/
typedef struct
{
	/*the x and y coordinates of the point on the screen*/
    uint8 x;
    uint8 y;
} point_t;

/*a struct that represents the snake itself*/
typedef struct
{
	/*an array of points that represent the snake body*/
    point_t body[MAX_BODY_LEN];
	/*the current length of the snake body*/
    uint8 current_len;
} Snake;

typedef struct
{
		point_t position; 
}obstacle; 


/*---------------------------------- ENUMS ----------------------------------*/
typedef enum{
	CURRENTLY_PLAYING, GAME_OVER, JUST_STARTED, GAME_WON
}State_Enum;


/*--------------------- Extern Global Variables ---------------*/
/*A global variable used by XOR_SHIFT function representing the current state of the random number*/
extern uint32 current_state;
/*Obstacles array*/ 
extern obstacle obstacles[MAX_OBSTACLE_COUNT]; 
extern uint8 obstacle_count; 
/*a 2D array that represents the screen pixel by pixel*/
extern uint8 screen[rows][columns];
/*The snake itself*/
extern Snake snake;
/*the fruit location on the screen*/
extern point_t fruit_location;
extern volatile point_t special_power_up_location;
/*the current direction the snake is headed, this value is initialized with right*/
extern uint8 currentDirection;
/*current game score*/
extern uint8 current_score;
extern uint8 highest_score;
/*a flag that represents whether the player lost the game or not*/
extern uint8 state_of_game;
/*variables that control the speed of the game*/
extern uint8 target_score;

/*-------------------------- FUNCTION PROTOTYPES----------------------------*/
void NoInput(uint8 Cmd);
void init_game(void);
uint8 detect_collision(sint8 x_dir_to_check, sint8 y_dir_to_check);
void processInput(uint8 cmd); 
uint32 xor_shift(void);
uint8 bodyCollided(uint8 direction);
uint8 check_if_special_fruit_taken(short direction_to_check_x, short direction_to_check_y);
uint8 check_if_fruit_taken(short direction_to_check_x, short direction_to_check_y);
void update_fruit_location();
void update_snake_pos_fruit(uint8 x_position, uint8 y_position);
void update_snake_normal(short increment_x, short increment_y);
void update_special_fruit_pos();
void update_obstacle_pos(); 
uint8 snake_took_fruit(short direction_to_check_x, short direction_to_check_y);
uint8 snake_took_special_fruit(short direction_to_check_x, short direction_to_check_y);


#endif