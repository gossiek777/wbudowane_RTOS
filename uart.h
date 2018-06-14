#define RECIEVER_SIZE				20
#define UART_RX_BUFFER_SIZE 20
#define TRANSMITER_SIZE			20

enum eRecieverStatus {EMPTY, READY, OVERFLOW};
enum eTransmiterStatus {FREE, BUSY};

void UART_InitWithInt(unsigned int);

void Uart_GetString(char *);

char Transmiter_GetCharacterFromBuffer(void);
void Transmiter_SendString(char cString[]);
enum eTransmiterStatus Transmiter_GetStatus(void);
