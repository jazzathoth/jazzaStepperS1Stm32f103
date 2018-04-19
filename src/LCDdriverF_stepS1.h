/**
  ******************************************************************************
  * @file    LCDdriverF.h
  * @author  Joe Bender
  * @version V0.2
  * @date    March 2018
  * @brief   Header file for 16x2 LCD Display.
  ******************************************************************************
*/

#ifndef LCDFunctionsHeader
#include "stm32f1xx.h"
#include "stdio.h"

#define LCDFunctionsHeader

#define LCDpin0 12
#define LCDpin1 13
#define LCDpin2 14
#define LCDpin3 15
#define LCDpin4 6
#define LCDpin5 7
#define LCDpin6 8
#define LCDpin7 9
#define LCDpin0port GPIOB
#define LCDpin1port GPIOB
#define LCDpin2port GPIOB
#define LCDpin3port GPIOB
#define LCDpin4port GPIOB
#define LCDpin5port GPIOB
#define LCDpin6port GPIOB
#define LCDpin7port GPIOB

#define LCDenpin 11
#define LCDenport GPIOB
#define LCDrwpin 10
#define LCDrwport GPIOB
#define LCDrspin 1
#define LCDrsport GPIOB

#define delayBTD 30
#define disDelayBTD 600

//LCD command definitions 8 bit
#define LCDfxSet8b2l 0b00111000    			//lcd command to set mode to 8 bit input, 2 lines
#define LCDdspOnCrsrOn 0b00001110  			//lcd command to turn on display and turn on cursor, blink off
#define LCDentryMode_1 0b00000110			//lcd command move cursor to right 1 for next letter display shift stationary
#define LCDclearDsp 0b00000001				//lcd command clears the display... what else
#define LCDmoveCrsr_L 0b00010000			//lcd command move cursor left
#define LCDmoveCrsr_R 0b00010100			//lcd command move cursor right
#define LCDsetEntr_L1B1 0b10000000			//lcd move cursor/current data entry byte to 1st byte of 1st line. Increment by 1 to move to next byte
#define LCDsetEntr_L2B1 0b11000000			//lcd move cursor/current data entry byte to 1st byte of 2nd line. Increment by 1 to move to next byte


void badTimDelLCD(int BTD)
{
	volatile int i;
	for (i = 0;i < BTD; i++)
	{

	}
}

void setPortNPinsToOutLCD(GPIO_TypeDef *port, int pinNum)
{
	if(port == GPIOA)
	{
		RCC ->APB2ENR |= RCC_APB2ENR_IOPAEN;
	}
	if(port == GPIOB)
	{
		RCC ->APB2ENR |= RCC_APB2ENR_IOPBEN;
	}
	if(port == GPIOC)
	{
		RCC ->APB2ENR |= RCC_APB2ENR_IOPCEN;
	}

	if(pinNum <= 7)
	{
		port ->CRL &= ~(0x3U << ((pinNum * 4)+2));
		port ->CRL |= (0x3U << (pinNum * 4)); //set mode to output @ 50mhz pb6-7
	}
	if(pinNum >= 8)
	{
		port ->CRH &= ~(0x3U << (((pinNum - 8) * 4)+2));
		port ->CRH |= (0x3U << ((pinNum - 8) * 4));//set mode to output @ 50mhz pb8-9, 12-15
	}

}

void initializePortsLCD(void)
{
	setPortNPinsToOutLCD(LCDpin0port, LCDpin0);
	setPortNPinsToOutLCD(LCDpin1port, LCDpin1);
	setPortNPinsToOutLCD(LCDpin2port, LCDpin2);
	setPortNPinsToOutLCD(LCDpin3port, LCDpin3);
	setPortNPinsToOutLCD(LCDpin4port, LCDpin4);
	setPortNPinsToOutLCD(LCDpin5port, LCDpin5);
	setPortNPinsToOutLCD(LCDpin6port, LCDpin6);
	setPortNPinsToOutLCD(LCDpin7port, LCDpin7);

	setPortNPinsToOutLCD(LCDenport, LCDenpin);
	setPortNPinsToOutLCD(LCDrwport, LCDrwpin);
	setPortNPinsToOutLCD(LCDrsport, LCDrspin);
}

void sendBitToPortnPin(GPIO_TypeDef *port,int pinNum, uint8_t bState)
{
	if (bState)
	{
		port ->BSRR |= (1 << pinNum);
	}
	else
	{
		port ->BRR |= (1 << pinNum);
	}
}

