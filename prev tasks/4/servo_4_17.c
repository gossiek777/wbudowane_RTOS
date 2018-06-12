#include <LPC21xx.H>
#include "servo.h"
#include "led.h"
#include "timer_interrupts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DETECTOR_OUT_bm (1<<10)
#define QUEUE_LENGTH 		25
#define QUEUE_ITEM_SIZE sizeof(struct ServoMsg)

QueueHandle_t xQueue;
extern TaskHandle_t xHandle;

enum ServoState {CALLIB, IDLE, IN_PROGRESS, WAIT, SPEED};
enum DetectorState {ACTIVE, INACTIVE};

struct Servo{
	enum ServoState eState;
	unsigned int uiCurrentPosition;
	unsigned int uiDesiredPosition;
	unsigned int uiDelay;
	unsigned int uiStepTicks;
};

struct ServoMsg{
	enum ServoState eState;
	unsigned int uiArgumentValue;
};


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

void Automat(void *pvParameters){
	struct Servo sServo;
	struct ServoMsg sServoCmd;
	sServo.uiStepTicks=1;
	sServo.eState=CALLIB;
	
	while(1){
		
		switch(sServo.eState){
			case IDLE:
				if(xQueueReceive(xQueue, &sServoCmd, portMAX_DELAY) == pdPASS){
					sServo.eState=sServoCmd.eState;
					switch(sServoCmd.eState){
						case(IN_PROGRESS):
							sServo.uiDesiredPosition=sServoCmd.uiArgumentValue;
						break;
						case(WAIT):
							sServo.uiDelay=sServoCmd.uiArgumentValue;
						break;
						case(SPEED):
							sServo.eState=IDLE;
							sServo.uiStepTicks=sServoCmd.uiArgumentValue;
						break;
						default:
						break;
					}
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
				vTaskDelay(sServo.uiStepTicks);
				break;
			case CALLIB: 
				if(eReadDetector()==ACTIVE){
					LedStepRight();
				}
				else{
					sServo.eState = IDLE;
					sServo.uiCurrentPosition = 0;
					sServo.uiDesiredPosition = 0;				
				}	
				break;
			case WAIT:
				vTaskDelay(sServo.uiDelay);
				sServo.eState = IDLE;
				break;
			default:
				break;
		}
	}
}

void ServoInit(){
	xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	LedInit();
	DetectorInit();
}

void ServoCallib(void){
	struct ServoMsg sServoCallib;
	sServoCallib.eState = CALLIB;
	sServoCallib.uiArgumentValue = 0;
	xQueueSendToBack(xQueue,&sServoCallib,2);
}

void ServoGoTo(unsigned int uiPosition){
	struct ServoMsg sServoGoTo;
	sServoGoTo.eState=IN_PROGRESS;
	sServoGoTo.uiArgumentValue = uiPosition;
	xQueueSendToBack(xQueue,&sServoGoTo,10);
}

void ServoWait(unsigned int uiTicks){
	struct ServoMsg sServoWait;
	sServoWait.eState=WAIT;
	sServoWait.uiArgumentValue = uiTicks;
	xQueueSendToBack(xQueue,&sServoWait,10);
}

void ServoSpeed(unsigned int uiTicksPerStep){
	struct ServoMsg sServoSpeed;
	sServoSpeed.eState=SPEED;
	sServoSpeed.uiArgumentValue = uiTicksPerStep;
	xQueueSendToBack(xQueue,&sServoSpeed,10);
}
