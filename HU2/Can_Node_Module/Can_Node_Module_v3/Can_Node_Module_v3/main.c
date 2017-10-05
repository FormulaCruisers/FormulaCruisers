
#define F_CPU 16000000UL	// Define processor clock speed for compiler

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();


#define MASTERID	0x317 // Transmit Adress Master
#define NODEID1		0x201 // NODE ID 1
#define NODEID2		0x202 // NODE ID 2
#define NODEID3		0x203 // NODE ID 3
#define NODEID4		0x204 // NODE ID 4
#define NODEID5		0x205 // NODE ID 5
#define NODEID6		0x206 // NODE ID 6

#define FUNCTION MASTERID

#define BAUD 500


void USARTInit(uint16_t ubrr_value);
char USARTReadChar( void );
void USARTWriteChar(char data);

void chip_init (void);
void timer_init(void);

void can_init (uint16_t Baud);
void can_rx(uint16_t NODE_ID);
void can_tx(uint16_t Adress, uint8_t DLC);

void int_ADC(void);
void getADC(uint8_t ADC_pin);


volatile int8_t OntvangData[8];
volatile int8_t ZendData[8];
volatile int8_t Counter = 1;

uint16_t ADCValue[4] = {0x10,0x02AA,0x30,0x40};

int main( void )
{
	USARTInit(51);	// UBRR = 51  19200 Baud
	//timer_init();
	chip_init();	// Chip initialization
	can_init(500); 	// Can initialization
	int_ADC();
	sei();
	uint16_t toggle = 0;
	
	can_rx(FUNCTION);
	if(FUNCTION == MASTERID){
		while(1){
			if ((PINE & (1<<PE5)) == 0){	// If button "But" is pressed (Grounded) then:
				if(toggle == 0){
					
					ZendData[0] = 0x3D;
					ZendData[1] = 0x03;
					ZendData[2] = 0x01;
					ZendData[3] = 0x02;
					can_tx(NODEID1, 4);
				}
				toggle = 1;
			}
			else{							// Else if not button "But" is pressed(High) then:
				if(toggle == 1){
					ZendData[0] = 0x00;
					ZendData[1] = 0x01;
					ZendData[2] = 0x02;
					ZendData[3] = 0x03;
					can_tx(NODEID1, 4);
				}
				toggle = 0;
			}
		}
	}
	
	else{
		while(1){
			
			if ((PINE & (1<<PE5)) == 0){	// If button "But" is pressed (Grounded) then:
				if(toggle == 0){
					
					ZendData[0] = 0x3D;
					ZendData[1] = 0x01;
					ZendData[2] = 0x02;
					can_tx(MASTERID, 3);
					getADC(0);
				}
				toggle = 1;
			}
			else{							// Else if not button "But" is pressed(High) then:
				if(toggle == 1){
					ZendData[0] = 0x00;
					ZendData[1] = 0x03;
					ZendData[2] = 0x04;
					can_tx(MASTERID, 3);
				}
				toggle = 0;
			}
			
		}
	}
	
	return(0);
}

//*****Timer 0 Interrupt*****
ISR(TIMER0_COMP_vect) {
	Counter++;
	//if((Counter % 1) == 0){ //10ms
		//ZendData[0] = 0x3D;
		//ZendData[1] = 0x99;
		//ZendData[2] = 0x98;
		//ZendData[3] = Counter;
		//can_tx(NODEID1, 4);
	//}
	
	if((Counter % 20) == 0){; //200ms
		ZendData[0] = 0x3D;
		ZendData[1] = 0x01;
		//ZendData[2] = 0x01;
		//ZendData[3] = 0x01;
		can_tx(NODEID1, 2);
	}
	if(((Counter + 3) % 50) == 0){ //500ms
		ZendData[0] = 0x3D;
		ZendData[1] = 0x02;
		//ZendData[2] = 0x44;
		//ZendData[3] = 0x55;
		can_tx(NODEID1, 2);
	}
	if(((Counter + 6) % 100) == 0){ //1s
		ZendData[0] = 0x3D;
		ZendData[1] = 0x03;
		//ZendData[2] = 0x02;
		//ZendData[3] = 0x03;
		can_tx(NODEID1, 2);
	}
	
	if (Counter == 100){
		Counter = 1;
	}
	TIFR0 |= (1 << OCF0A);
	
}


