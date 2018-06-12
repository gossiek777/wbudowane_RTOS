#include <LPC21xx.H>
#include "servo.h"
#include "led.h"
#include "timer_interrupts.h"
#include "FreeRTOS.h"
#include "task.h"

#define DETECTOR_OUT_bm (1<<10)

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

	sServo.eState = CALLIB;
}

void ServoGoTo(unsigned int uiPosition){

	sServo.eState = IN_PROGRESS;
	sServo.uiDesiredPosition = uiPosition;
}

void Automat(void *pvParameters){
	struct ServoFreq sServoFrequency = *((struct ServoFreq*)pvParameters);
	unsigned int uiPeriod = 1000/(sServoFrequency.uiFreq);
	while(1){
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
				LedOn(2);
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
	LedInit();
	DetectorInit();
	ServoCallib();
}


