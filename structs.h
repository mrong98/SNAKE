#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include <stdlib.h>
#include <fsl_device_registers.h>

typedef struct food_type {
	int x;
	int y;
} food_item;


typedef struct snake_part_type {
	int x;
	int y;
	struct snake_part_type * next; //pointer to the next snake part (null if its the tail of the snake)
	struct snake_part_type * prev; //pointer to the previous snake part (null if its the head)
} snake_part;

extern snake_part * head;				//head of the snake, a snake part object
extern snake_part * tail;				//tail of snake, a snake part object
extern food_item * food;				//the food item currently on the screen

/* 
 * Returns 1 if the head of the snake has collided with any part
 * of its body, and returns 0 otherwise
 */
int body_collision(snake_part * new_obj);

/* 
 * Returns 1 if the head of the snake has collided with any part
 * of the border, and returns 0 otherwise
 */
int border_collision (snake_part * new_obj);

/*
 * Returns 1 if the head of the snake has collided with a food item
 * and returns 0 otherwise
 */
int food_collision (snake_part * new_obj);

#endif
