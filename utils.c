#include <fsl_device_registers.h>
#include "utils.h"

/*----------------------------------------------------------------------------
  Function that initializes LEDs
 *----------------------------------------------------------------------------*/
void LED_Initialize(void) {

  SIM->SCGC5    |= (1 <<  10) | (1 <<  13);  /* Enable Clock to Port B & E */ 
  PORTB->PCR[22] = (1 <<  8) ;               /* Pin PTB22 is GPIO */
  PORTB->PCR[21] = (1 <<  8);                /* Pin PTB21 is GPIO */
  PORTE->PCR[26] = (1 <<  8);                /* Pin PTE26  is GPIO */
  
  PTB->PDOR = (1 << 21 | 1 << 22 );          /* switch Red/Green LED off  */
  PTB->PDDR = (1 << 21 | 1 << 22 );          /* enable PTB18/19 as Output */

  PTE->PDOR = 1 << 26;            /* switch Blue LED off  */
  PTE->PDDR = 1 << 26;            /* enable PTE26 as Output */
}

/*----------------------------------------------------------------------------
  Function that initializes SW2 and SW3 hardware buttons
 *----------------------------------------------------------------------------*/
void Button_Initialize(void) {
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK; 	// enable clock to Port A, C
	
	PORTC->PCR[6] = PORT_PCR_MUX(001);							// Pin PTC6 is GPIO
	PORTC->PCR[6] |= (0xA << PORT_PCR_IRQC_SHIFT);	// enable interrupt on rising edge
	PORTC->PCR[6] |= 3;															// setup pullup resistor for pin
	NVIC_EnableIRQ(PORTC_IRQn);											// Set IRQ handler for SW2
	PTC->PDDR &= ~(1 << 6);													// Enable PTA4 as input

	PORTA->PCR[4] = PORT_PCR_MUX(001);							// Pin PTA4 is GPIO
	PORTA->PCR[4] |= (0xA << PORT_PCR_IRQC_SHIFT);	// enable interrupt on rising edge
	PORTA->PCR[4] |= 3;															// setup pullup resistor for pin
	NVIC_EnableIRQ(PORTA_IRQn);											// Set IRQ handler for SW3
	PTA->PDDR &= ~(1 << 4);													// Enable PTA4 as input

	char *a = (char*) 0x40020003;										// Load FOPT register address
	*a &= ~(1 << 2);																// disable NMI on next Reset
}

/*
 * Initializes timer for game state cycle, and 5ms current_time timer
*/
void Timer_Initialize (void) {
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR &= ~(0x2); // Enables standard PIT timers and makes them run in debug mode
	
	NVIC_EnableIRQ(PIT0_IRQn); //enable PIT0 Interrupts
	PIT->CHANNEL[0].LDVAL = 0x00200000; // Set load value of zeroth PIT 
	
	//PIT1 initialization, modified from realtime lab
	PIT->CHANNEL[1].LDVAL = 0x19000; // Set load value of first PIT - set for 0.005s
	NVIC_EnableIRQ(PIT1_IRQn); /* enable PIT1 Interrupts */

	PIT->CHANNEL[0].TCTRL |= 0x3; // Enables timer 0 and enables interrupts
	PIT->CHANNEL[1].TCTRL |= 0x3; // Enables timer 1 and enables interrupts
}

void I2C_Initialize(void) {
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK; 							// enable clock to I2C0 module
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;							// enable clock to Port B (needed?)
	PORTB->PCR[2] = PORT_PCR_MUX(2);								// Set PTB2 to I2C0 operation
	PORTB->PCR[3] = PORT_PCR_MUX(2);								// Set PTB3 to I2C0 operation
	//I2C0->F = 0x1E;																	// 192 times clock prescaling yields ~100kHz
	I2C0->C1 |= (3 << 6);														// Enable I2C and interrupts
	//NVIC_EnableIRQ(I2C0_IRQn);											// Set IRQ handler for I2C0
}

/*----------------------------------------------------------------------------
  Function that toggles the red LED
 *----------------------------------------------------------------------------*/

void LEDRed_Toggle (void) {
	PTB->PTOR = 1 << 22; 	   /* Red LED Toggle */
}

/*----------------------------------------------------------------------------
  Function that toggles the blue LED
 *----------------------------------------------------------------------------*/
void LEDBlue_Toggle (void) {
	PTB->PTOR = 1 << 21; 	   /* Blue LED Toggle */
}

/*----------------------------------------------------------------------------
  Function that toggles the green LED
 *----------------------------------------------------------------------------*/
void LEDGreen_Toggle (void) {
	PTE->PTOR = 1 << 26; 	   /* Green LED Toggle */
}

/*----------------------------------------------------------------------------
  Function that turns on Red LED & all the others off
 *----------------------------------------------------------------------------*/
void LEDRed_On (void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTB->PCOR   = 1 << 22;   /* Red LED On*/
  PTB->PSOR   = 1 << 21;   /* Blue LED Off*/
  PTE->PSOR   = 1 << 26;   /* Green LED Off*/
	
	// Restore interrupts
	__set_PRIMASK(m);
}

/*----------------------------------------------------------------------------
  Function that turns on Green LED & all the others off
 *----------------------------------------------------------------------------*/
void LEDGreen_On (void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTB->PSOR   = 1 << 21;   /* Blue LED Off*/
  PTE->PCOR   = 1 << 26;   /* Green LED On*/
  PTB->PSOR   = 1 << 22;   /* Red LED Off*/
	
	// Restore interrupts
	__set_PRIMASK(m);
}

/*----------------------------------------------------------------------------
  Function that turns on Blue LED & all the others off
 *----------------------------------------------------------------------------*/
void LEDBlue_On (void) {
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTE->PSOR   = 1 << 26;   /* Green LED Off*/
  PTB->PSOR   = 1 << 22;   /* Red LED Off*/
  PTB->PCOR   = 1 << 21;   /* Blue LED On*/
	
	// Restore interrupts
	__set_PRIMASK(m);
}

/*----------------------------------------------------------------------------
  Function that turns all LEDs off
 *----------------------------------------------------------------------------*/
void LED_Off (void) {	
	// Save and disable interrupts (for atomic LED change)
	uint32_t m;
	m = __get_PRIMASK();
	__disable_irq();
	
  PTB->PSOR   = 1 << 22;   /* Green LED Off*/
  PTB->PSOR   = 1 << 21;   /* Red LED Off*/
  PTE->PSOR   = 1 << 26;   /* Blue LED Off*/
	
	// Restore interrupts
	__set_PRIMASK(m);
}

void delay(void){
	int j;
	for(j=0; j<1000000; j++);
}
