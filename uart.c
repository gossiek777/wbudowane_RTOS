#include <LPC210X.H>
#include "uart.h"
#include "string.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/************ UART ************/
//pin control register
#define mPINSEL0_UART_RX_MODE 							0x00000004
#define mPINSEL0_UART_TX_MODE 							0x00000001

// U0LCR Line Control Register
#define mDIVISOR_LATCH_ACCES_BIT                   0x00000080
#define m8BIT_UART_WORD_LENGTH                     0x00000003

// UxIER Interrupt Enable Register
#define mRX_DATA_AVALIABLE_INTERRUPT_ENABLE        0x00000001
#define mTHRE_INTERRUPT_ENABLE                     0x00000002

// UxIIR Pending Interrupt Identification Register
#define mINTERRUPT_PENDING_IDETIFICATION_BITFIELD  0x0000000F
#define mTHRE_INTERRUPT_PENDING                    0x00000002
#define mRX_DATA_AVALIABLE_INTERRUPT_PENDING       0x00000004

/************ Interrupts **********/
// VIC (Vector Interrupt Controller) channels
#define VIC_UART0_CHANNEL_NR  6
#define VIC_UART1_CHANNEL_NR  7

// VICVectCntlx Vector Control Registers
#define mIRQ_SLOT_ENABLE                           0x00000020

#define SEND_TO_QUEUE_DELAY 20

char cOdebranyZnak;

xQueueHandle xUART_RX_Queue;
xQueueHandle xUART_TX_Queue;
xSemaphoreHandle xUART_TX_Sem;

///////////////////////////////////////////
__irq void UART0_Interrupt (void) {
   // jesli przerwanie z odbiornika (Rx)
   
   unsigned int uiCopyOfU0IIR=U0IIR; // odczyt U0IIR powoduje jego kasowanie wiec lepiej pracowac na kopii

   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING) // odebrano znak
   {
      cOdebranyZnak = U0RBR;
		 xQueueSendToBackFromISR(xUART_RX_Queue, &cOdebranyZnak, NULL);
   } 
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING)              // wyslano znak - nadajnik pusty 
   {
		 char cCharToSend;
		 xQueueReceiveFromISR(xUART_TX_Queue, &cCharToSend, NULL);
			if(cCharToSend !=NULL)
				U0THR = cCharToSend;
			else if (cCharToSend ==NULL){
				U0THR = '\r';
				xSemaphoreGiveFromISR(xUART_TX_Sem, NULL);
			}
		}

   VICVectAddr = 0; // Acknowledge Interrupt
}

//////////////////////////////////////////// Standard Version

void UART_InitWithInt(unsigned int uiBaudRate){

   // UART0
   PINSEL0 = PINSEL0 | mPINSEL0_UART_RX_MODE | mPINSEL0_UART_TX_MODE;                      // ustawic pina na odbiornik uart0
   U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT; // dlugosc slowa, DLAB = 1
   U0DLL   = (((15000000)/16)/uiBaudRate);                      // predkosc transmisji
   U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       // DLAB = 0
   U0IER  |= mRX_DATA_AVALIABLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE;               // Wlaczamy przerwanie po odebraniu znaku i wpisaniu go do bufora U0RBR

   // INT
   VICVectAddr2  = (unsigned long) UART0_Interrupt;             // set interrupt service routine address
   VICVectCntl2  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
   VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel

	xUART_RX_Queue = xQueueCreate(UART_RX_BUFFER_SIZE,sizeof(char));
	xUART_TX_Queue = xQueueCreate(UART_TX_BUFFER_SIZE,sizeof(char));
	xUART_TX_Sem = xSemaphoreCreateBinary();
	xSemaphoreGive(xUART_TX_Sem);
}

////////////////////////Version for low BaudRate (300)
/*
void UART_InitWithInt(unsigned int uiBaudRate){
	
	unsigned long ulDivisor, ulWantedClock;
	ulWantedClock=uiBaudRate*16;
	ulDivisor=15000000/ulWantedClock;
	// UART
	PINSEL0 = PINSEL0 | 0x55;                                     // ustawic piny uar0 odbiornik nadajnik
	U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT; // d³ugosc s³owa, DLAB = 1
	U0DLL = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	ulDivisor >>= 8;
	U0DLM = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       // DLAB = 0
	U0IER  |= mRX_DATA_AVALIABLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE ;               

	// INT
	VICVectAddr1  = (unsigned long) UART0_Interrupt; 
	//VICVectAddr1  = (unsigned long) ISR;             						// set interrupt service routine address
	VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
	VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel

}
*/




//**************************** RECIEVER FUNCTIONS ****************************///

char cUart_GetChar(){
	char cRecievedChar;
	xQueueReceive(xUART_RX_Queue, &cRecievedChar, portMAX_DELAY);
	return cRecievedChar;
}
void Uart_GetString(char *acDestination){
	char cRecievedChar;
	unsigned char ucCounter = 0;
	xQueueReceive(xUART_RX_Queue, &cRecievedChar, portMAX_DELAY);
	while (cRecievedChar != TERMINATOR){
		acDestination[ucCounter] = cRecievedChar;
		ucCounter++;
		xQueueReceive(xUART_RX_Queue, &cRecievedChar, portMAX_DELAY);
	}
	acDestination[ucCounter] = NULL;
}


//**************************** TRANSMITER FUNCTIONS ****************************///

void Uart_PutString(char *acStringToSend){
	char cFristChar = acStringToSend[0];
	unsigned char ucCounter=1;
	while(acStringToSend[ucCounter] != NULL){
		xQueueSendToBack(xUART_TX_Queue, &acStringToSend[ucCounter], SEND_TO_QUEUE_DELAY);
		ucCounter++;
	}
	xQueueSendToBack(xUART_TX_Queue, &acStringToSend[ucCounter], SEND_TO_QUEUE_DELAY);
	xSemaphoreTake(xUART_TX_Sem, portMAX_DELAY);
	U0THR = cFristChar;
}
