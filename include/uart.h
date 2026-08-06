#ifndef UART_H 
#define UART_H

#include <stdlib.h>
#include <stdint.h>

#define TAILLE_RING_BUFFER 64

typedef struct{
	uint8_t count,head,tail;
  uint8_t array[TAILLE_RING_BUFFER];
}ring_buffer;

// Uart Functions
void uart_init(unsigned long int baudrate);
char uart_getchar(void);
void uart_putchar(char character);

// Ring Buffer Function
void ring_buffer_put(volatile ring_buffer * buffer,char entree);
char ring_buffer_get(volatile ring_buffer * buffer);
#endif
