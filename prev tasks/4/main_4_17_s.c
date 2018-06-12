#include "FreeRTOS.h"
#include "task.h"
#include "servo.h"
#include "string.h"
#include "keyboard.h"
#include "semphr.h"
#include "led.h"

#define QUEUE_LENGTH 5
#define QUEUE_ITEM_SIZE 25

TaskHandle_t xHandle;


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
				ServoSpeed(8);
				ServoGoTo(12);
				ServoWait(500);
				ServoSpeed(4);
				ServoGoTo(24);
				ServoWait(500);
				ServoSpeed(2);
				ServoGoTo(36);
				ServoWait(500);
				ServoSpeed(1);
				ServoGoTo(0);
				break;
			case BUTTON_3: 
				ServoGoTo(12);
				ServoWait(500);
				ServoGoTo(0);
				ServoGoTo(24);
				ServoWait(800);
				ServoGoTo(0);
				ServoGoTo(36);
				ServoWait(1000);
				ServoGoTo(0);
				break;
			default:
				break;
		}
		vTaskDelay(100);
	}
}



int main( void ){
	struct SetServo sServoParams;
	sServoParams.uiFreq=200;
	
	KeyboardInit();
	ServoInit();
	xTaskCreate(Keyboard, NULL, 128, NULL, 1, NULL );
	xTaskCreate(Automat, NULL, 512, &sServoParams, 5, &xHandle);
	vTaskStartScheduler();
	while(1);
}
