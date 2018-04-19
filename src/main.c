/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f1xx.h"
#include "LCDdriverF_stepS1.h"
#include "StepperA4988drv_stepS1.h"
			

int main(void)
{
	stdSetupDisplayLCD();

	//enable timer 2 and enable interrupts after each reload
	RCC ->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2 ->PSC = 80;
	TIM2 ->ARR = 100;
	TIM2 ->CR1 |= TIM_CR1_ARPE;
	TIM2 ->DIER |= TIM_DIER_UIE;
	TIM2 ->CR1 |= TIM_CR1_CEN;

	//adc Setup
	ADC1 ->CR2 &= ~ADC_CR2_ADON;

	send2StringsLCD("Starting ADC    ", "Calibration     ");

	ADC1 ->CR2 |= ADC_CR2_CAL;

	while ((ADC1 ->CR2 & ADC_CR2_CAL) != 0)
	{}

	send2StringsLCD("Calibration Good", "*               *");

	RCC ->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC ->CR |= RCC_CR_HSION;

	while ((RCC ->CR & RCC_CR_HSIRDY) == 0)
	{}
	send2StringsLCD("Clock Started   ", "Starting GetTemp");
	badTimDelLCD(1000000);

	RCC ->CFGR &= ~RCC_CFGR_ADCPRE;
	ADC1 ->CR2 |= ADC_CR2_ADON;
	ADC1 ->SQR3 &= ~ADC_SQR1_L;
	ADC1 ->SQR3 |= ADC_SQR3_SQ1_3;

	ADC1 ->SMPR1 |= ADC_SAMPLETIME_239CYCLES5_SMPR1ALLCHANNELS;

	ADC1 ->CR2 |= ADC_CR2_ADON;

	sendCmdLCD(LCDclearDsp);

	initializeOutputsSTEP();
	driverEnableSTEP(0);
	setMicroSTEP(4);
	setDirectionLeftSTEP();

	NVIC_EnableIRQ(TIM2_IRQn);

	while(1)
	{
		while ((ADC1 ->SR & ADC_SR_EOC) == 0)
		{}
		int read = ADC1->DR;
		sendCmdLCD(LCDsetEntr_L1B1);
		sendStrLCD("Potentiometer = ");
		sendCmdLCD(LCDsetEntr_L2B1);
		sendIntNumberToLCD(read, 8);
		TIM2 ->PSC = (read / 10);
	}
}

void TIM2_IRQHandler(void)
{
	if(TIM2 ->SR & TIM_SR_UIF)
	{
		TIM2 ->SR &= ~TIM_SR_UIF;
		moveOneSTEP();
	}
}
