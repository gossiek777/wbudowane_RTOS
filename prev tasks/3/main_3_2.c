#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void Delay(unsigned int uiMiliSec) {
	unsigned int uiLoopCtr, uiDelayLoopCount;
	uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr = 0; uiLoopCtr < uiDelayLoopCount; uiLoopCtr++) {}
} 

void LedBlink( void *pvParameters ){
	while(1){
		unsigned char ucFreq = *((unsigned char*)pvParameters);
		LedToggle(0);
		vTaskDelay((1000/ucFreq)/2);
	}
}

void LedCtrl(void *pvParameters ){
	unsigned char ucFrequency = 1;
	while(1){
		vTaskDelay(1000);
		ucFrequency++;
		ucFrequency = ucFrequency%11;
		if (ucFrequency==0)
			ucFrequency=1;
		*((unsigned char*)pvParameters) = ucFrequency;
	}
}
int main( void ){
	unsigned char ucBlinkingFreq = 2;
	LedInit();
	xTaskCreate(LedBlink, NULL , 100 , &ucBlinkingFreq, 2 , NULL );
	xTaskCreate(LedCtrl, NULL , 100 , &ucBlinkingFreq, 2 , NULL );
	vTaskStartScheduler();	
	while(1);
}
