#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "timer_interrupts.h"
#include "semphr.h"

SemaphoreHandle_t xSemHandler;

void LedBlink(void *pvParameters){
	while(1){
		xSemaphoreTake(xSemHandler,portMAX_DELAY);
		LedToggle(0);
	}
} 

void HandleTimer(){
	xSemaphoreGiveFromISR(xSemHandler, NULL);
}

int main(void){
	xSemHandler = xSemaphoreCreateBinary();
	LedInit();
	Timer1Interrupts_Init(500000, &HandleTimer);
	xTaskCreate(LedBlink,NULL,128,NULL,1,NULL);
	vTaskStartScheduler();
	while(1);
}
