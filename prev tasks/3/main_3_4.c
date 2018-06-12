#include "FreeRTOS.h"
#include "task.h"
#include "led.h"


void Delay(unsigned int uiMiliSec) {
	unsigned int uiLoopCtr, uiDelayLoopCount;
	uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr = 0; uiLoopCtr < uiDelayLoopCount; uiLoopCtr++) {}
} 

void LedBlink( void *pvParameters ){
	unsigned char ucFreq = *((unsigned char*)pvParameters);
	while(1){
	LedToggle(0);
	vTaskDelay((1000/ucFreq)/2);
	}
}

void LedCtrl( void *pvParameters){
	TaskHandle_t tBlinkHandler = *((TaskHandle_t*)pvParameters);
	unsigned char ucSuspendBlinking = 0;
	while(1){
		if(ucSuspendBlinking)
			vTaskSuspend(tBlinkHandler);
		else
			vTaskResume(tBlinkHandler);
		ucSuspendBlinking =!(ucSuspendBlinking);
		vTaskDelay(1000);
	}
}

int main( void ){
	unsigned char ucBlinkingFreq = 10;
	TaskHandle_t tHandle;

	LedInit();
	xTaskCreate(LedBlink, NULL , 100 , &ucBlinkingFreq, 2 , &tHandle);
	xTaskCreate(LedCtrl, NULL , 100 , &tHandle, 2 , NULL);
	vTaskStartScheduler();
	while(1);
}

