#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart.h>
#include <string.h>
//#include <>

#define ON 1
#define OFF 0
#define TAILLE_COMMANDE_MAX 16
#define DELIMITERS "\n\t\r" 

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

ISR(TIMER1_COMPA_vect){ // interruption qui fait clignoter la LED
	PORTB ^= (1 << PORTB5);// L'utilisation de ^ XOR permet de faire changer le bit sans connaitre son état précedent
}

ISR(USART_RX_vect){
  ring_buffer_put(&buffer, UDR0);
}

int main(void){
	OCR1A 	= 15624;
	TCCR1B	|=  (1 << CS10) | (1 << CS12) | (1 << WGM12);
	TIMSK1 	|= (1 << OCIE1A);
  
  buffer.count = buffer.head = buffer.tail = 0;

	uart_init(9600);

	sei();
	set_DDRB(5);

  uint8_t index_commande = 0;
  uint8_t feed_state = 1;
  char commande[TAILLE_COMMANDE_MAX];

  wdt_enable(WDTO_8S);

	while(1){
    char new_char = ring_buffer_get(&buffer); // La fonction ring_buffer_get attend deja le remplissage du buffer avant d'envoyer un char (polling)
    // A chaque fois que le buffer se rempli il est vidé par la fonction. Il est donc impossible de mettre le meme caractere 2 fois.

    switch (new_char) {
      case 13:
      case 10:
        commande[index_commande] = '\0';
        
        if( strcmp(commande,"FEED_ON") == 0 ){
          feed_state = 1;
        }else if(strcmp(commande,"FEED_OFF") == 0 ){
          feed_state = 0;
          char init_string[] = "Restarting Program...";
          for(int i = 0; i < strlen(init_string) - 1 ; i++){
            uart_putchar(init_string[i]);
          }
        }else if(strcmp(strtok(commande,":"),"FREQ") == 0 ){
          char * token = strtok(NULL,DELIMITERS);
			    double FREQ = (double) strtol(token,(char **) NULL,10);
          cli(); // Necessaire pour éviter des erreurs pendant le changement
          OCR1A = (F_CPU/(1024*FREQ)) - 1;
          sei(); // Necessaire pour éviter des erreurs pendant le changement
        }

        index_commande = 0;
        break;
      default:
        if(index_commande < TAILLE_COMMANDE_MAX - 1)
          commande[index_commande++] = new_char;
        break;
    }

    if(feed_state){
      wdt_reset();
    }
	}
}
