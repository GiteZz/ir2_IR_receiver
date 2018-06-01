/*
 * dwenguinoBoard.c
 *
 *  Created on: Jan 19, 2016
 *      Author: Tom
 */


#include "../HeaderFiles/dwenguinoBoard.h"

void initBoard(void){

	//if LCD display is connected (default)
#ifndef	NO_LCD
	initLCD();
	clearLCD();
	backlightOff();
#endif

	//	Set buttons as inputs

	SW_C_IN;
	SW_N_IN;
	SW_E_IN;
	SW_S_IN;
	SW_W_IN;

	//	Enalbe pullups for the switches

	SW_C_HIGH;
	SW_N_HIGH;
	SW_E_HIGH;
	SW_S_HIGH;
	SW_W_HIGH;

	//	Set led pins as output

	LEDS_DIR = PORT_HIGH;	//LED pins as output
	LEDS = PORT_LOW;		//Turn LEDs off






}
