#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC0 16000000
#define BAUD0 9600
#define MYUBR0 FOSC0/16/BAUD0-1

#define OLEN 128 // size of serial transmission buffer
extern volatile unsigned char ostart0; // transmission buffer start index
extern volatile unsigned char oend0; // transmission buffer end index
extern volatile char outbuf0[OLEN]; // storage for transmission buffer

#define ILEN 128 // size of serial receiving buffer
extern volatile unsigned char istart0; // receiving buffer start index
extern volatile unsigned char iend0; // receiving buffer end index
extern volatile char inbuf0[ILEN]; // storage for receiving buffer

extern volatile char sendfull0; // flag: marks transmit buffer full
extern volatile char sendactive0; // flag: marks transmitter active

extern volatile char TX0;
extern volatile char RX0;

void putbuf0 (char c) {
	
	if (!sendactive0)  // if transmitter not active:
	{
		sendactive0 = 1; // transfer first character direct
		UDR0 = c; // to SBUF to start transmission
	}
	else 
	{
		cli(); // disable serial interrupts during buffer update
		outbuf0[oend0++] = c; // put char to transmission buffer
		sei(); // enable serial interrupts again
	}
}

void print0 (char *data){
	int i;
	TX0=0;
	for(i=0; data[i];i++)
	{
		putbuf0(data[i]);
	}
	while(!TX0);
	TX0=0;
}

ISR(USART0_TX_vect)
{	
	if (ostart0 != oend0)  // if characters in buffer
	{	
		UDR0 = outbuf0[ostart0++]; // transmit character
		TX0=0;
	}
	else  // if all characters transmitted
	{	
		oend0=0;
		ostart0=0;
		sendactive0 = 0; // clear 'sendactive'
		TX0=1;
	}
}

ISR(USART0_RX_vect)
{
	char c;
	c = UDR0; // read character
		
 	if (iend0 == ILEN || c == 0x0D)		//Carriage Return 0x0d ou não poder escrever mais
	{
		if (c == 0x0D)
		{
			inbuf0[iend0] = '\0';
			iend0=0;
			RX0=1;
		}
	}

	else 
	{
		inbuf0[iend0++] = c; // put character into buffer
		RX0=0;
	}	
}

void serie_setup0(){
	UCSR0B |= (1<<RXCIE0) | (1<<TXCIE0) | (1<<RXEN0) | (1<<TXEN0); //Ativar porta Série (Interrupts + REceber e transmitir)
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00); //Modo Assíncrono
	UBRR0H = ((MYUBR0)>>8);
	UBRR0L = MYUBR0;
}
