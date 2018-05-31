#include <stdlib.h>
#include <fsl_device_registers.h>
#include "utils.h"
#include "inputs.h"
#include "structs.h"

#define snake_brightness 		15
#define food_brightness			150


//game status variables
int volatile game_started;		//0 if game not started, 1 if game started
int volatile game_over;			//0 if game not over, 1 if game is over
int volatile score;				//represents the player's score - # of food eaten
int current_time;				//number of 5ms intervals since start, reset every 5s
int speed;						//speed level, ranges from 0 to 3, 0 being the slowest
											//and 3 being the fastest

snake_part * head = NULL;
snake_part * tail = NULL;
food_item * food = NULL;

int turn;							//0 = forward, 1 = left, 2 = right and 0 is the default turn
int direction;						//0 = up, 1 = left, 2 = right, 3 = down and 1 is the initial direction
//empty_spaces: kevin

/*
MOVE FOOD: kevin
*/

/* 
 * Removes the tail object of the snake and reassigns the global variable
 * to point to the new tail object
 */
void remove_tail (void) {
	
	draw_pixel(tail->x, tail->y, 0);
	//empty_spaces[tail->x + 16*tail->y] = 1;        KEVIN
	
	snake_part * temp = tail->prev;

	free(tail);

	temp->next = NULL;
	tail = temp;
}

/*
 * Sets the head variable to be the new head and displays it on the LEDs
 */
void move_to (snake_part * new_head) {
	head = new_head;
	
	draw_pixel(head->x, head->y, snake_brightness);
}

/* 
 * Creates the new snake_part of what the next head of the snake should be
 * based on what its current direction is and which way it turns
 */
snake_part * new_head_coord (void) {
	int new_x = head->x;
	int new_y = head->y;
	int old_dir = direction;
	
	if (old_dir == 0) { //going up
		if (turn == 1) { 
			new_x = new_x - 1;
			direction = 1;
		}
		else if (turn == 2) {
			new_x = new_x + 1;
			direction = 2;
		}
		else  {
			new_y = new_y - 1;
			direction = 0;
		}
	}
	else if (old_dir == 1) { //going left
		if (turn == 1) {
			new_y = new_y + 1;
			direction = 3;
		}
		else if (turn == 2) {
			new_y = new_y - 1;
			direction = 0;
		}
		else  {
			new_x = new_x - 1;
			direction = 1;
		}
	}
	else if (old_dir == 2) { //going right
		if (turn == 1) {
			new_y = new_y - 1;
			direction = 0;
		}
		else if (turn == 2) {
			new_y = new_y + 1;
			direction = 3;
		}
		else  {
			new_x = new_x + 1;
			direction = 2;
		}
	}
	else { //going down
		if (turn == 1) {
			new_x = new_x + 1;
			direction = 2;
		}
		else if (turn == 2) {
			new_x = new_x - 1;
			direction = 1;
		}
		else  {
			new_y = new_y + 1;
			direction = 3;
		}
	}
	turn = 0;
	LED_Off();

	snake_part * new_head = (snake_part *) malloc(sizeof(snake_part));
	if (!new_head) {
		return NULL;
	}
	new_head->x = new_x;
	new_head->y = new_y;
	new_head->prev = NULL;
	head->prev = new_head;
	new_head->next = head;
	
	//empty_spaces[new_head->x + 16*new_head->y] = 0;	KEVIN

	return new_head;
}

/*
 * Changes the speed of the snake by loading a specific value into PIT0 based on
 * what the speed was set to.
 */
void set_speed(int speed_lvl) {
	if (speed_lvl == 0) {
		PIT->CHANNEL[0].LDVAL = 0x00300000;
	}
	else if (speed_lvl == 1) {
		PIT->CHANNEL[0].LDVAL = 0x00200000;
	}
	else if (speed_lvl == 2) {
		PIT->CHANNEL[0].LDVAL = 0x00180000;
	}
	else {
		PIT->CHANNEL[0].LDVAL = 0x00100000;
	}
}

/*
 * Intializes the game state in the beginning of a game
 */
