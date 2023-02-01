
/*--------------------------------- INCLUDES---------------------------------*/
#include "game.h"
#include "uart.h"

/*--------------------- Global Variables ---------------*/
/*A global variable used by XOR_SHIFT function representing the current state of the random number*/
uint32 current_state = 10;
/*Obstacles array*/ 
obstacle obstacles[MAX_OBSTACLE_COUNT]; 
uint8 obstacle_count = 1; 
/*a 2D array that represents the screen pixel by pixel*/
uint8 screen[rows][columns];
/*The snake itself*/
Snake snake;
/*the fruit location on the screen*/
point_t fruit_location;
volatile point_t special_power_up_location;
/*the current direction the snake is headed, this value is initialized with right*/
uint8 currentDirection = 'R';
/*current game score*/
uint8 current_score = 0;
uint8 highest_score = 0;
/*a flag that represents whether the player lost the game or not*/
uint8 state_of_game = JUST_STARTED;
/*variables that control the speed of the game*/
uint8 target_score = INITIAL_TARGET_SCORE;



/*-------------------------- FUNCTION DEFINITIONS----------------------------*/

/*initialize screen by putting the spaces and the borders of the screen*/ 
void initalize_screen()
{
	uint8 row, col;
	for (row = 0; row < rows; ++row)
		{
				for (col = 0; col < columns; ++col)
				{
						if(row == 0 || row == rows-1 || col == 0 || col == columns -1)
						{
							screen[row][col] = BORDER_SYMBOL; 
						}	
						else
						{
							screen[row][col] = ' ';
						}
				}
		}
}

/* Draw the Enemies on the screen */
void draw_obstacle()
{
	uint8 indx; 
	for (indx = 0; indx < obstacle_count; ++indx)
	{
		screen[obstacles[indx].position.y][obstacles[indx].position.x] = OBSTACLE_SHAPE; 
	}
}

/* Draw the Snake on the screen */
void draw_snake()
{
	uint8 indx;
	for (indx = 0; indx < snake.current_len; indx++)
		{
				screen[snake.body[indx].y][snake.body[indx].x] = SNAKE_BODY_SYMBOL;
		}
}

/* Draw the Fruits on the screen */
void draw_fruits()
{
	screen[fruit_location.y][fruit_location.x] = FRUIT_SYMBOL;
	screen[special_power_up_location.y][special_power_up_location.x] = POWER_UP_SYMBOL;
}

/* Draw the screen on the terminal through UART */
void draw_screen()
{
	uint8 row, col; 
	for (row = 0; row < rows; ++row)
	{
		for (col = 0; col < columns; ++col)
		{
				uart0_putchar(screen[row][col]);
		}
		print("\r\n");
	}
}


/*processing the input from the user*/ 
void processInput(uint8 cmd) 
{
	uint8 fruit_taken; 
	uint8 special_fruit_taken; 
	uint8 indx = 0; 
	switch(cmd)
	{
		case 'L': 
		case 'a': 
			/*if body collides*/ 
			if(detect_collision(LEFT_DIR,0))
			{
				state_of_game = GAME_OVER; 
			}
			else
			{
				fruit_taken = check_if_fruit_taken(LEFT_DIR,0); 
				special_fruit_taken = check_if_special_fruit_taken(LEFT_DIR,0); 
				if(!(fruit_taken || special_fruit_taken))
				{
					update_snake_normal(LEFT_DIR,0);  
				}
				currentDirection = 'L';
			break; 
		case 'R': 
		case 'd':
			if(detect_collision(RIGHT_DIR,0))
			{
				state_of_game = GAME_OVER; 
			}
			else
			{
				fruit_taken = check_if_fruit_taken(RIGHT_DIR,0); 
				special_fruit_taken = check_if_special_fruit_taken(RIGHT_DIR,0); 
				if(!(fruit_taken || special_fruit_taken))
				{
					update_snake_normal(RIGHT_DIR,0); 
				}
				currentDirection = 'R';
			}
			break; 
		case 'U': 
		case 'w': 
			if(detect_collision(0,UP_DIR))
			{
				state_of_game = GAME_OVER; 
			}
			else
			{
				fruit_taken = check_if_fruit_taken(0,UP_DIR); 
				special_fruit_taken = check_if_special_fruit_taken(0,UP_DIR); 
				if(!(fruit_taken || special_fruit_taken))
				{
					update_snake_normal(0,UP_DIR); 
				}
				currentDirection = 'U'; 
			}
			break; 
		case 'D':
		case 's':
			if(detect_collision(0,DOWN_DIR))
			{
				state_of_game = GAME_OVER; 
			}
			else
			{
				fruit_taken = check_if_fruit_taken(0,DOWN_DIR); 
				special_fruit_taken = check_if_special_fruit_taken(0,DOWN_DIR); 
				if(!(fruit_taken || special_fruit_taken))
				{
					update_snake_normal(0,DOWN_DIR); 
				}
				currentDirection = 'D'; 
			}
			break; 
		}
	}
}

/*check if snake took special fruit*/ 
uint8 snake_took_special_fruit(short direction_to_check_x, short direction_to_check_y)
{
	return (snake.body[snake.current_len - 1].x + direction_to_check_x == special_power_up_location.x 
		&& snake.body[snake.current_len-1].y + direction_to_check_y == special_power_up_location.y);
}

