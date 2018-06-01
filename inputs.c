#include <stdlib.h>
#include <fsl_device_registers.h>  //allows you to interact with peripherals on device
#include <fsl_i2c.h>
#include "utils.h"
#include "inputs.h"

#define defaultaddr					0x74
#define functionreg					0x0B
#define commandreg					0xFD
#define shutdown						0x0A
#define pictureframe				0x01
#define picturemode					0x00
#define configreg						0x00
#define audioreg						0x06

#define length							16
#define height							9


/*
###########################################################
Main body of code
###########################################################
*/

void writeRegister(int b, int reg, int data) {
	unsigned char b2 = (unsigned char) b;
	i2c_master_transfer_t transfer = {
		.flags = kI2C_TransferDefaultFlag,
		.slaveAddress = defaultaddr,
		.direction = kI2C_Write,
		.subaddress = commandreg,
		.subaddressSize = 1,
		.data = &(b2),
		.dataSize = 1
	};
	status_t status = I2C_MasterTransferBlocking(I2C0, &transfer);
	
	unsigned char data2 = (unsigned char) data;
	i2c_master_transfer_t transfer2 = {
		.flags = kI2C_TransferDefaultFlag,
		.slaveAddress = defaultaddr,
		.direction = kI2C_Write,
		.subaddress = reg,
		.subaddressSize = 1,
		.data = &(data2),
		.dataSize = 1
	};
	status = I2C_MasterTransferBlocking(I2C0, &transfer2);
}

/*
	Draws a pixel on LED matrix
	Returns 1 if invalid, 0 otherwise
	Inspiration Drawn From begin, drawPixel, writeRegister8
	https://github.com/adafruit/Adafruit_IS31FL3731/blob/master/Adafruit_IS31FL3731.cpp
*/
int draw_pixel(int x, int y, int brightness) {
	unsigned char addr;									// 8 bit address
	
	if ((x < 0) | (x >= length) | (y < 0) | (y >= height)) {
		return 1;
	}
	if ((brightness < 0) | (brightness > 255)) {
		return 1;
	}
	
	addr = x+0x24+y*16;
	
	writeRegister(0, addr, brightness);
	
	return 0;
}

void clear(void) {
	unsigned char b = 0;
	i2c_master_transfer_t transfer = {
		.flags = kI2C_TransferDefaultFlag,
		.slaveAddress = defaultaddr,
		.direction = kI2C_Write,
		.subaddress = commandreg,
		.subaddressSize = 1,
		.data = &(b),
		.dataSize = 1
	};
	status_t status = I2C_MasterTransferBlocking(I2C0, &transfer);
	
	// write zeros to all LEDs in a slightly more efficient way
	for (int i=0; i<6; i++) {
		unsigned char i2 = 0x24+i*24;
		unsigned char b2[24] = {0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0};
		i2c_master_transfer_t transfer2 = {
			.flags = kI2C_TransferDefaultFlag,
			.slaveAddress = defaultaddr,
			.direction = kI2C_Write,
			.subaddress = i2,
			.subaddressSize = 1,
			.data = b2,
			.dataSize = 24
		};
		status_t status = I2C_MasterTransferBlocking(I2C0, &transfer2);
	}
}

