#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "uart.h"

void UartRx( void *pvParameters ){
	char acBuffer[UART_RX_BUFFER_SIZE];
	while(1){
		enum eRecieverStatus eStatus = eUartRx_GetStatus();
		while (eUartRx_GetStatus()==EMPTY){
			Led_Off();
		};
		Uart_GetStringCopy(acBuffer);
		Led_On(0);
	}
}
int main( void ){
	Led_Init();
	UART_InitWithInt(9600);
	xTaskCreate(UartRx, NULL , 100 , NULL, 1 , NULL);
	vTaskStartScheduler();
	while(1);
}
