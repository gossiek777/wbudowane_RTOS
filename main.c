#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "keyboard.h"
#include "led.h"
#include "string.h"
#include "uart.h"
#include "command_decoder.h"
#include "servo.h"


#define QUEUE_LENGTH 			5
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
	char cString [30];
	struct ServoStatus sServoStatus;
	while(1){
		xQueueReceive(xCommon_Queue,&cEvent,portMAX_DELAY);
		DecodeMsg(cEvent);
		cString[0]=NULL;
		if ((ucTokenNr > 0) && (asToken[0].eType == KEYWORD)){
				switch (asToken[0].uValue.eKeyword){
					case CALLIB:
						ServoCallib();
						Uart_PutString("CALLIB");
						break;
					
					case GOTO:
						if (asToken[1].eType == NUMBER){
							Uart_PutString("GO_TO");
							ServoGoTo(asToken[1].uValue.uiNumber);
						}
						break;
					
					case ID:
						Uart_PutString("id: 0x0029");
						break;
					
					case BUTTON:
						if (asToken[1].eType == NUMBER){
							switch(asToken[1].uValue.uiNumber){
								case 0:
									ServoCallib();
									Uart_PutString("BUT_0");
									break;
								case 1:
									ServoWait(12);
									Uart_PutString("BUT_1");
									break;
								case 2:
									ServoWait(10);
									Uart_PutString("BUT_2");
									break;
								case 3:
									ServoSpeed(8);
									ServoGoTo(12);
									ServoWait(50);
									ServoGoTo(24);
									ServoWait(50);
									ServoGoTo(36);
									ServoWait(100);
									ServoGoTo(0);
									Uart_PutString("BUT_3");
									break;
								default:
									break;
							}
						}
						break;
					case STATE:
						sServoStatus = Servo_State();
						switch (sServoStatus.eState){
							case _CALLIBRATION : 
								CopyString("state callib ",cString); 
								break;
							case _IDDLE : 
								CopyString("state iddle ",cString); 
								break;
							case _IN_PROGRESS : 
								CopyString("state in_proggres ",cString); 
								break;
							case _WAITING : 
								CopyString("state waiting ",cString); 
								break;
							default:
								break;
						};
						AppendUIntToString(sServoStatus.uiPosition, cString);
						Uart_PutString(cString);
						break;
						
					default:
						break;
			}
		}
		else{
			Uart_PutString("Unknown command");
		}
	}
}

int main(){
	Led_Init();
	Keyboard_Init();
	ServoInit();
	UART_InitWithInt(9600);
	xCommon_Queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	xTaskCreate(Button_To_Led, NULL, 128, NULL, 1, NULL);
	xTaskCreate(UART_Recieve_Msg, NULL, 128, NULL, 1, NULL);
	xTaskCreate(Executor, NULL, 128, NULL, 1, NULL);
	vTaskStartScheduler();	
}
