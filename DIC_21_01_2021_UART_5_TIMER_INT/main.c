/*
 * DIC_21_01_2021_UART_5_TIMER_INT.c
 *
 * Created: 21.01.2021 23:21:26
 * Author : plack
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile int endyet = 1;
unsigned char data = 0;

void startTimer1();
void stopTimer1();

void init_ADC();
void init_UART();
void init_TIMER1();
unsigned char USART_Receive(void);

ISR(TIMER1_COMPA_vect);
ISR(USART0_RX_vect);

int main(void){
	
	init_ADC();
	init_UART();
	init_TIMER1();
	
	while (1) while(endyet == 0) USART_Transmit(ADCH);

}

void init_ADC(){
	ADMUX |= (1<<REFS0) | (1<<ADLAR);
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADEN) | (1<<ADATE) | (1<<ADSC);
}

void init_UART(){
	UBRR0 = 103;												//baud rate 9600

	UCSR0B |= (1<<RXCIE0);
	
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);							//enable recieve and transmit
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);							//8 Databits 1 Stopbit
	
	
}

void USART_Transmit(unsigned char data){
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void startTimer1(){
	TCCR1B |= (1<<CS10);
	TCCR1B &= ~(1<<CS11);	// Presc = 1024
	TCCR1B |= (1<<CS12);
}

void stopTimer1(){
	TCCR1B &= ~(1<<CS10);
	TCCR1B &= ~(1<<CS11);	// Timer Deactivated
	TCCR1B &= ~(1<<CS12);
	
	TCNT1 = 0x0000;
}

void init_TIMER1(){
	TCCR1B |= (1<<WGM12);	// CTC-Mode (OCR1A)
	
	stopTimer1();
	
	OCR1A = 0x7A12;			// Compare Value
	TIMSK1 |= (1<<OCIE1A);	// Enable Compare Interrupt
	sei();
}

ISR(USART0_RX_vect){
	data = UDR0;			// Get Data out of the Shift-Register
	if(data == 'g'){		
		endyet = 0;			// if 'g' -> start sending & timer
		startTimer1();
	} else if(data == 's'){
		endyet = 1;			// else if 's' -> stop sending & timer
		stopTimer1();
	}
}

ISR(TIMER1_COMPA_vect){
	endyet = 1;
	stopTimer1();
}


