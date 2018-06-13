#include <LPC210X.H>
#include "uart.h"
#include "string.h"

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


char cOdebranyZnak;

struct TransmiterBuffer{
	char cData[TRANSMITER_SIZE];
	enum eTransmiterStatus eStatus;
	unsigned char fLastCharacter;
	unsigned char ucCharCtr;
};

struct TransmiterBuffer sTransmiterBuffer;

struct RecieverBuffer{
	char cData[RECIEVER_SIZE];
	unsigned char ucCharCtr;
	enum eRecieverStatus eStatus;
};

struct RecieverBuffer sBuffer;


///////////////////////////////////////////
__irq void UART0_Interrupt (void) {
   // jesli przerwanie z odbiornika (Rx)
   
   unsigned int uiCopyOfU0IIR=U0IIR; // odczyt U0IIR powoduje jego kasowanie wiec lepiej pracowac na kopii

   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING) // odebrano znak
   {
      cOdebranyZnak = U0RBR;
			Reciever_PutCharacterToBuffer(cOdebranyZnak);
   } 
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING)              // wyslano znak - nadajnik pusty 
   {
      if(sTransmiterBuffer.eStatus == BUSY){
				U0THR = Transmiter_GetCharacterFromBuffer();
			}
   }

   VICVectAddr = 0; // Acknowledge Interrupt
}

////////////////////////////////////////////Baudrate standard

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
}


//////////////////////////////////////////// Baudrate = 300
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

void Reciever_PutCharacterToBuffer(char cCharacter){
	if (sBuffer.ucCharCtr < RECIEVER_SIZE){	
		if (cCharacter != TERMINATOR){
			sBuffer.cData[sBuffer.ucCharCtr] = cCharacter;
			sBuffer.ucCharCtr++;
			sBuffer.eStatus = EMPTY;
		}
		else{
			sBuffer.cData[sBuffer.ucCharCtr] = NULL;
			sBuffer.eStatus = READY;
			sBuffer.ucCharCtr = 0;
		}
	}
	else{
		sBuffer.eStatus = OVERFLOW;
		sBuffer.ucCharCtr = 0;
	}	
}

enum eRecieverStatus eReciever_GetStatus(void) {
	return sBuffer.eStatus;
}

void Reciever_GetStringCopy(char * ucDestination) {
	CopyString(sBuffer.cData, ucDestination);
	sBuffer.eStatus=EMPTY;
}

char Transmiter_GetCharacterFromBuffer(void){
	
	char cCurrentSign;
	
	cCurrentSign = sTransmiterBuffer.cData[sTransmiterBuffer.ucCharCtr];

	if(cCurrentSign == NULL){
		if(sTransmiterBuffer.fLastCharacter==0){
			sTransmiterBuffer.fLastCharacter=1;
			return(TERMINATOR);
		}
		else{
			sTransmiterBuffer.fLastCharacter=0;
			sTransmiterBuffer.eStatus=FREE;
			return(NULL);
		}
	}
	else{
		sTransmiterBuffer.ucCharCtr++;
		return(cCurrentSign);
	}	
}

void Transmiter_SendString(char cString[]){
	sTransmiterBuffer.eStatus = BUSY;
	sTransmiterBuffer.ucCharCtr = 0;
	CopyString(cString, sTransmiterBuffer.cData);
	U0THR = Transmiter_GetCharacterFromBuffer();
}

enum eTransmiterStatus Transmiter_GetStatus(void){
	return sTransmiterBuffer.eStatus;
}
