#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "string.h"
#include "keyboard.h"
#include "semphr.h"

void  Rtos_Transmiter_SendString(char *cStrToSend, xSemaphoreHandle xSem){
	if(xSemaphoreTake(xSem,portMAX_DELAY) == pdTRUE){
			Transmiter_SendString(cStrToSend);
			while (Transmiter_GetStatus()!=FREE){};
			xSemaphoreGive(xSem);
		}}
	
void LettersTx (void *pvParameters){
	xSemaphoreHandle xSem= *((xSemaphoreHandle*)pvParameters);
	unsigned int uiStartTime;
	unsigned int uiStopTime;
	unsigned int uiDelta = 0;
	char cStringToSend[25];
	char cDelta[10];
	while(1){
		UIntToHexStr(uiDelta,cDelta);
		cStringToSend[0]=NULL;
		AppendString("-ABCDEEFGH-:", cStringToSend);
		AppendString(cDelta,cStringToSend);
		AppendString("\n",cStringToSend);
		
		uiStartTime = xTaskGetTickCount();
		Rtos_Transmiter_SendString(cStringToSend, xSem);
		uiStopTime=xTaskGetTickCount();
		uiDelta = uiStopTime - uiStartTime;
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
	xSemaphoreHandle xSem= *((xSemaphoreHandle*)pvParameters);
	KeyboardInit();
	while(1){
		if(eKeyboardRead() != RELASED){
			Rtos_Transmiter_SendString("-Keyborad-\n", xSem);
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
