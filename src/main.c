#include <avr/io.h>
#include <avr/wdt.h>
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

void config_PORTB(int state, int bit){
	if( state  == 0 ){
		PORTB &= ~(1 << bit); // PORTB permet d'écrire une valeur logique (haut/bas) sur chaque broche du port B configurée en sortie
	}else if( state  == 1 ){
		PORTB |= (1 << bit);
	}
}

volatile ring_buffer buffer;

volatile uint8_t count_time = 0;

ISR(TIMER1_COMPA_vect){
	PORTB ^= (1 << PORTB5);
  count_time += 1;
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

  for(int i = 0; i < strlen(str) - 1;i++){
    received[i] = ring_buffer_get(&buffer);
  }

	for(int i = 0; i < strlen(str) - 1; i++){
		uart_putchar(received[i]);
	}

  wdt_enable(WDTO_2S);

	while(1){
    if(count_time <= 10) //10 itérations de count_time correspondent à 5s
      wdt_reset();
	}
}
