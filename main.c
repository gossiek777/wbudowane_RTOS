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
	char cButtonMsg[20];
	char cButtonNum[10];
	while(1){
		eButton = eKeyboard_Read();
		if(eButton != ePreviousButton){
			switch(eButton){
				case(BUTTON_0):
					UIntToHexStr(0, cButtonNum);
					break;
				case(BUTTON_1):
					UIntToHexStr(1, cButtonNum);
					break;
				case(BUTTON_2):
					UIntToHexStr(2, cButtonNum);
					break;
				case(BUTTON_3):
					UIntToHexStr(3, cButtonNum);
					break;
				default:
					break;
			}
			CopyString("button ", cButtonMsg);
			AppendString(cButtonNum, cButtonMsg);
			xQueueSendToBack(xCommon_Queue, cButtonMsg, 1);
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
						CopyString("callib_ok", cString);
						break;
					
					case GOTO:
						if (asToken[1].eType == NUMBER){
							ServoGoTo(asToken[1].uValue.uiNumber);
							CopyString("goto_ok ", cString);
						}
						break;
					
					case ID:
						CopyString("id: 0x0029", cString);
						break;
					
					case WAIT:
						if (asToken[1].eType == NUMBER){
							ServoWait(asToken[1].uValue.uiNumber);
							CopyString("wait_ok ", cString);					
						}
						break;
					
					case SPEED:
						if (asToken[1].eType == NUMBER){
							ServoSpeed(asToken[1].uValue.uiNumber);
							CopyString("speed_ok ", cString);
						}
						break;
					
					case BUTTON:
						if (asToken[1].eType == NUMBER){
							CopyString("button_ok ", cString);
							switch(asToken[1].uValue.uiNumber){
								case 0:
									ServoCallib();
									break;
								case 1:
									ServoGoTo(12);
									break;
								case 2:
									ServoGoTo(24);
									ServoWait(200);
									ServoGoTo(0);
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
						break;
						
					default:
						break;
			}
			Uart_PutString(cString);
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
