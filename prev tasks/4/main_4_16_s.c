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
	enum ButtonState eButton;
	while(1){
		eButton = eKeyboardRead();
		switch(eButton){
			case BUTTON_0: 
				ServoCallib();
				break;
			case BUTTON_1: 
				ServoGoTo(50);
				break;
			case BUTTON_2: 
				ServoGoTo(100);
				break;
			case BUTTON_3: 
				ServoGoTo(12);
				ServoGoTo(0);
				ServoGoTo(24);
				ServoGoTo(0);
				ServoGoTo(36);
				ServoGoTo(0);
				break;
			default:
				break;
		}
		vTaskDelay(100);
	}
}



int main( void ){
	struct ServoFreq sFreq;
	sFreq.uiFreq=20;
	
	KeyboardInit();
	ServoInit();
	xTaskCreate(Keyboard, NULL, 128, NULL, 1, NULL );
	xTaskCreate(Automat, NULL, 128, &sFreq, 1, NULL );
	vTaskStartScheduler();
	while(1);
}
