#include "FreeRTOS.h"
#include "task.h"
#include "watch.h"
#include "led.h"


void WatchEvent_To_Led(void *pvParameters){
	struct WatchEvent sWatch;
	while(1){
		sWatch = sWatch_Read();
		if(sWatch.eTimeUnit == SECONDS)
			Led_Toggle(0);
		else
			Led_Toggle(1);
		vTaskDelay(50);
	}
}
int main( void ){

	Led_Init();
	Watch_Init();
	xTaskCreate(WatchEvent_To_Led, NULL, 128, NULL, 1, NULL);
	vTaskStartScheduler();	
}
