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

enum ServoStateCmd {CAL, GO_TO, WAIT, SPEED};
enum ServoState {CALLIB, IDLE, IN_PROGRESS};
enum DetectorState {ACTIVE, INACTIVE};

struct Servo{
	enum ServoState eState;
	unsigned int uiCurrentPosition;
	unsigned int uiDesiredPosition;
	unsigned int uiDelay;
};

struct ServoMsg{
	enum ServoStateCmd eStateCmd;
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
	struct ServoMsg sServoMsg;
	sServo.uiDelay=1;
	sServo.eState=CALLIB;
	
	while(1){
		
		switch(sServo.eState){
			case IDLE:
				if(xQueueReceive(xQueue, &sServoMsg, portMAX_DELAY) == pdPASS){
					switch(sServoMsg.eStateCmd){
						case(GO_TO):
							sServo.uiDesiredPosition=sServoMsg.uiArgumentValue;
							sServo.eState=IN_PROGRESS;
						break;
						case(WAIT):
							sServo.uiDelay=sServoMsg.uiArgumentValue;
						break;
						case(SPEED):
							sServo.uiDelay=sServoMsg.uiArgumentValue;
						break;
						default:
						break;
					}
				}
				break;
			case IN_PROGRESS: 
				if(sServo.uiCurrentPosition > sServo.uiDesiredPosition){
					Led_StepLeft();
					sServo.eState = IN_PROGRESS;
					sServo.uiCurrentPosition--;
				}
				else if(sServo.uiCurrentPosition < sServo.uiDesiredPosition){
					sServo.eState = IN_PROGRESS;
					Led_StepRight();
					sServo.uiCurrentPosition++;
				}
				else{
					sServo.eState = IDLE;
				}
				break;
			case CALLIB: 
				if(eReadDetector()==ACTIVE){
					Led_StepRight();
				}
				else{
					sServo.eState = IDLE;
					sServo.uiCurrentPosition = 0;
					sServo.uiDesiredPosition = 0;				
				}	
				break;
			default:
				break;
		}
		vTaskDelay(sServo.uiDelay);
	}
}

void ServoInit(){
	xTaskCreate(Automat, NULL, 512, NULL, 3, NULL);
	xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	Led_Init();
	DetectorInit();
}

void ServoCallib(void){
	struct ServoMsg sServoCallib;
	sServoCallib.eStateCmd = CAL;
	sServoCallib.uiArgumentValue = 0;
	xQueueSendToBack(xQueue,&sServoCallib,2);
}

void ServoGoTo(unsigned int uiPosition){
	struct ServoMsg sServoGoTo;
	sServoGoTo.eStateCmd=GO_TO;
	sServoGoTo.uiArgumentValue = uiPosition;
	xQueueSendToBack(xQueue,&sServoGoTo,10);
}

void ServoWait(unsigned int uiTicks){
	struct ServoMsg sServoWait;
	sServoWait.eStateCmd=WAIT;
	sServoWait.uiArgumentValue = uiTicks;
	xQueueSendToBack(xQueue,&sServoWait,10);
}

void ServoSpeed(unsigned int uiTicksPerStep){
	struct ServoMsg sServoSpeed;
	sServoSpeed.eStateCmd=SPEED;
	sServoSpeed.uiArgumentValue = uiTicksPerStep;
	xQueueSendToBack(xQueue,&sServoSpeed,10);
}
