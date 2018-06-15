#define UART_RX_BUFFER_SIZE 20
#define UART_TX_BUFFER_SIZE 30

enum eRecieverStatus {EMPTY, READY, OVERFLOW};
enum eTransmiterStatus {FREE, BUSY};

void UART_InitWithInt(unsigned int);

void Uart_GetString(char *);

void Uart_PutString(char *);
