#include "watch.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define QUEUE_LENGTH 		5
#define QUEUE_ITEM_SIZE sizeof(struct WatchEvent)
#define SEND_TO_QUEUE_DELAY	1

#define SECONDS_IN_MINUTE		10
#define MINUTES_IN_HOUR			6


QueueHandle_t xWatchQueue;


struct WatchEvent sWatch_Read(void){
	struct WatchEvent sWatch;
	xQueueReceive(xWatchQueue, &sWatch, portMAX_DELAY);
	return sWatch;
}

void Watch_Thread (void *pvParameters){
	struct WatchEvent sWatch;
	unsigned char ucMinutes = 0;
	unsigned char ucSeconds = 0;
	while(1){
			ucSeconds++;
			if(ucSeconds == SECONDS_IN_MINUTE){
				ucSeconds = 0;
				ucMinutes++;
				if(ucMinutes == MINUTES_IN_HOUR){
					ucMinutes = 0;
				}
				sWatch.eTimeUnit=MINUTES;
				sWatch.TmeValue = ucMinutes;
				xQueueSendToBack(xWatchQueue, &sWatch, SEND_TO_QUEUE_DELAY);
			}
			sWatch.eTimeUnit=SECONDS;
			sWatch.TmeValue = ucSeconds;
			xQueueSendToBack(xWatchQueue, &sWatch, SEND_TO_QUEUE_DELAY);
			vTaskDelay(100);
	}
}

void Watch_Init(){
	struct WatchEvent sWatch;
	
	xWatchQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	sWatch.eTimeUnit = MINUTES;
	sWatch.TmeValue = 0;
	xQueueSendToBack(xWatchQueue, &sWatch, SEND_TO_QUEUE_DELAY);
	sWatch.eTimeUnit = SECONDS;
	sWatch.TmeValue = 0;
	xQueueSendToBack(xWatchQueue, &sWatch, SEND_TO_QUEUE_DELAY);
	
	xTaskCreate(Watch_Thread, NULL, 128, NULL, 1, NULL );
}

