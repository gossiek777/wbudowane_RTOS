#include "FreeRTOS.h"
#include "task.h"
#include "servo.h"
#include "string.h"
#include "keyboard.h"
#include "semphr.h"
#include "led.h"

#define QUEUE_LENGTH 5
#define QUEUE_ITEM_SIZE 25


void Keyboard (void *pvParameters){
	enum ButtonState eButtonCurrent;
	enum ButtonState eButtonPreviuos = RELASED;
	while(1){
		eButtonCurrent = eKeyboardRead();
		if(eButtonCurrent != eButtonPreviuos){
			switch(eButtonCurrent){
				case BUTTON_0: 
					ServoCallib();
					break;
				case BUTTON_1: 
					ServoGoTo(50);
					break;
				case BUTTON_2:			
					ServoSpeed(8);
					ServoGoTo(12);
					ServoWait(50);
					ServoSpeed(4);
					ServoGoTo(24);
					ServoWait(50);
					ServoSpeed(2);
					ServoGoTo(36);
					ServoWait(50);
					ServoSpeed(1);
					ServoGoTo(0);
					break;
				case BUTTON_3: 
					ServoGoTo(12);
					ServoWait(100);
					ServoGoTo(0);
					ServoGoTo(24);
					ServoWait(200);
					ServoGoTo(0);
					ServoGoTo(36);
					ServoWait(300);
					ServoGoTo(0);
					break;
				default:
					break;
			}
		}
		eButtonPreviuos=eButtonCurrent;
		vTaskDelay(10);
	}
}


int main( void ){
	
	KeyboardInit();
	ServoInit();
	xTaskCreate(Keyboard, NULL, 128, NULL, 1, NULL );
	//xTaskCreate(Automat, NULL, 512, NULL, 3, NULL);
	vTaskStartScheduler();
	while(1);
}
