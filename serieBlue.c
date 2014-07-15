#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC1 16000000
#define BAUD1 38400
#define MYUBR1 FOSC1/16/BAUD1-1

#define OLEN 128 // size of serial transmission buffer
extern volatile unsigned char ostart1; // transmission buffer start index
extern volatile unsigned char oend1; // transmission buffer end index
extern volatile char outbuf1[OLEN]; // storage for transmission buffer

#define ILEN 128 // size of serial receiving buffer
extern volatile unsigned char istart1; // receiving buffer start index
extern volatile unsigned char iend1; // receiving buffer end index
extern volatile char inbuf1[ILEN]; // storage for receiving buffer

extern volatile char sendfull1; // flag: marks transmit buffer full
extern volatile char sendactive1; // flag: marks transmitter active

extern volatile char TX1;
extern volatile char RX1;

void putbuf1 (char c) {
	
	if (!sendactive1)  // if transmitter not active:
	{
		sendactive1 = 1; // transfer first character direct
		UDR1 = c; // to SBUF to start transmission
	}
	else 
	{
		cli(); // disable serial interrupts during buffer update
		outbuf1[oend1++] = c; // put char to transmission buffer
		sei(); // enable serial interrupts again
	}
}

void print1 (char *data){
	int i;
	TX1=0;
	for(i=0; data[i];i++)
	{
		putbuf1(data[i]);
	}
	putbuf1('\n');
	putbuf1('\r');
	putbuf1('\n');
	while(!TX1);
	TX1=0;
}

ISR(USART1_TX_vect)
{	
	if (ostart1 != oend1)  // if characters in buffer
	{	
		UDR1 = outbuf1[ostart1++]; // transmit character
		TX1=0;
	}
	else  // if all characters transmitted
	{	
		oend1=0;
		ostart1=0;
		sendactive1 = 0; // clear 'sendactive'
		TX1=1;
	}
}

ISR(USART1_RX_vect)
{
	char c;
	c = UDR1; // read character
		
 	if (iend1 == ILEN || c == 0x0D)		//Carriage Return 0x0d ou não poder escrever mais
	{
		if (c == 0x0D)
		{
			inbuf1[iend1] = '\0';
			iend1=0;
			RX1=1;
		}
	}

	else 
	{
		inbuf1[iend1++] = c; // put character into buffer
		RX1=0;
	}	
}

void serie_setup1(){
	UCSR1B |= (1<<RXCIE1) | (1<<TXCIE1) | (1<<RXEN1) | (1<<TXEN1); //Ativar porta Série (Interrupts + REceber e transmitir)
	UCSR1C |= (1<<UCSZ11) | (1<<UCSZ10); //Modo Assíncrono
	UBRR1H = ((MYUBR1)>>8);
	UBRR1L = MYUBR1;
}
