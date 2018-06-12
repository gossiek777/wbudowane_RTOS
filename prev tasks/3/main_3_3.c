#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

struct sLedParams{
	unsigned char ucBlinkingFreq;
	unsigned char ucBlinkingLed;
};

void Delay(unsigned int uiMiliSec) {
	unsigned int uiLoopCtr, uiDelayLoopCount;
	uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr = 0; uiLoopCtr < uiDelayLoopCount; uiLoopCtr++) {}
} 

void LedBlink( void *pvParameters ){
	while(1){
		struct sLedParams sFunctParams = *((struct sLedParams*)pvParameters);
		LedToggle(sFunctParams.ucBlinkingLed);
		vTaskDelay((1000/sFunctParams.ucBlinkingFreq)/2);
	}
}

void LedCtrl(void *pvParameters ){
	unsigned char ucFrequency = 1;
	unsigned char ucLed = 0;
	struct sLedParams sFunctParams;
	while(1){
		vTaskDelay(1000);
		ucFrequency++;
		ucFrequency = ucFrequency%11;
		if (ucFrequency==0)
			ucFrequency=1;
		if (ucFrequency%2==0){
			ucLed++;
			ucLed=ucLed%4;
			LedOff();
		}
		sFunctParams.ucBlinkingFreq = ucFrequency;
		sFunctParams.ucBlinkingLed = ucLed;
		*((struct sLedParams*)pvParameters) = sFunctParams;
	}
}
int main( void ){
	struct sLedParams sParams;
	sParams.ucBlinkingFreq = 1;
	sParams.ucBlinkingLed = 0;
	LedInit();
	xTaskCreate(LedBlink, NULL , 100 , &sParams, 2 , NULL );
	xTaskCreate(LedCtrl, NULL , 100 , &sParams, 2 , NULL );
	vTaskStartScheduler();	
	while(1);
}
