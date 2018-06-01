/*
 * dwenguinoLCD.h
 *
 *  Created on: Jan 19, 2016
 *      Author: Tom Neutens
 */

#ifndef DWENGUINO_LCD_H_
#define DWENGUINO_LCD_H_


#include "dwenguinoBoard.h"
#include <avr/delay.h>

//	Properties
#define LCD_WIDTH		16
#define LCD_HEIGHT		2
#define LCD_LASTLINE	(LCD_HEIGHT - 1)
#define LCD_LASTPOS		(LCD_WIDTH - 1)

//	Macro's
#define backlightOn()	(LCD_BACKLIGHT_ON)
#define backlightOff()	(LCD_BACKLIGHT_OFF)

#define appendStringToLCD(message)	appendStringToLCD_((const char*)(message))

//	Functions
void initLCD(void);
void clearLCD(void);

void commandLCD(const BYTE c);
void setCursorLCD(BYTE l, BYTE p);

void appendCharToLCD(const char c);
void printCharToLCD(const char s, BYTE l, BYTE p);

void appendIntToLCD(int i);
void printIntToLCD(int i, BYTE l, BYTE p);

//	Structures
struct lcd_info_type{
	unsigned char line;
	unsigned char pos;
};

//	Keeps track of current line number and character position.
extern struct lcd_info_type lcd_info;

#endif /* DWENGUINO_LCD_H_ */