/*
 * Draws a line from (x0, y0) to (x1, y1)
 * on the external LED matrix using draw_pixel
 * Taken shamelessly from
 * https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp
*/
void draw_line(int x0, int y0, int x1, int y1, int brightness) {
	int tmp;
	int dx, dy;
	int err, ystep;
	
	bool steep = abs(y1-y0) > abs(x1-x0);
	if (steep) { // forces change in y to be less than in x
		tmp = x0;
		x0 = y0;
		y0 = tmp;
		tmp = x1;
		x1 = y1;
		y1 = tmp;
	}
	if (x0 > x1) { // forces x0 <= x1
		tmp = x0;
		x0 = x1;
		x1 = tmp;
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}
	
	dx = x1 - x0;
	dy = abs(y1 - y0);
	
	err = dx / 2;
	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;
	for (; x0<=x1; x0++) {
		if (steep) {
			draw_pixel(y0, x0, brightness);			// swap x/y coordinates back
		} else {
			draw_pixel(x0, y0, brightness);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

/*
 * Draws the digit 0 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_zero(int xoffset, int brightness) {
	draw_line(2+xoffset, 1, 2+xoffset, 7, brightness);
	draw_line(6+xoffset, 1, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 1, 6+xoffset, 1, brightness);
	draw_line(2+xoffset, 7, 6+xoffset, 7, brightness);
}

/*
 * Draws the digit 1 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_one(int xoffset, int brightness) {
	draw_line(2+xoffset, 1, 2+xoffset, 7, brightness);
}

/*
 * Draws the digit 2 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_two(int xoffset, int brightness) {
	draw_line(2+xoffset, 4, 2+xoffset, 7, brightness);
	draw_line(6+xoffset, 1, 6+xoffset, 4, brightness);
	draw_line(2+xoffset, 1, 6+xoffset, 1, brightness);
	draw_line(2+xoffset, 7, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 4, 6+xoffset, 4, brightness);
}

/*
 * Draws the digit 3 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_three(int xoffset, int brightness) {
	draw_line(2+xoffset, 1, 2+xoffset, 7, brightness);
	draw_line(2+xoffset, 1, 6+xoffset, 1, brightness);
	draw_line(2+xoffset, 7, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 4, 5+xoffset, 4, brightness);
}

/*
 * Draws the digit 4 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_four(int xoffset, int brightness) {
	draw_line(2+xoffset, 1, 2+xoffset, 7, brightness);
	draw_line(2+xoffset, 4, 6+xoffset, 4, brightness);
	draw_line(6+xoffset, 4, 6+xoffset, 7, brightness);
}

/*
 * Draws the digit 5 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_five(int xoffset, int brightness) {
	draw_line(6+xoffset, 4, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 1, 2+xoffset, 4, brightness);
	draw_line(2+xoffset, 1, 6+xoffset, 1, brightness);
	draw_line(2+xoffset, 7, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 4, 6+xoffset, 4, brightness);
}

/*
 * Draws the digit 6 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_six(int xoffset, int brightness) {
	draw_line(6+xoffset, 1, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 1, 2+xoffset, 4, brightness);
	draw_line(2+xoffset, 1, 6+xoffset, 1, brightness);
	draw_line(2+xoffset, 7, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 4, 6+xoffset, 4, brightness);
}

/*
 * Draws the digit 7 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_seven(int xoffset, int brightness) {
	draw_line(2+xoffset, 1, 2+xoffset, 7, brightness);
	draw_line(2+xoffset, 7, 6+xoffset, 7, brightness);
}

/*
 * Draws the digit 8 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_eight(int xoffset, int brightness) {
	draw_line(2+xoffset, 1, 2+xoffset, 7, brightness);
	draw_line(2+xoffset, 1, 6+xoffset, 1, brightness);
	draw_line(2+xoffset, 7, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 4, 6+xoffset, 4, brightness);
	draw_line(6+xoffset, 1, 6+xoffset, 7, brightness);
}

/*
 * Draws the digit 9 on the external LED matrix using
 * draw_line. The digit is shifted xoffset LEDs left.
*/
void draw_nine(int xoffset, int brightness) {
	draw_line(2+xoffset, 1, 2+xoffset, 7, brightness);
	draw_line(2+xoffset, 4, 6+xoffset, 4, brightness);
	draw_line(6+xoffset, 4, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 7, 6+xoffset, 7, brightness);
	draw_line(2+xoffset, 1, 6+xoffset, 1, brightness);
}

/*
 * Draws a digit on the external LED matrix using
 * draw_line. If left=1, shifts all lines to for first digit
*/
void draw_digit(int digit, int left, int brightness) {
	int xoffset = left*7;
	switch (digit) {
		case 0:
			draw_zero(xoffset, brightness);
			break;
		case 1:
			draw_one(xoffset, brightness);
			break;
		case 2:
			draw_two(xoffset, brightness);
			break;
		case 3:
			draw_three(xoffset, brightness);
			break;
		case 4:
			draw_four(xoffset, brightness);
			break;
		case 5:
			draw_five(xoffset, brightness);
			break;
		case 6:
			draw_six(xoffset, brightness);
			break;
		case 7:
			draw_seven(xoffset, brightness);
			break;
		case 8:
			draw_eight(xoffset, brightness);
			break;
		case 9:
			draw_nine(xoffset, brightness);
			break;
		default:
			draw_one(xoffset, brightness);
			break;
	}
}

/* 
 * Draws the least significant two digits in a number
 * on the external LED matrix using draw_digit
 * Numerals drawn are 5x7 to fit 2 on matrix
*/
void draw_num(int num, int brightness) {
	int digit0 = num % 10;
	int digit1 = (num%100 - digit0)/10;
	draw_digit(digit0, 0, brightness);
	draw_digit(digit1, 1, brightness);
}

/*
 * Draws the speed level of the next game to be played
 */
void draw_speed(int speed, int brightness) {
	clear();
	draw_line(15, 8, 15, (8-speed), brightness);
}

/*
 * I2C and Board Setup - initializes I2C on MCU and output pins
 * Then restarts external board and clears display
 * Drawn from Adafruit begin function
 * https://github.com/adafruit/Adafruit_IS31FL3731/blob/master/Adafruit_IS31FL3731.cpp
 */
void board_enable(void) {
	int i = 100000;
	i2c_master_config_t config = {
		.enableMaster = true,
		.enableStopHold = false,		//.enablehighDrive = false,
		.baudRate_Bps = 100000,
		.glitchFilterWidth = 0
	};
	I2C_MasterInit(I2C0, &config, 21000000U);
	I2C_Initialize();
	
	// restart board
	writeRegister(functionreg, shutdown, 0);
	while (i > 0) {
		i--;
	}
	writeRegister(functionreg, shutdown, 1);
	
	// picture mode
	writeRegister(functionreg, configreg, picturemode);
	// display frame
	writeRegister(functionreg, pictureframe, 0);
	
	clear();
	
	for (uint8_t f=0; f<8; f++) {  
    for (uint8_t j=0; j<=0x11; j++)
      writeRegister(f, j, 0xff);     // each 8 LEDs on
  }
	
	writeRegister(functionreg, audioreg, 0);
}
