#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "uart.h"

void UartRx( void *pvParameters ){
	//char acBuffer[UART_RX_BUFFER_SIZE];
	char c;
	while(1){
		c = cUart_GetChar()-'0';
		Led_Toggle(c%4);
		vTaskDelay(5);
	}
}
int main( void ){
	Led_Init();
	UART_InitWithInt(9600);
	xTaskCreate(UartRx, NULL , 100 , NULL, 1 , NULL);
	vTaskStartScheduler();
	while(1);
}
