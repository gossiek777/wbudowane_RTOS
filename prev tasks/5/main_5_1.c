#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "timer_interrupts.h"

void LedBlink(void){
	LedToggle(0);
} 

int main(void){
	LedInit();
	Timer1Interrupts_Init(500000, &LedBlink);
	vTaskStartScheduler();
	while(1);
}
