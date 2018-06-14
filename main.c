#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "uart.h"
#include "string.h"

void UartRx( void *pvParameters ){
	char acBuffer[UART_RX_BUFFER_SIZE];
	acBuffer[0]=NULL;
	AppendString("0123456789\n", acBuffer);
	while(1){
		/*
		vTaskDelay(500);
		Uart_PutString(acBuffer);
		Uart_PutString(acBuffer);
		Uart_PutString(acBuffer);
		*/
		
		vTaskDelay(500);
		Uart_PutString(acBuffer);
		vTaskDelay(10);
		Uart_PutString(acBuffer);
		vTaskDelay(10);
		Uart_PutString(acBuffer);
		
	}
}
int main( void ){
	Led_Init();
	UART_InitWithInt(9600);
	xTaskCreate(UartRx, NULL , 100 , NULL, 1 , NULL);
	vTaskStartScheduler();
	while(1);
}