void init_game_state (void) {
	clear();
	for (int i=0; i < 144; i++) {						// initialize all spaces as empty
		empty_spaces[i] = 1;
	}

	snake_part * new_head = (snake_part *) malloc(sizeof(snake_part));
	if (!new_head) {
		return;
	}
	snake_part * new_tail = (snake_part *) malloc(sizeof(snake_part));
	if (!new_tail) {
		return;
	}

	// add head and tail
	new_head->prev = NULL;
	new_head->next = new_tail;
	new_head->x = 7;
	new_head->y = 4;
	head = new_head;
	
	new_tail->prev = new_head;
	new_tail->next = NULL;
	new_tail->x = 8;
	new_tail->y = 4;
	tail = new_tail;
	
	draw_pixel(head->x, head->y, snake_brightness);
	draw_pixel(tail->x, tail->y, snake_brightness);
	
	empty_spaces[7+16*4]=0;
	empty_spaces[8+16*4]=0;

	// add food
	food_item * new_food = (food_item *) malloc(sizeof(food_item));
	if (!new_food) {
		return;
	}
	food = new_food;
	move_food();
	draw_pixel(food->x, food->y, food_brightness);

	set_speed(speed);
	
	score = 0;
	direction = 1; //initially pointing left
	turn = 0;

}

/*
SETUP - KEVIN
*/

/*
 * Main loop of the game code
 */
int main (void) {
	game_started = 0;
	game_over = 0;
	speed = 0;
	
	setup();
	init_game_state();

	while (1) {

		while (!game_over);
		
		//game is over
		clear();
		draw_num(score, 30);
		draw_speed(speed, 30);
		
		free(food);
		snake_part * temp = head->next;
		while (temp) {
			free(temp->prev);
			temp = temp->next;
		}
		free(tail);
	
		game_started = 0;
		while (!game_started);
	}
	return 0;
}

/*
 * Updates the game state to reflect a single iteration of movement in the snake
 * Called on every timer interrupt
 */
void game_loop (void) {

	if (game_started && !game_over) {
		snake_part * new_head_obj = new_head_coord();
		if (new_head_obj == NULL) {return;}

		if (body_collision(new_head_obj) || border_collision(new_head_obj)) {
			game_over = 1;
			remove_tail();
		}
		else { //a game-over collision did not occur
			if (food_collision(new_head_obj)) {
				score = score + 1;
				move_food();
				draw_pixel(food->x, food->y, food_brightness);
				//snake length increases by 1, tail is not removed
			}
			else {
				remove_tail();
			}
			move_to(new_head_obj);
		}
	}
}


/* 
     PIT0 Interrupt Handler - the timer
*/
void PIT0_IRQHandler(void)
{
	game_loop();
	PIT->CHANNEL[0].TFLG |= 1; 			//reset timer flag
	NVIC_ClearPendingIRQ(PIT0_IRQn);	//remove interrupt
}

/*
	PORT A IRQ Handler for SW3 (left button)
*/
void PORTA_IRQHandler(void) {
	if (game_started == 0) {
		if (game_over) {
			game_over = 0;
			init_game_state();
		}
		else {
			game_started = 1;
		}
		srand(current_time); //add randomness to all new food locations
	}

	else {turn = 1;}
	LEDBlue_On();
	PORTA->PCR[4] |= PORT_PCR_ISF_MASK; //clear interrupt flag
	NVIC_ClearPendingIRQ(PORTA_IRQn); 	//remove interrupt
}

/*
	PORT C IRQ Handler for SW2 (right button)
*/
void PORTC_IRQHandler(void) {
	if (game_started == 0) {
		if (game_over) {
			speed = (speed + 1) % 4;
			draw_speed(speed, 30);
		}
		else { 
			game_started = 1;
		}
		srand(current_time); //add randomness to all new food locations
	}
	else {turn = 2;}
	LEDRed_On();
	PORTC->PCR[6] |= PORT_PCR_ISF_MASK; //clear interrupt flag
	NVIC_ClearPendingIRQ(PORTC_IRQn); 	//remove interrupt
}


/*
PIT1_IRQHandler - KEVIN
*/