//***** Reception ISR **********************************
ISR(CANIT_vect){  					// use interrupts

	int8_t length;
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1

	length = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received
	for ( int8_t i = 0; i < length; i++ ){
		OntvangData[i] = CANMSG; 		// Get data, INDX auto increments CANMSG
		USARTWriteChar(OntvangData[i]);
	}
	
	uint16_t OntvangAdress = (CANIDT1 << 3) | ((CANIDT2 & 0b11100000) >> 5);
	
	//USARTWriteChar((OntvangAdress & 0xFF00) >> 8);
	//USARTWriteChar(OntvangAdress & 0x00FF);
	
	
	/*
	if(OntvangAdress == MASTERID){
		
		for (uint8_t i = 0; i < length; i = i ){
			if ((OntvangData[i] & 0b00111111) == 0x01){
				USARTWriteChar(OntvangData[i] & 0b00111111);
				USARTWriteChar((OntvangData[i] & 0b11000000) >> 6);
				USARTWriteChar(OntvangData[i + 1]);
				i += 2;
			}
			if ((OntvangData[i] & 0b00111111) == 0x02){
				USARTWriteChar(OntvangData[i] & 0b00111111);
				USARTWriteChar((OntvangData[i] & 0b11000000) >> 6);
				USARTWriteChar(OntvangData[i + 1]);
				i += 2;
			}
			if ((OntvangData[i] & 0b00111111) == 0x03){
				USARTWriteChar(OntvangData[i] & 0b00111111);
				USARTWriteChar((OntvangData[i] & 0b11000000) >> 6);
				USARTWriteChar(OntvangData[i + 1]);
				i += 2;
			}
			if ((OntvangData[i] & 0b00111111) == 0x04){
				USARTWriteChar(OntvangData[i] & 0b00111111);
				USARTWriteChar((OntvangData[i] & 0b11000000) >> 6);
				USARTWriteChar(OntvangData[i + 1]);
				i += 2;
			}
		}
	}
	
	
	if(OntvangAdress == NODEID1){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			uint8_t j = 0;
			for(uint8_t i = 1; i < length; i++){
				if (OntvangData[i] == 0x01){
					ZendData[j++] = 0x01 | ((ADCValue[0] >> 2) & 0b11000000 ) ;
					ZendData[j++]		= ADCValue[0];
				}
				if (OntvangData[i] == 0x02){
					ZendData[j++] = 0x02 | ((ADCValue[1] >> 2) & 0b11000000 ) ;
					ZendData[j++]		= ADCValue[1];
				}
				if (OntvangData[i] == 0x03){
					ZendData[j++] = 0x03 | ((ADCValue[2] >> 2) & 0b11000000 ) ;
					ZendData[j++]		= ADCValue[2];
				}
				if (OntvangData[i] == 0x04){
					ZendData[j++] = 0x04 | ((ADCValue[3] >> 2) & 0b11000000 ) ;
					ZendData[j++]		= ADCValue[3];
				}
			}
			
			can_tx(MASTERID, j);
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	if(OntvangAdress == NODEID2){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			if (OntvangData[1] == 0x01){
				ZendData[0] = 0x01 | (ADCValue[0] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[2] == 0x02){
				ZendData[0] = 0x02 | (ADCValue[1] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[3] == 0x03){
				ZendData[0] = 0x03 | (ADCValue[2] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[3];
			}
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	if(OntvangAdress == NODEID3){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			if (OntvangData[1] == 0x01){
				ZendData[0] = 0x01 | (ADCValue[0] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[2] == 0x02){
				ZendData[0] = 0x02 | (ADCValue[1] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[3] == 0x03){
				ZendData[0] = 0x03 | (ADCValue[2] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[3];
			}
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	if(OntvangAdress == NODEID4){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			if (OntvangData[1] == 0x01){
				ZendData[0] = 0x01 | (ADCValue[0] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[2] == 0x02){
				ZendData[0] = 0x02 | (ADCValue[1] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[3] == 0x03){
				ZendData[0] = 0x03 | (ADCValue[2] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[3];
			}
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	if(OntvangAdress == NODEID5){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			if (OntvangData[1] == 0x01){
				ZendData[0] = 0x01 | (ADCValue[0] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[2] == 0x02){
				ZendData[0] = 0x02 | (ADCValue[1] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[3] == 0x03){
				ZendData[0] = 0x03 | (ADCValue[2] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[3];
			}
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	if(OntvangAdress == NODEID6){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			if (OntvangData[1] == 0x01){
				ZendData[0] = 0x01 | (ADCValue[0] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[2] == 0x02){
				ZendData[0] = 0x02 | (ADCValue[1] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[0];
			}
			if (OntvangData[3] == 0x03){
				ZendData[0] = 0x03 | (ADCValue[2] >> 2 & 0b11000000 ) ;
				ZendData[1] = ADCValue[3];
			}
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	*/
	
	for (int8_t i = 0; i < 8; i++){
		OntvangData[i] = 0;
		ZendData[i] = 0;
	}
	
	
	CANSTMOB = 0x00;
	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = MOB0
	
	USARTWriteChar(0xDD);
}


void USARTInit(uint16_t ubrr_value){

	//Set Baud rate
	UBRR0L = ubrr_value;
	UBRR0H = (ubrr_value>>8);

	/*	Set Frame Format:
	>> Asynchronous mode
	>> No Parity
	>> 1 StopBit
	>> char size 8		*/
	UCSR0C=(0<<UMSEL0)|(1<<UCSZ00)|(1<<UCSZ01);

	//Enable The receiver and transmitter
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
}


//This function is used to read the available data
//from USART. This function will wait until data is
//available.
char USARTReadChar( void )
{
	while(!(UCSR0A & (1<<RXC0)))	{}	// Wait until data is ready on the RS232

	USARTWriteChar(UDR0);
	//USARTWriteChar('\n');

	return UDR0;	// Return data from function
}


