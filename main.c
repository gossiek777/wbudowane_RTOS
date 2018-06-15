#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "keyboard.h"
#include "led.h"
#include "string.h"
#include "uart.h"
#include "command_decoder.h"
#include "servo.h"


#define QUEUE_LENGTH 			25
#define QUEUE_ITEM_SIZE 	20

QueueHandle_t xCommon_Queue;

void Button_To_Led(void *pvParameters){
	enum ButtonState eButton;
	enum ButtonState ePreviousButton = RELASED;
	while(1){
		eButton = eKeyboard_Read();
		if(eButton != ePreviousButton){
			switch(eButton){
				case(BUTTON_0):
					Led_On(0);
					xQueueSendToBack(xCommon_Queue,"button 0x0000",1);
					break;
				case(BUTTON_1):
					Led_On(1);
					xQueueSendToBack(xCommon_Queue,"button 0x0001",1);
					break;
				case(BUTTON_2):
					Led_On(2);
					xQueueSendToBack(xCommon_Queue,"button 0x0002",1);
					break;
				case(BUTTON_3):
					Led_On(3);
					xQueueSendToBack(xCommon_Queue,"button 0x0003",1);
					break;
				default:
					break;
			}
		}
		ePreviousButton=eButton;
		vTaskDelay(10);
	}
}

void UART_Recieve_Msg(void *pvParameters){
	char cMsg[UART_RX_BUFFER_SIZE];
	while(1){
		Uart_GetString(cMsg);
		xQueueSendToBack(xCommon_Queue,&cMsg,1);
	}
}

void Executor (void *pvParameters){
	char cEvent[UART_RX_BUFFER_SIZE];
	char cID_Str [20];
	while(1){
		xQueueReceive(xCommon_Queue,&cEvent,portMAX_DELAY);
		DecodeMsg(cEvent);
		if ((ucTokenNr > 0) && (asToken[0].eType == KEYWORD)){
				switch (asToken[0].uValue.eKeyword){
					case CALLIB:
						ServoCallib();
						break;
					
					case GOTO:
						if (asToken[1].eType == NUMBER){
							ServoGoTo(asToken[1].uValue.uiNumber);
						}
						break;
					
					case ID:
						cID_Str[0]=NULL;
						CopyString("id: 0x0029",cID_Str); 
						Uart_PutString(cID_Str);
						break;
					
					case BUTTON:
						if (asToken[1].eType == NUMBER){
							switch(asToken[1].uValue.uiNumber){
								case 0:
									Uart_PutString("BUT_0");
									//ServoCallib();
									break;
								case 1:
									Uart_PutString("BUT_1");
									//ServoGoTo(12);
									break;
								case 2:
									Uart_PutString("BUT_2");
								//ServoGoTo(24);
									break;
								case 3:
									Uart_PutString("BUT_3");
									//ServoGoTo(36);
									break;
								default:
									break;
							}
						}
						break;
						
					default:
						Uart_PutString("Unknown command");
						break;
			}
		}
		vTaskDelay(10);
	}
}

int main(){
	Led_Init();
	Keyboard_Init();
	//ServoInit();
	UART_InitWithInt(9600);
	xCommon_Queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	xTaskCreate(Button_To_Led, NULL, 128, NULL, 1, NULL);
	xTaskCreate(UART_Recieve_Msg, NULL, 128, NULL, 1, NULL);
	xTaskCreate(Executor, NULL, 128, NULL, 1, NULL);
	//xTaskCreate(Automat, NULL, 512, NULL, 3, NULL);
	vTaskStartScheduler();	
}
