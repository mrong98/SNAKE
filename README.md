# SNAKE

## Introduction
Our project implements the classic Nokia phone game, SNAKE using the FRDM K64F board and an LED matrix.

Users can play the game SNAKE on a two-dimensional LED matrix. The snake moves automatically approximately every 0.1 seconds and in a forward direction unless the user dictates it to turn left or right. One food item is on the board at all times and the snake can eat the food and increase in length if it collides into it. The goal of this game is to get to the snake to the longest length possible without colliding into itself or the border of the board.

## High Level Description
The SNAKE game is a simple two-dimensional video game, popularized by its inclusion on early Nokia phones. The player controls a snake, which is a contiguous chain of grid spaces on a two dimensional grid. The snake is constantly moving forward and the player is able to control it by indicating when to turn left or right. Once a player indicates turning left or right, it then continues to move in the new forward direction, with each segment of the snake following the path which the head took such that every segment moves exactly one grid space every time step. Additionally on the grid there is always one food item which is one grid space, where if the snake “eats” it the snake grows longer by one grid space. The snake initially start with length 2. The food item then randomly regenerates in an unoccupied location every time the previous food item is eaten. The player loses the game when the snake dies by either colliding into the border of the grid or with itself.

We created our version of SNAKE using the FRDM-K64F board with its hardware buttons and an LED screen. The snake moves on a timer set to approximately 0.1 seconds. The two buttons on the board allow the player to control the snake to turn left or right, and the GUI with the snake and food item are displayed on a LED matrix board. 

## System Description
The game consists of a large loop which initializes and deinitializes game state variables as needed. It is periodically interrupted by PIT interrupts, which causes the snake to move one step forward. Button inputs also cause interrupts, which change the way in which the snake moves on the next update. All changes to the game's state are then communicated over I2C with the LED Matrix. We use code aimed at the Adafruit IS31FL3731 and its corresponding Charlieplexed LED matrix.

There is also another periodic timer which acts as a random number seed generator.
