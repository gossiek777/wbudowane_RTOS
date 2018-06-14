#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "watch.h"
#include "keyboard.h"
#include "led.h"
#include "string.h"
#include "uart.h"

#define QUEUE_LENGTH 			5
#define QUEUE_ITEM_SIZE 	15

QueueHandle_t xUART_Queue;

void Button_To_Led(void *pvParameters){
	enum ButtonState eButton;
	enum ButtonState ePreviousButton = RELASED;
	while(1){
		eButton = eKeyboard_Read();
		if(eButton != ePreviousButton){
			switch(eButton){
				case(BUTTON_0):
					Led_On(0);
					xQueueSendToBack(xUART_Queue,"button 0x0000",1);
					break;
				case(BUTTON_1):
					Led_On(1);
					xQueueSendToBack(xUART_Queue,"button 0x0001",1);
					break;
				case(BUTTON_2):
					Led_On(2);
					xQueueSendToBack(xUART_Queue,"button 0x0002",1);
					break;
				case(BUTTON_3):
					Led_On(3);
					xQueueSendToBack(xUART_Queue,"button 0x0003",1);
					break;
				default:
					break;
			}
		}
		ePreviousButton=eButton;
		vTaskDelay(10);
	}
}

void WatchEvent_To_Led(void *pvParameters){
	struct WatchEvent sWatch;
	char cMessage[15];
	while(1){
		cMessage[0] = NULL;
		sWatch = sWatch_Read();
		if(sWatch.eTimeUnit == SECONDS){
			Led_Toggle(0);
			AppendString("sec ", cMessage);
		}
		else{
			Led_Toggle(1);
			AppendString("min ", cMessage);
		}
		AppendUIntToString(sWatch.TimeValue, cMessage);
		xQueueSendToBack(xUART_Queue,&cMessage,1);
		vTaskDelay(10);
	}
}

void UART_Transmit_Msg(void *pvParameters){
	char cMsg[15];
	while(1){
		xQueueReceive(xUART_Queue,&cMsg,portMAX_DELAY);
		Uart_PutString(cMsg);
	}
}

int main( void ){

	Led_Init();
	Watch_Init();
	Keyboard_Init();
	UART_InitWithInt(9600);
	xUART_Queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	xTaskCreate(WatchEvent_To_Led, NULL, 128, NULL, 1, NULL);
	xTaskCreate(Button_To_Led, NULL, 128, NULL, 1, NULL);
	xTaskCreate(UART_Transmit_Msg, NULL, 128, NULL, 1, NULL);
	vTaskStartScheduler();	
}
