/*
 * dwenguinoBoard.h
 *
 *  Created on: Jan 19, 2016
 *      Author: Tom Neutens
 */

#ifndef DWENGUINO_BOARD_H_
#define DWENGUINO_BOARD_H_

//	Constants
#define TRUE		1
#define FALSE		0
#define HIGH		1
#define LOW			0
#define PORT_HIGH	0xFF
#define PORT_LOW	0x00
#define INPUT		0
#define OUTPUT		1

#define SET_PIN_HIGH(PORT, PIN)	PORT |= (1 << PIN)
#define SET_PIN_LOW(PORT, PIN)	PORT &= ~(1 << PIN)

#define SET_BIT_HIGH(REG, BIT)	REG |= (1 << BIT)
#define SET_BIT_LOW(REG, BIT)	REG &= ~(1 << BIT)

//	Data types
#define BYTE unsigned char

//	Pin assingments

//	LEDs

#define LEDS_DIR		DDRA
#define LEDS			PORTA
#define LED_ON(LED)		SET_PIN_HIGH(PORTA, LED)
#define LED_OFF(LED)	SET_PIN_LOW(PORTA, LED)

//	Buttons
#define SW_C_HIGH	SET_PIN_HIGH(PORTC, 6)
#define SW_C_LOW	SET_PIN_LOW(PORTC, 6)
#define SW_C_IN		SET_PIN_LOW(DDRC, 6)
#define SW_C_OUT	SET_PIN_HIGH(DDRC, 6)

#define SW_W_HIGH	SET_PIN_HIGH(PORTE, 4)
#define SW_W_LOW	SET_PIN_LOW(PORTE, 4)
#define SW_W_IN		SET_PIN_LOW(DDRE, 4)
#define SW_W_OUT	SET_PIN_HIGH(DDRE, 4)

#define SW_S_HIGH	SET_PIN_HIGH(PORTE, 5)
#define SW_S_LOW	SET_PIN_LOW(PORTE, 5)
#define SW_S_IN		SET_PIN_LOW(DDRE, 5)
#define SW_S_OUT	SET_PIN_HIGH(DDRE, 5)

#define SW_E_HIGH	SET_PIN_HIGH(PORTE, 6)
#define SW_E_LOW	SET_PIN_LOW(PORTE, 6)
#define SW_E_IN		SET_PIN_LOW(DDRE, 6)
#define SW_E_OUT	SET_PIN_HIGH(DDRE, 6)

#define SW_N_HIGH	SET_PIN_HIGH(PORTE, 7)
#define SW_N_LOW	SET_PIN_LOW(PORTE, 7)
#define SW_N_IN		SET_PIN_LOW(DDRE, 6)
#define SW_N_OUT	SET_PIN_HIGH(DDRE, 6)

//	LCD display
#define LCD_DATA			PORTA
#define LCD_DATA_DIR		DDRA

#define LCD_BACKLIGHT_ON	SET_PIN_HIGH(PORTE, 3)
#define LCD_BACKLIGHT_OFF	SET_PIN_LOW(PORTE, 3)
#define LCD_BACKLIGHT_OUT	SET_PIN_HIGH(DDRE, 3)
#define LCD_BACKLIGHT_IN	SET_PIN_LOW(DDRE, 3)

#define LCD_RW_HIGH			SET_PIN_HIGH(PORTE, 1)
#define LCD_RW_LOW			SET_PIN_LOW(PORTE, 1)
//#define LCD_RW_IN			DDRE |= (1 << PE1)
#define LCD_RW_OUT			SET_PIN_HIGH(DDRE, 1)

#define LCD_RS_HIGH			SET_PIN_HIGH(PORTE, 0)
#define LCD_RS_LOW			SET_PIN_LOW(PORTE, 0)
//#define LCD_RS_IN			DDRE |= (1 << PE0)
#define LCD_RS_OUT			SET_PIN_HIGH(DDRE, 0)

#define LCD_EN_HIGH			SET_PIN_HIGH(PORTE, 2)
#define LCD_EN_LOW			SET_PIN_LOW(PORTE, 2)
//#define LCD_EN_IN			DDRE |= (1 << PE2)
#define LCD_EN_OUT			SET_PIN_HIGH(DDRE, 2)

//	Servo connectors
#define SERVO1	PORTC0
#define SERVO2	PORTC1

//	Motor driver
#define MOTOR1_0_HIGH	SET_PIN_HIGH(PORTC, 3)
#define MOTOR1_0_LOW	SET_PIN_LOW(PORTC, 3)

#define MOTOR1_1_HIGH	SET_PIN_HIGH(PORTC, 4)
#define MOTOR1_1_LOW	SET_PIN_LOW(PORTC, 4)

#define MOTOR2_0_HIGH	SET_PIN_HIGH(PORTC, 2)
#define MOTOR2_0_LOW	SET_PIN_LOW(PORTC, 2)

#define MOTOR2_1_HIGH	SET_PIN_HIGH(PORTC, 5)
#define MOTOR2_1_LOW	SET_PIN_LOW(PORTC, 5)


//	Include libraries
#include <avr/io.h>
#include <avr/delay.h>

#ifndef NO_LCD
#include "dwenguinoLCD.h"
#endif

//	Initialisation routine for the board
void initBoard(void);

#endif	//DWENGUINO_BOARD_H_


