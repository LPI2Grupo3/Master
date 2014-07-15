#include <avr/interrupt.h>
#define F_CPU 16000000
#include <util/delay.h>

#include "serie.h"
#include "serieBlue.h"

int main(){

	SerieSetup0();
	SerieSetup1();
	sei();	
	while(1)
	{
		RX1 = 0;
		while(!RX1);

		if (UDR1=='E') putbuf(++count);		// Receiving an 'E' means someone entered: counter is incremented and sent to PC
		else if(count > 0) putbuf(--count);	// Otherwise, checks if there's someone inside and decrements counter, sending it to PC
	}
	return 0;
}
