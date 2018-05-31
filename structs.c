#include <stdlib.h>
#include <fsl_device_registers.h>
#include "utils.h"
#include "inputs.h"
#include "structs.h"

/* 
 * Returns 1 if the head of the snake has collided with any part
 * of its body, and returns 0 otherwise
 */
int body_collision (snake_part * new_obj) {
	snake_part * temp = head;
	while (temp != NULL) {
		if (new_obj->x == temp->x 
		    && new_obj->y == temp->y) {
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

/* 
 * Returns 1 if the head of the snake has collided with any part
 * of the border, and returns 0 otherwise
 */
int border_collision (snake_part * new_obj) {
	if (new_obj->x < 0 || new_obj->x > 15) { //width [0, 15]
		return 1;
	}
	else if (new_obj->y < 0 || new_obj->y > 8) { //height [0, 8]
		return 1;
	}
	else {
		return 0;
	}
}

/*
 * Returns 1 if the head of the snake has collided with a food item
 * and returns 0 otherwise
 */
int food_collision (snake_part * new_obj) {
	if (new_obj->x == food->x &&  new_obj->y == food->y) {
		return 1;
	}
	else {
		return 0;
	}
}
