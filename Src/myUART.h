#include "MKL25Z4.h"
#define BAUD_RATE 9600
#define UART_TX_PTE22 22
#define UART_RX_PTE23 23
#define UART2_INT_PRIO 128

void initUART2(uint32_t baud_rate);