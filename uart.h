#define RECIEVER_SIZE				20
#define UART_RX_BUFFER_SIZE 200
#define TRANSMITER_SIZE			20

enum eRecieverStatus {EMPTY, READY, OVERFLOW};
enum eTransmiterStatus {FREE, BUSY};

void UART_InitWithInt(unsigned int);

//void Reciever_PutCharacterToBuffer(char);
//enum eRecieverStatus eReciever_GetStatus(void);
//void Reciever_GetStringCopy(char * ucDestination);
char cUart_GetChar(void);

char Transmiter_GetCharacterFromBuffer(void);
void Transmiter_SendString(char cString[]);
enum eTransmiterStatus Transmiter_GetStatus(void);
