/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Header File for stepper
  ******************************************************************************
*/
#ifndef StepperDriverA4988

#include "stm32f1xx.h"
#include "stdio.h"

#define StepperDriverA4988

#define STEPdir 2
#define STEPstep 3
#define STEPms3 4
#define STEPms2 5
#define STEPms1 6
#define STEPena 7
#define STEPdirPort GPIOA
#define STEPstepPort GPIOA
#define STEPms3Port GPIOA
#define STEPms2Port GPIOA
#define STEPms1Port GPIOA
#define STEPenaPort GPIOA

void setPortNPinsToOutSTEP(GPIO_TypeDef *port, int pinNum)
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

void initializeOutputsSTEP(void)
{
	setPortNPinsToOutSTEP(STEPdirPort, STEPdir);
	setPortNPinsToOutSTEP(STEPstepPort, STEPstep);
	setPortNPinsToOutSTEP(STEPms3Port, STEPms3);
	setPortNPinsToOutSTEP(STEPms2Port, STEPms2);
	setPortNPinsToOutSTEP(STEPms1Port, STEPms1);
	setPortNPinsToOutSTEP(STEPenaPort, STEPena);
}

//set step mode; 0 = whole step, 4 = 1/16 step
void setMicroSTEP(int microstep)
{
	if(microstep == 0)
	{
		STEPms1Port ->BSRR |= (1 << (STEPms1 + 16));
		STEPms2Port ->BSRR |= (1 << (STEPms2 + 16));
		STEPms3Port ->BSRR |= (1 << (STEPms3 + 16));
	}
	if(microstep == 1)
	{
		STEPms1Port ->BSRR |= (1 << STEPms1);
		STEPms2Port ->BSRR |= (1 << (STEPms2 + 16));
		STEPms3Port ->BSRR |= (1 << (STEPms3 + 16));
	}
	if(microstep == 2)
	{
		STEPms1Port ->BSRR |= (1 << (STEPms1 + 16));
		STEPms2Port ->BSRR |= (1 << STEPms2);
		STEPms3Port ->BSRR |= (1 << (STEPms3 + 16));
	}
	if(microstep == 3)
	{
		STEPms1Port ->BSRR |= (1 << STEPms1);
		STEPms2Port ->BSRR |= (1 << STEPms2);
		STEPms3Port ->BSRR |= (1 << (STEPms3 + 16));
	}
	if(microstep == 4)
	{
		STEPms1Port ->BSRR |= (1 << STEPms1);
		STEPms2Port ->BSRR |= (1 << STEPms2);
		STEPms3Port ->BSRR |= (1 << STEPms3);
	}
}

//send 1 to enable or 0 to disable
void driverEnableSTEP(uint8_t enableS)
{
	if (enableS)
	{
		STEPenaPort ->BSRR |= (1 << STEPena);
	}
	else
	{
		STEPenaPort ->BRR |= (1 << STEPena);
	}
}


void setDirectionRightSTEP(void)
{
	STEPdirPort ->BSRR |= (1 << STEPdir);
}

void setDirectionLeftSTEP(void)
{
	STEPdirPort ->BRR |= (1 << STEPdir);
}

void moveOneSTEP(void)
{
	STEPstepPort ->ODR ^= (1 << STEPstep);
}




#endif
