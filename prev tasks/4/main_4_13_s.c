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
	//enum ButtonState eButton = *((enum ButtonState*)pvParameters);
	enum ButtonState eButton;
	while(1){
		eButton = eKeyboardRead();
		switch(eButton){
			case BUTTON_0: 
				ServoCallib();
				LedOn(0);
				break;
			case BUTTON_1: 
				ServoGoTo(50);
				LedOn(1);
				break;
			case BUTTON_2: 
				ServoGoTo(100);
				LedOn(2);
				break;
			case BUTTON_3: 
				ServoGoTo(150);
				LedOn(3);
			break;
			default:
				break;
		}
		vTaskDelay(100);
	}
}


int main( void ){
	
	KeyboardInit();
	//LedInit();
	ServoInit(100);
	xTaskCreate(Keyboard, NULL, 128, NULL, 1, NULL );
	vTaskStartScheduler();
	while(1);
}