void enAndDelLCD()
{
	badTimDelLCD(delayBTD);
	sendBitToPortnPin(LCDenport, LCDenpin, 1);

}

void setWriteLCD()
{
	sendBitToPortnPin(LCDrwport, LCDrwpin, 0);
}

void setReadLCD()
{
	sendBitToPortnPin(LCDrwport, LCDrwpin, 1);
}

void cmdModeLCD()
{
	sendBitToPortnPin(LCDrsport, LCDrspin, 0);
}

void charModeLCD()
{
	sendBitToPortnPin(LCDrsport, LCDrspin, 1);
}


void sendByteToLCDpins(char character)
{
	sendBitToPortnPin(LCDpin0port, LCDpin0, character & 0b00000001);
	sendBitToPortnPin(LCDpin1port, LCDpin1, character & 0b00000010);
	sendBitToPortnPin(LCDpin2port, LCDpin2, character & 0b00000100);
	sendBitToPortnPin(LCDpin3port, LCDpin3, character & 0b00001000);
	sendBitToPortnPin(LCDpin4port, LCDpin4, character & 0b00010000);
	sendBitToPortnPin(LCDpin5port, LCDpin5, character & 0b00100000);
	sendBitToPortnPin(LCDpin6port, LCDpin6, character & 0b01000000);
	sendBitToPortnPin(LCDpin7port, LCDpin7, character & 0b10000000);

	badTimDelLCD(disDelayBTD);
	sendBitToPortnPin(LCDenport, LCDenpin, 0);
	badTimDelLCD(disDelayBTD);

}

void sendCharToLCD(char character)
{
	setWriteLCD();
	charModeLCD();
	enAndDelLCD();
	sendByteToLCDpins(character);
}

void sendCmdLCD(uint8_t cmd2LCD)
{
	setWriteLCD();
	cmdModeLCD();
	enAndDelLCD();
	sendByteToLCDpins(cmd2LCD);
}


void sendStrLCD(char *charStrLCD)
{
	while(*charStrLCD)
	{
		sendCharToLCD(*charStrLCD++);
	}
}

void send2StringsLCD(char *str21, char *str22)
{
	sendCmdLCD(LCDclearDsp);
	setInputPointLCD(1, 0);
	sendStrLCD(str21);
	setInputPointLCD(2, 0);
	sendStrLCD(str22);
	badTimDelLCD(400000);
}

/*
 |*****Function setting up the display:*********|
 |*******Standard Setup Parameters:*************|
 |**********Initializes defined ports***********|
 |***Set display in 8 bit mode, 2 lines*********|
 |***Enables the display, cursor on solid*******|
 |***Entry mode 1: write next char to right*****|
 |**************Clears display******************|
 |****Sets entry point to 1st bit of 1st line***|
 |*********CALL FUNCTION ONCE IN MAIN.C*********|
 */
void stdSetupDisplayLCD(void)
{
	initializePortsLCD();
	sendCmdLCD(LCDfxSet8b2l);
	sendCmdLCD(LCDdspOnCrsrOn);
	sendCmdLCD(LCDentryMode_1);
	sendCmdLCD(LCDclearDsp);
	sendCmdLCD(LCDsetEntr_L1B1);
}

void setInputPointLCD(int lineNum, int byteNum)
{
	if (lineNum == 1)
	{
		int entrPtLCD = 128 + byteNum;
		sendCmdLCD(entrPtLCD);
	}
	if (lineNum == 2)
	{
		int entrPtLCD = 192 + byteNum;
		sendCmdLCD(entrPtLCD);
	}
}

void sendIntNumberToLCD(int displayThisIntNumLCD, uint8_t MaxDigitsDisplay)
{
	char displayIntStr[MaxDigitsDisplay];
	snprintf(displayIntStr, MaxDigitsDisplay, "%d", displayThisIntNumLCD);
	sendStrLCD(displayIntStr);
}

//To fix float reference error add “-u _printf_float” flag to G++ linker (misc section) in project properties

void sendFlNumberToLCD(float displayThisFloatNumLCD, uint8_t MaxDigitsDisplay)
{
	char displayFloatStr[MaxDigitsDisplay];
	snprintf(displayFloatStr,MaxDigitsDisplay + 1, "%f", displayThisFloatNumLCD);
	sendStrLCD(displayFloatStr);
}
#endif

