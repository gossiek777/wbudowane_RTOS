#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"



void PulseTrigger( void *pvParameters ){
	xSemaphoreHandle xSem= *((xSemaphoreHandle*)pvParameters);
	while(1) {
		vTaskDelay(1000);
		xSemaphoreGive(xSem);
	}
}

void PulseTrigger2( void *pvParameters ){
	xSemaphoreHandle xSem= *((xSemaphoreHandle*)pvParameters);
	vTaskDelay(1000/3);
	while(1) {
		vTaskDelay(1000/3);
		xSemaphoreGive(xSem);
	}
}

void Pulse_LED0( void *pvParameters ){
	xSemaphoreHandle xSem= *((xSemaphoreHandle*)pvParameters);
	while(1) {
		if(xSemaphoreTake(xSem,portMAX_DELAY) == pdTRUE){
			LedOn(0);
			vTaskDelay(100);
			LedOff();
		}
	}
}

int main( void ){
	xSemaphoreHandle xSemaphore;
	vSemaphoreCreateBinary (xSemaphore);
	LedInit();
	
	xTaskCreate(PulseTrigger, NULL , 100 , &xSemaphore, 2 , NULL);
	xTaskCreate(PulseTrigger2, NULL , 100 , &xSemaphore, 2 , NULL);
	xTaskCreate(Pulse_LED0, NULL , 100 , &xSemaphore, 2 , NULL);
	vTaskStartScheduler();
	while(1);
}

