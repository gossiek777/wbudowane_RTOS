#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void Delay(unsigned int uiMiliSec) {
	unsigned int uiLoopCtr, uiDelayLoopCount;
	uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr = 0; uiLoopCtr < uiDelayLoopCount; uiLoopCtr++) {}
} 

void Led0Blink( void *pvParameters ){
	LedOn(0);
	while(1){
		Delay(1000);
		LedToggle(0);
		vTaskDelay(1000);
		LedToggle(0);
	}
} 

void Led1Blink( void *pvParameters ){
	while(1){
		LedToggle(1);
		vTaskDelay(50);
	}
} 

int main(void){
	LedInit();
	xTaskCreate(Led0Blink, NULL , 100 , NULL, 2 , NULL );
	xTaskCreate(Led1Blink, NULL , 100 , NULL, 10 , NULL );
	vTaskStartScheduler();
	while(1);
}
