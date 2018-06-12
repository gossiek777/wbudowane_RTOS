#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "string.h"
#include "keyboard.h"
#include "semphr.h"


void LettersTx (void *pvParameters){
	xSemaphoreHandle xSem= *((xSemaphoreHandle*)pvParameters);
	while(1){
		if(xSemaphoreTake(xSem,portMAX_DELAY) == pdTRUE){
			Transmiter_SendString("-ABCDEEFGH-\n");
			while (Transmiter_GetStatus()!=FREE){};
			xSemaphoreGive(xSem);
		}
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
	xSemaphoreHandle xSem= *((xSemaphoreHandle*)pvParameters);
	KeyboardInit();
	while(1){
		if((eKeyboardRead() != RELASED)&&(xSemaphoreTake(xSem,portMAX_DELAY) == pdTRUE)){
			Transmiter_SendString("-Keyborad-\n");
			while (Transmiter_GetStatus()!=FREE){};
			xSemaphoreGive(xSem);
		}
		vTaskDelay(300);
	}
}

int main( void ){
	xSemaphoreHandle xSemaphore;
	vSemaphoreCreateBinary (xSemaphore);
	
	UART_InitWithInt(300);
	xTaskCreate(LettersTx, NULL, 128, &xSemaphore, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, &xSemaphore, 1, NULL );
	vTaskStartScheduler();
	while(1);
}
