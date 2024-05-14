/*;UNIVERSIDAD DEL VALLE DE GUATEMALA
;IE2023 PROGRAMACIÓN DE MICROCONTROLADORES
;PROYECTO 2
;main.c
;AUTOR: GABRIEL ANTONIO ALVAREZ DE LEÓN
;PROYECTO 2, Rostro animatrónico
;HARDWARE: ATMEGA328P
;CREADO: 22/04/2024
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void initUART9600(void);
void initADC(void);
void initT1(void);



uint8_t mode = 0;

volatile uint16_t Ser0_LR_LeftEye = 1999;
volatile uint16_t Ser1_UD_LeftEye = 0;
volatile uint16_t Ser2_LR_RightEye = 0;
volatile uint16_t Ser3_UD_RightEye = 0;
volatile uint16_t Ser4_L_Eyebrow = 0;
volatile uint16_t Ser5_R_Eyebrow = 0;


int main(void)
{
    //SETUP
	cli();
	DDRB |= 0b1111111; //PORTB as output
	
	initUART9600();
	initADC();
	initT1();
	sei();
	
    while (1) //LOOP
    {
		//switch(ADMUX&0x07){
			//case 0:
				//OCR1A =  1999;
				//break;
			//case 1:
				//OCR1B = 3500;
				//break;
			//case 2:
				//break;
			//case 3:
				//break;
			//case 4:
				//break;
			//case 5:
				//break;
				//
				//
		//}
			_delay_us(1000);
			OCR1A = Ser0_LR_LeftEye;
			OCR1B = Ser1_UD_LeftEye;
			ADCSRA |= (1<<ADSC);
											
    }
}

void initUART9600(void){
	//Rx as input, Tx as output
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	
	UCSR0A = 0;	UCSR0B = 0; UCSR0C = 0; //Initial values
	UCSR0A |= (0<<U2X0); //UCSR0A Config
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0)|(0<<UCSZ02); //UCSR0B Config.
	UCSR0C |= (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00); //UCSR0C Config.
	UBRR0 = 103;
	
}

void initADC(void){
	ADMUX = 0;
	ADMUX |= (1<<REFS0)|(0<<REFS1)|(1<<ADLAR);
	ADCSRA = 0;
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADIE);
	
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

void initT1(void){
	//setup OC1A, OC1B as non inverting (clear when compare)
	TCCR1A |= (1<<COM1A1)|(0<<COM1A0);
	TCCR1A |= (1<<COM1B1)|(0<<COM1B0);
	
	//Set fast PWM mode for Timer1. TOP = ICR1
	TCCR1A |= (1<<WGM11)|(0<<WGM10);
	TCCR1B |= (1<<WGM13)|(1<<WGM12);
	TCCR1B |= (0<<CS12)|(1<<CS11)|(0<<CS10);//Prescaler 1/8 Tmax = 32768 µs
	ICR1 = 39999;
	
}



ISR(ADC_vect){ //ADC completion ISR
	
	switch(ADMUX&0x07){
		case 0: Ser0_LR_LeftEye = 1000 + ADCH *(15.6862745);	break;
		case 1: Ser1_UD_LeftEye = 1000 + ADCH *(15.6862745);	break;
		case 2:	Ser2_LR_RightEye= 1000 + ADCH *(15.6862745);	break;
		case 3: Ser3_UD_RightEye= 1000 + ADCH *(15.6862745);	break;
		case 4: Ser4_L_Eyebrow  = 1000 + ADCH *(15.6862745);	break;
		case 5: Ser5_R_Eyebrow  = 1000 + ADCH *(15.6862745);	break;
	}

	if((ADMUX & 0b00000111)==5){ADMUX &= 0b11111000;}
	else{ADMUX++;}

	ADCSRA |= (1<<ADIF); //Clear interrupt flag.

}

