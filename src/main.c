#include <avr/io.h>
#include <util/delay.h>
//#include <>

#define ON 1
#define OFF 0

void set_DDRB(int bit){
	DDRB |= ( 1 << bit );
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
		PORTB &= ~(1 << bit);
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

int main(void){
	set_DDRB(5);
	while(1){
		config_PORTB(ON,5);
		_delay_ms(1000);
		config_PORTB(OFF,5);
		_delay_ms(1000);
	}
}