//This function writes the given "data" to
//the USART which then transmit it via TX line
void USARTWriteChar(char data)
{
	while(!(UCSR0A & (1<<UDRE0))){}	//Wait until previous writing to rs232 is ready

	UDR0=data;	// Write data to RS232
}

void timer_init(void){
	//Timer 1
	//Prescaler 1024
	// Tellen tot 156
	TCNT0= 0;
	TCCR0A |= (1<<CS02) | (1<<CS00) | (1<<WGM01);
	TIMSK0 |= (1<<OCIE0A);
	OCR0A = 156;
	TIFR0 |= (1 << OCF0A);
}


//***** chip Initialization **********************************
void chip_init(void){
	
	
	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable
	CLKPR = 0x00;				// no prescaler CLK 16Mhz
	
	DDRE = 0x10;	// stat led output
	PORTE |= (1<<PE4);
}

//***** CAN ialization *****************************************************
void can_init(uint16_t Baud){

	CANGCON = ( 1 << SWRES );   // Software reset
	
	CANTCON = 0x00;         // CAN timing prescaler set to 0;
	
	if (Baud == 500){
		CANBT1 = 0x02;      	// Set baud rate to 500kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;          // "
		CANBT3 = 0x37; 			// ""
	}
	if (Baud == 250){
		CANBT1 = 0x0E;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
		CANBT2 = 0x04;          // "
		CANBT3 = 0x13; 			// ""
	}
	if ( Baud == 125){
		CANBT1 = 0x06;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;          // "
		CANBT3 = 0x37; 			// ""
	}
	

	for ( int8_t mob=0; mob<14; mob++ ) {

		CANPAGE = ( mob << 4 );     	// Selects Message Object 0-14
		CANCDMOB = 0x00;       		// Disable mob
		CANSTMOB = 0x00;     		// Clear mob status register;
	}
	
	CANIE2 = (( 1 << IEMOB1 ) | ( 0 << IEMOB0 ));   	// IEMOB1 = MOB1 Enable interrupts on mob1 for reception and transmission
	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 0 << ENTX );   // Enable interrupts on receive
	CANGCON |= ( 1 << 1 );		// ENASTB = 1	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
	
}

//***** CAN Creating RX *****************************************************
void can_rx(uint16_t NODE_ID){
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
	
	CANIDT1 = NODE_ID >> 3;   	// Ontvangst adress
	CANIDT2 = NODE_ID << 5;		//
	CANIDT3 = 0x00; 			// ""
	CANIDT4 = 0x00; 			// ""
	
	//CANIDM1 = 0b11111111;   // Ontvangst adress
	//CANIDM2 = 0b11100000;	//
	CANIDM1 = 0b00000000;   // Ontvangst adress
	CANIDM2 = 0b00000000;	//
	
	CANIDM3 = 0b00000000; 	// ""
	CANIDM4 = 0b00000000; 	// ""

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));  // Enable Reception | 11 bit | IDE DLC8

}


//***** CAN Creating TX *****************************************************
void can_tx(uint16_t Adress, uint8_t DLC) {
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = MOB0

	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANSTMOB = 0x00;    		// Clear mob status register
	
	CANIDT4 = 0x00;     		//
	CANIDT3 = 0x00;				//
	CANIDT2 = Adress << 5;		//
	CANIDT1 = Adress >> 3;		//
	
	for ( int8_t i = 0; i < 8; i++ ){
		CANMSG = ZendData[i];
	}
	
	CANCDMOB = (( 1 << CONMOB0 ) | ( 0 << IDE ) | ( DLC << DLC0));

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) );	// wait for TXOK flag set
	
	CANCDMOB = 0x00;

	CANSTMOB = 0x00;	// Clear TXOK flag
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
}


void int_ADC(void){
	DDRF	= 0x00;
	PORTF	= 0x00;
	
	ADMUX	= ( 1 << REFS0 )
	| ( 1 << REFS1 )
	| ( 0 << ADLAR )
	| ( 0 << MUX4 )
	| ( 0 << MUX3 )
	| ( 0 << MUX2 )
	| ( 0 << MUX1 )
	| ( 0 << MUX0 );
	ADCSRA	= ( 1 << ADEN )
	| ( 0 << ADSC )
	| ( 0 << ADATE )
	| ( 0 << ADIF )
	| ( 1 << ADIE )
	| ( 1 << ADPS2 )
	| ( 1 << ADPS1 )
	| ( 1 << ADPS0 );
}

void getADC(uint8_t ADC_pin){
	ADMUX	= (ADMUX & 0b11111100) | (ADC_pin & 0b00000011);
	ADCSRA |= ( 1 << ADSC);
}


ISR(ADC_vect){
	uint16_t ReadValue = 0;
	ReadValue = ADCL;
	ReadValue = ReadValue | (ADCH << 8);
	
	ADCValue[ADMUX & 0b00000011] = ReadValue;
	
	//USARTWriteChar((i & 0xFF00) >> 8);
	//USARTWriteChar(i & 0x00FF);
	USARTWriteChar(ReadValue >> 8);
	USARTWriteChar(ReadValue);
}

