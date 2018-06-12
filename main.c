#include "FreeRTOS.h"
#include "task.h"
#include "keyboard.h"
#include "led.h"

void Delay(unsigned int uiMiliSec) {
	unsigned int uiLoopCtr, uiDelayLoopCount;
	uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr = 0; uiLoopCtr < uiDelayLoopCount; uiLoopCtr++) {}
}

void Button_To_Led(void *pvParameters){
	enum ButtonState eButton;
	
	while(1){
		eButton = eKeyboard_Read();
		//eButton = eReadButtons();
		switch(eButton){
			case(BUTTON_0):
				Led_On(0);
				break;
			case(BUTTON_1):
				Led_On(1);
				break;
			case(BUTTON_2):
				Led_On(2);
				break;
			case(BUTTON_3):
				Led_On(3);
				break;
			default:
				break;
		}
		vTaskDelay(100);
	}
}

int main( void ){

	Keyboard_Init();
	Led_Init();
	xTaskCreate(Button_To_Led, NULL, 128, NULL, 1, NULL);
	vTaskStartScheduler();	
}
