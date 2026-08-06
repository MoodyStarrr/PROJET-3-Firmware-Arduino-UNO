#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart.h>
#include <string.h>
//#include <>

#define ON 1
#define OFF 0

void set_DDRB(int bit){
	DDRB |= ( 1 << bit ); // DDRB = Data Direction Register pour le port B. Permet de configurer chaque pin du port B en entrée 0 ou sortie 1
}
/* Explications : Avec OU | on compare les bits de DDRB avec les bits de (1 << bit). 
 * L'opérateur OU | dit que si au moins un des 2 bits est à 1 alors le resultat est 1.
 * L'opérateur << (bit-shift) decale les bits vers la gauche (>> vers la droite). ( a << b ) permet donc 
 * de décaler un nombre a de b positions vers la gauche.
 * 
 * Exemple : (1 << 5) = 0 0 0 0 1 0 0 0
 *   0 0 0 0 0 0 0 0 | (1 << 5) 
 * = 0 0 0 0 0 0 0 0 | 0 0 0 0 1 0 0 0 = 0 0 0 0 1 0 0 0
 *
 * Le bit à la position 5 à donc bien été mis à 1 sans toucher le reste de l'octet.
 * */

void config_PORTB(int state, int bit){
	if( state  == 0 ){
		PORTB &= ~(1 << bit); // PORTB permet d'écrire une valeur logique (haut/bas) sur chaque broche du port B configurée en sortie
	}else if( state  == 1 ){
		PORTB |= (1 << bit);
	}
}
/* Explcations : Avec ET & on compare les bits de PORTB avec les bits de (1 << bit)
 * L'opérateur ET & dit que les 2 bits doivent etre à 1 pour sortir 1
 *
 * Avec NOT ~ on inverse chaque bit
 *
 * Exemple: 
 * 1 0 1 0 & ~(0 0 1 0) = 1 0 1 0 & 1 1 0 1 = 1 0 0 0 
 *
 * Le bit à la position 3 à bien été mis à 0 sans toucher le reste de l'octet
 *
 * */

volatile ring_buffer buffer;

ISR(TIMER1_COMPA_vect){
	PORTB ^= (1 << PORTB5);
}

ISR(USART_RX_vect){
  ring_buffer_put(&buffer, UDR0);
}

int main(void){
	OCR1A 	= 15624; // Valeur qui va etre comparé au compteur du matériel, qui augmente à chaque cycle (TCNT1) pour remettre le compteur (TCNT1) à 0, ie tout les OCR1A incréments de TCNT1 il revient à 0. Permet de créer une "période". Valeur obtenue en faisant OCR1A = ((Freq_CPU)/(prescaler * Freq_Voulue)) - 1 avec prescaler = 1024
	TCCR1B	|=  (1 << CS10) | (1 << CS12) | (1 << WGM12);// Configuration du prescaler (diviseur de fréquence pour l'horloge)
	TIMSK1 	|= (1 << OCIE1A);// Active ou désactive l'interruption périodique
  
  buffer.count = buffer.head = buffer.tail = 0;
	char str[] = "Hello World\n";
  char received[strlen(str)];

	uart_init(9600);

	sei();
	set_DDRB(5);

	for(int i = 0; i < strlen(str) - 1; i++){
		uart_putchar(str[i]);
	}
  /* TP3	
	char received[4];
	received[0] = uart_getchar();
	received[1] = uart_getchar();
	received[2] = uart_getchar();
	received[3] = uart_getchar();
  */
  for(int i = 0; i < strlen(str) - 1;i++){
    received[i] = ring_buffer_get(&buffer);
  }

	for(int i = 0; i < strlen(str) - 1; i++){
		uart_putchar(received[i]);
	}
	while(1){
	}
}
