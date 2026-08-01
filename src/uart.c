#include <stdlib.h>
#include <avr/io.h>

void uart_init(int baudrate){
	int UBRR0 = ((16000000)/16*baudrate) - 1;
	uint8_t UBRR0L = UBRR0; // Les 8 premiers bits sont mis directement dans la vari de type uint8_t
	uint8_t UBRR0H = (UBRR0 >> 8) & 0x0F; // Décalage de 8 bits pour fit 103 puis masque avec & 0x0F = 0000 1111 pour garder seulement les 4 premiers bits
	
       UCSR0B |= (1 << TXEN0) | (1 << RXEN0); // Enable Reception et Envoi
       UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Nombre de Bits à Envoyer/Recevoir

}

char uart_getchar(void){
	return UDR0;
}
void uart_putchar(char character){
	UDR0 = character;
};
