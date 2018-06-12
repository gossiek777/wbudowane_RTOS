#include <LPC21xx.H>
#include "servo.h"
#include "led.h"
#include "timer_interrupts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DETECTOR_OUT_bm (1<<10)
#define QUEUE_LENGTH 		25
#define QUEUE_ITEM_SIZE sizeof(unsigned int)

QueueHandle_t xQueue;

enum ServoState {CALLIB, IDLE, IN_PROGRESS};
enum DetectorState {ACTIVE, INACTIVE};

struct Servo
{
enum ServoState eState;
unsigned int uiCurrentPosition;
unsigned int uiDesiredPosition;
};
struct Servo sServo;

void DetectorInit(void){

	IO0DIR=IO0DIR&(~DETECTOR_OUT_bm);
}

enum DetectorState eReadDetector (void){

	if((IO0PIN&DETECTOR_OUT_bm) == 0){
		return(INACTIVE);
	}
	else{
		return(ACTIVE);
	}
}

void ServoCallib(void){

	//sServo.eState = CALLIB;
	unsigned int uiPos = 0;
	xQueueSendToBack(xQueue,&uiPos,2);
}

void ServoGoTo(unsigned int uiPosition){

	//sServo.eState = IN_PROGRESS;
	//sServo.uiDesiredPosition = uiPosition;
	xQueueSendToBack(xQueue,&uiPosition,10);
}

void Automat(void *pvParameters){
	struct ServoFreq sServoFrequency = *((struct ServoFreq*)pvParameters);
	unsigned int uiPeriod = 1000/(sServoFrequency.uiFreq);
	unsigned int uiNewPosition = 0;
	sServo.eState=CALLIB;
	while(1){
		
		if(sServo.eState == IDLE){
			if( xQueueReceive( xQueue, &uiNewPosition, 10) == pdPASS){
				sServo.uiDesiredPosition = uiNewPosition;
				sServo.eState=IN_PROGRESS;
			}
		}
		
		switch(sServo.eState){
			case IDLE: 	
				if(sServo.uiCurrentPosition != sServo.uiDesiredPosition){
					sServo.eState = IN_PROGRESS;
				}
				else{
					sServo.eState = IDLE;
				}
				break;
			case IN_PROGRESS: 
				if(sServo.uiCurrentPosition > sServo.uiDesiredPosition){
					LedStepLeft();
					sServo.eState = IN_PROGRESS;
					sServo.uiCurrentPosition--;
				}
				else if(sServo.uiCurrentPosition < sServo.uiDesiredPosition){
					sServo.eState = IN_PROGRESS;
					LedStepRight();
					sServo.uiCurrentPosition++;
				}
				else{
					sServo.eState = IDLE;
				}
				break;
			case CALLIB: 
				if(eReadDetector()==INACTIVE){
					LedStepRight();
				}
				else{
					sServo.eState = IDLE;
					sServo.uiCurrentPosition = 0;
					sServo.uiDesiredPosition = 0;				
				}	
				break;
		}
		vTaskDelay(uiPeriod);
	}
}
void ServoInit(){
	xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	LedInit();
	DetectorInit();
}