/*check if snake took normal fruit*/ 
uint8 snake_took_fruit(short direction_to_check_x, short direction_to_check_y)
{
	return (snake.body[snake.current_len - 1].x + direction_to_check_x == fruit_location.x
		&& snake.body[snake.current_len-1].y + direction_to_check_y == fruit_location.y);
}

/*update the special powerup position*/ 
void update_special_fruit_pos()
{
	special_power_up_location.x = 20;
	special_power_up_location.y = 15;
}

/*update the snake position in case of not taking any power up*/ 
void update_snake_normal(short increment_x, short increment_y)
{
	unsigned int indx; 
	for(indx = 0 ; indx < snake.current_len ; indx++)
	{
		if(indx == snake.current_len - 1)
		{
			snake.body[snake.current_len - 1].x = snake.body[snake.current_len - 1].x + increment_x;
			snake.body[snake.current_len - 1].y = snake.body[snake.current_len - 1].y + increment_y;
		}
		else
		{
			snake.body[indx].x = snake.body[indx+1].x;
			snake.body[indx].y = snake.body[indx+1].y;
		}
	}
}

/*updates the snake position if it took a normal fruit*/ 
void update_snake_pos_fruit(uint8 x_position, uint8 y_position)
{
	snake.current_len++;
	snake.body[snake.current_len - 1].x = x_position;
  snake.body[snake.current_len - 1].y = y_position;
}

void update_snake_pos_special_fruit(uint8 x_position, uint8 y_position)
{
	snake.current_len += 2;
	snake.body[snake.current_len - 2].x = x_position;
  snake.body[snake.current_len - 2].y = y_position;
	switch(currentDirection) 
	{
		case 'R': 
			snake.body[snake.current_len - 1].x = x_position + 1;
			snake.body[snake.current_len - 1].y = y_position;
		break; 
		case 'L':
			snake.body[snake.current_len - 1].x = x_position - 1;
			snake.body[snake.current_len - 1].y = y_position;
		break; 
		case 'U':
			snake.body[snake.current_len - 1].x = x_position;
			snake.body[snake.current_len - 1].y = y_position - 1;
		break; 
		case 'D':
			snake.body[snake.current_len - 1].x = x_position;
			snake.body[snake.current_len - 1].y = y_position + 1;
	}
}

/*update fruit location after it has been taken*/ 
void update_fruit_location()
{
		fruit_location.x = xor_shift() % (columns - 3) + 1;
		fruit_location.y = xor_shift() % (rows - 3) + 1;
}

/*update obstacle position*/ 
void update_obstacle_pos()
{
		uint8 indx; 
		for(indx = 0; indx < obstacle_count; ++indx)
		{
			obstacles[indx].position.x = xor_shift() % (columns - 3) + 1;
			obstacles[indx].position.y = xor_shift() % (rows - 3) + 1;
		}
}

/*checks if a fruit is going to be taken in a certain direction and updates the game accordingly*/ 
uint8 check_if_fruit_taken(short direction_to_check_x, short direction_to_check_y)
{
	if(snake_took_fruit(direction_to_check_x,direction_to_check_y)) 
	{
		current_score++; 
		if(current_score == target_score)
		{
			state_of_game = GAME_WON; 
		}
		update_snake_pos_fruit(fruit_location.x, fruit_location.y); 
		update_fruit_location(); 
		return 1; 
	}
	return 0; 
}

/*checks if a special fruit is going to be taken in a certain direction and updates the game accordingly*/ 
uint8 check_if_special_fruit_taken(short direction_to_check_x, short direction_to_check_y)
{
	if (snake_took_special_fruit(direction_to_check_x,direction_to_check_y))
	{
		current_score += POWER_UP_SCORE_INCREMENT;
		if(current_score >= target_score)
		{
				current_score = target_score;
				state_of_game = GAME_WON;
		}	
		update_snake_pos_special_fruit(special_power_up_location.x, special_power_up_location.y); 
		update_special_fruit_pos(); 
		return 1;
	}
	return 0; 
}

void init_game(void){
	uint8 indx; 
	uint8 currentDirection = 'R';
	
	current_score = 0 ;
	snake.current_len = 1;
	snake.body[0].x = 4;
	snake.body[0].y = 4;
	fruit_location.x = 10;
	fruit_location.y = 3;
	special_power_up_location.x = 20;
	special_power_up_location.y =9;
}

uint8 detect_collision(sint8 x_dir_to_check, sint8 y_dir_to_check)
{
	uint8 next_y = snake.body[snake.current_len- 1].y + y_dir_to_check; /*y coordinate the snake is about to pass*/  
	uint8 next_x = snake.body[snake.current_len - 1].x + x_dir_to_check;
	uint8 indx; 
	/*if the snake is going to hit the frame*/ 
	if(next_y == 0 || next_y == rows - 1 || 
		next_x ==0 ||next_x == columns - 1)
	{
		return 1; 
	}
	/*if the snake is going to hit an obstacle*/ 
	for(indx = 0; indx < obstacle_count; ++indx)
	{
		if(obstacles[indx].position.x == next_x && obstacles[indx].position.y == next_y)
			{
				return 1;
			}
	}
	/*if the snake is going to eat it self*/ 
	for(indx = 0 ; indx < snake.current_len ; indx++)
	{
		if(snake.body[indx].x == next_x && snake.body[indx].y == next_y)
		{
			return 1;
		}
	}
	return 0; 
}

uint32 xor_shift(void)
{
    current_state ^= (current_state << 13);
    current_state ^= (current_state >> 17);
    current_state ^= (current_state << 5);
	return current_state;
}
