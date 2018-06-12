#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "string.h"
#include "keyboard.h"
#include "led.h"
#include "queue.h"

#define QUEUE_LENGTH 5
#define QUEUE_ITEM_SIZE 25

void  Rtos_Transmiter_SendString(char *cStrToSend){
			Transmiter_SendString(cStrToSend);
			while (Transmiter_GetStatus()!=FREE){};
}
	
void LettersTx (void *pvParameters){
	QueueHandle_t xQueue = *((QueueHandle_t*)pvParameters);

	unsigned int uiStartTime;
	unsigned int uiStopTime;
	unsigned int uiDelta = 0;
	char cStringToSend[QUEUE_ITEM_SIZE];
	char cDelta[10];
	
	LedInit();
	
	while(1){
		UIntToHexStr(uiDelta,cDelta);
		cStringToSend[0]=NULL;
		AppendString("-ABCDEEFGH-:", cStringToSend);
		AppendString(cDelta,cStringToSend);
		AppendString("\n",cStringToSend);
		
		uiStartTime = xTaskGetTickCount();
		if(xQueueSendToBack(xQueue, cStringToSend, 10) != pdPASS){
			LedToggle(0);
		}
		uiStopTime=xTaskGetTickCount();
		uiDelta = uiStopTime - uiStartTime;
		vTaskDelay(500);
	}
}

void KeyboardTx (void *pvParameters){
	QueueHandle_t xQueue = *((QueueHandle_t*)pvParameters);
	char cStringToSend[QUEUE_ITEM_SIZE];
	cStringToSend[0]=NULL;
	AppendString("-Keyboard-\n", cStringToSend);
	
	KeyboardInit();
	
	while(1){
		if(eKeyboardRead() != RELASED){
			xQueueSendToBack(xQueue, "-Keyboard-\n", 10);
		}
		vTaskDelay(300);
	}
}

void QueueTx(void *pvParameters){
	QueueHandle_t xQueue = *((QueueHandle_t*)pvParameters);
	char cStringToTransmit[QUEUE_ITEM_SIZE];
	while(1){
	if( xQueueReceive( xQueue, cStringToTransmit, portMAX_DELAY) == pdPASS);
		Rtos_Transmiter_SendString(cStringToTransmit);
	}
}


int main( void ){
	QueueHandle_t xQueue;
	xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	UART_InitWithInt(300);
	xTaskCreate(LettersTx, NULL, 128, &xQueue, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, &xQueue, 1, NULL );
	xTaskCreate(QueueTx, NULL, 128, &xQueue, 1, NULL );
	vTaskStartScheduler();
	while(1);
}
