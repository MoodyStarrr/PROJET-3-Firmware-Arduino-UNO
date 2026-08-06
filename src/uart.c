#include <stdlib.h>
#include <avr/interrupt.h>
#include <uart.h>
#include <avr/io.h>

void uart_init(unsigned long int baudrate){
	/*
	int UBRR0mine = ( (F_CPU)/(16*baudrate) ) - 1;
	UBRR0L = UBRR0mine; // Les 8 premiers bits sont mis directement dans la vari de type uint8_t
	UBRR0H = (UBRR0mine >> 8) & 0x0F; // Décalage de 8 bits pour fit 103 puis masque avec & 0x0F = 0000 1111 pour garder seulement les 4 premiers bits
	*/

	UBRR0 = ( (F_CPU)/(16*baudrate) ) - 1;
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); // Enable Reception et Envoi
       	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Nombre de Bits à Envoyer/Recevoir
	
}

char uart_getchar(void){
	while( (UCSR0A & (1 << RXC0)) == 0){};
	return UDR0;
}
void uart_putchar(char character){
	while( (UCSR0A & (1 << UDRE0)) == 0){}; // Attente du passage de UDRE0 à 1 pour lire
	UDR0 = character;
};

void ring_buffer_put(volatile ring_buffer * buffer, char entree){
  if(buffer->count < TAILLE_RING_BUFFER){
    buffer->array[buffer->head] = entree;
    buffer->count += 1;
    buffer->head  = (buffer->head + 1)%(TAILLE_RING_BUFFER);
  }
}

char ring_buffer_get(volatile ring_buffer * buffer){

  while( !(buffer->count) ){};// Tant que le compteur est vide (= buffer vide) alors Attente

  cli(); // Clear Interruption pour éviter les data races dans le main

  char sortie = buffer->array[buffer->tail];
  buffer->tail = (buffer->tail + 1)%(TAILLE_RING_BUFFER);
  buffer->count -= 1;

  sei(); // Set Interruption pour relancer les ISR
  return sortie;
}

