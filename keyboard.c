#include <LPC21xx.H>
#include "keyboard.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define BUTTON0_bm (1<<4)
#define BUTTON1_bm (1<<6)
#define BUTTON2_bm (1<<5)
#define BUTTON3_bm (1<<7)

#define QUEUE_LENGTH 		1
#define QUEUE_ITEM_SIZE sizeof(enum ButtonState)

#define SEND_TO_QUEUE_DELAY	10

QueueHandle_t xButtonQueue;

enum ButtonState eReadButtons(void){
	if (0 == (IO0PIN & BUTTON0_bm )){
		return BUTTON_0;
	}
	else if (0 == (IO0PIN & BUTTON1_bm )){
		return BUTTON_1;
	}
	else if (0 == (IO0PIN & BUTTON2_bm )){
		return BUTTON_2;
	}
	else if (0 == (IO0PIN & BUTTON3_bm )){
		return BUTTON_3;
	}
	else{
		return RELASED;
	}
}


void Keyboard_Thread(void *pvParameters){
	enum ButtonState ePressedButton;
	while(1){
		ePressedButton=eReadButtons();
		if(ePressedButton!= RELASED){
			xQueueSendToBack(xButtonQueue, &ePressedButton, SEND_TO_QUEUE_DELAY);
		}
		vTaskDelay(5);
	}
}

enum ButtonState eKeyboard_Read(){
	enum ButtonState eReceivedButton;
	xQueueReceive(xButtonQueue, &eReceivedButton, portMAX_DELAY);
	return eReceivedButton;
}
	
void Keyboard_Init (void){
	IO0DIR = IO0DIR & (~(BUTTON0_bm | BUTTON1_bm | BUTTON2_bm | BUTTON3_bm));
	
	xButtonQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	xTaskCreate(Keyboard_Thread, NULL, 128, NULL, 1, NULL );
}
