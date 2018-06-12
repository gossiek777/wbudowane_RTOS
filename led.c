#include <LPC21xx.H>
#include "led.h"

#define LED0_bm (1<<16)
#define LED1_bm (1<<17)
#define LED2_bm (1<<18)
#define LED3_bm (1<<19)

enum LedDirection {RIGHT, LEFT};

void Led_Init(void){
	IO1DIR = IO1DIR | (LED0_bm | LED1_bm | LED2_bm | LED3_bm);
	IO1SET = LED0_bm;
}

void Led_On (unsigned char ucLedIndex){
	IO1CLR = LED0_bm | LED1_bm | LED2_bm | LED3_bm;
	switch(ucLedIndex){
		case(0):
			IO1SET = LED0_bm;
			break;
		case(1):
			IO1SET = LED1_bm;
			break;
		case(2):
			IO1SET = LED2_bm;
			break;
		case(3):
			IO1SET = LED3_bm;
			break;
		default:
			break;
	}
}

void Led_Off (void){
	IO1CLR = LED0_bm | LED1_bm | LED2_bm | LED3_bm;
}

void Led_Step(enum LedDirection eLedDir){
	static unsigned int uiLightingLed;
	
	if (eLedDir == LEFT) {
		uiLightingLed++;
	}
	else{
		uiLightingLed--;
	}
	Led_On(uiLightingLed%4);		
}

void Led_StepLeft(void){
	Led_Step(LEFT);
}

void Led_StepRight (void){
	Led_Step(RIGHT);
}

void Led_Toggle (unsigned char ucLedIndex){
	
	unsigned int uiLedBitMask = 1<<(16+ucLedIndex);
	
	if(IO1PIN & uiLedBitMask)
		IO1CLR = uiLedBitMask;
	else
		IO1SET = uiLedBitMask;
}
