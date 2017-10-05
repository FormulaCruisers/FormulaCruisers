
#define F_CPU 16000000UL	// Define processer clock speed for compiler

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();

//This function is used to initialize the USART
//at a given UBRR value

void USARTInit(uint16_t ubrr_value);
char USARTReadChar( void );
void USARTWriteChar(char data);

void can_tx(uint16_t Address, uint8_t DLC);
void chip_init (void);
void can_init (void);

volatile int8_t OntvangData[8];
volatile int8_t ZendData[8];

int main( void )
{
	uint16_t adres = 0;
	char data;
	/*
	Value	= Baud
	51		= 19200
	34		= 28800
	25		= 38400
	16		= 57600
	12		= 76800
	8		= 115200
	3		= 230400
	3		= 250000
	1		= 500000
	0		= 1000000



	*/
	USARTInit(8);    // 51 = 19200 |  8 = 115200
	chip_init();	// Chip initialization
	can_init(); 	// Can initialization
	DDRE = 0x10;
	PORTE &= ~(1<<PORTE4);

	//Loop forever
	
	uint8_t Bytes = 0;


	while(1)
	{
		data=USARTReadChar();
		USARTWriteChar(data);
		Bytes = data;
		
		data=USARTReadChar();
		USARTWriteChar(data);
		adres = data << 8;
		
		data=USARTReadChar();
		USARTWriteChar(data);
		adres += data;
		
		for(uint8_t i = 0; i < Bytes; i++){
			data=USARTReadChar();
			USARTWriteChar(data);
			ZendData[i] = data;
		}
		
		USARTWriteChar(0xDD);
		
		can_tx(adres, Bytes);
		
		
	}
	return(0);
}


//***** Reception ISR **********************************
ISR(CANIT_vect){  				// use interrupts

	int8_t length, savecanpage;
	savecanpage = CANPAGE;			// Save current MOB
	CANPAGE = CANHPMOB & 0xF0;		// Selects MOB with highest priority interrupt
	
	uint16_t ReceiveAddress = (CANIDT1 << 3) | ((CANIDT2 & 0b11100000) >> 5);
	
	USARTWriteChar((ReceiveAddress >> 8) & 0xFF);
	USARTWriteChar(ReceiveAddress & 0xFF);
	

	if ( CANSTMOB & ( 1 << RXOK) ){  	// Interrupt caused by receive finished
		length = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received
		for ( int8_t i = 0; i < length; i++ ){
			OntvangData[i] = CANMSG; 		// Get data, INDX auto increments CANMSG
			USARTWriteChar(OntvangData[i]);
		}
		
		CANSTMOB = 0x00;
		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));
	}

	CANPAGE = savecanpage;		// Restore original MOB
}


void USARTInit(uint16_t ubrr_value){

	//Set Baud rate

	UBRR0L = ubrr_value;
	UBRR0H = (ubrr_value>>8);

	/*Set Frame Format


	>> Asynchronous modet8
	>> No Parity
	>> 1 StopBit

	>> char size 8

	*/

	UCSR0C=(0<<UMSEL0)|(1<<UCSZ00)|(1<<UCSZ01);


	//Enable The receiver and transmitter

	UCSR0B=(1<<RXEN0)|(1<<TXEN0);


}


//This function is used to read the available data
//from USART. This function will wait untill data is
//available.
char USARTReadChar( void )
{
	//Wait until a data is available

	while(!(UCSR0A & (1<<RXC0)))
	{
		//return 0;
	}

	//Now USART has got data from host
	//and is available is buffer
	

	return UDR0;
}


//This function writes the given "data" to
//the USART which then transmit it via TX line
void USARTWriteChar(char data)
{
	//Wait until the transmitter is ready

	while(!(UCSR0A & (1<<UDRE0)))
	{
		//Do nothing
	}

	//Now write the data to USART buffer

	UDR0=data;
}


//***** chip Initialization **********************************
void chip_init(void){

	// If using crystal oscillator of 16 Mhz
	// Note - May want to set the CKDIV8 fuse if MCU is only supposed to run
	// at 8MHz so as not to exceed its operating conditions at startup.

	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable

	CLKPR = 0x00;				// Clock Division Factor of 2
	
	// Note - CLKPR must be written within 4 clock cycles of setting CLKPCE
	
	//DDRB = 0x00;	// PORT B to be outputs for LED monotoring

	//DDRC = 0x00;    // Inputs, not used

	//DDRD = 0x00; 	// Inputs, not used

	//DDRE = 0x10;	// stat led output

	//PORTB = 0x00;	// ALL LEDs OFF

	//PORTC = 0x00;	// Inputs, not used

	//PORTD = 0x00;	// Inputs, not used

	//PORTE = 0x00;	// Inputs, not used

	//PRR = 0x00;	// Individual peripheral clocks enabled

}

//***** CAN ialization *****************************************************
void can_init(void){

	CANGCON = ( 1 << SWRES );   // Software reset
	

	CANTCON = 0x00;         // CAN timing prescaler set to 0;
	
	CANBT1 = 0x02;      	// Set baud rate to 500kb (assuming 16Mhz IOclk)
	CANBT2 = 0x0C;          // "
	CANBT3 = 0x37; 			// ""

	/*
	CANBT1 = 0x0E;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
	CANBT2 = 0x04;          // "
	CANBT3 = 0x13; 			// ""

	CANBT1 = 0x06;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
	CANBT2 = 0x0C;          // "
	CANBT3 = 0x37; 			// ""
	*/

	for ( int8_t mob=0; mob<14; mob++ ) {

		CANPAGE = ( mob << 4 );     	// Selects Message Object 0-14

		CANCDMOB = 0x00;       		// Disable mob

		CANSTMOB = 0x00;     		// Clear mob status register;
	}
	
	CANIE2 = (( 1 << IEMOB1 ) | ( 0 << IEMOB0 ));   	// IEMOB1 = RX -- IEMOB0 = TX

	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 0 << ENTX );   // Enable interrupts on receive
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1

	CANIDM1 = 0x00;   	// Clear Mask, let all IDs pass
	CANIDM2 = 0x00; 	// ""
	CANIDM3 = 0x00; 	// ""
	CANIDM4 = 0x00; 	// ""

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));  // Enable Reception | 11 bit | IDE DLC8
	
	CANGCON |= ( 1 << 1 );		// ENASTB = 1	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read

	sei();

}




//***** transmit *****************************************************
void can_tx(uint16_t Address, uint8_t DLC) {
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0

	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANSTMOB = 0x00;   // Clear mob status register
	
	CANIDT4 = 0x00;     		//
	CANIDT3 = 0x00;				//
	CANIDT2 = Address << 5;		//
	CANIDT1 = Address >> 3;		//
	
	for ( int8_t i = 0; i < 8; i++ ){
		CANMSG = ZendData[i]; //CAN Data Message Register: setting the data in the message register
	}
	
	CANCDMOB = (( 1 << CONMOB0 ) | ( 0 << IDE ) | ( DLC << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (DLC << DLC0) Set *DLC* Bytes in the data field of the message.

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) );	// wait for TXOK flag set
	
	CANCDMOB = 0x00; //Clear CAN Mob Control and DLC Register

	CANSTMOB = 0x00; // Clear TXOK flag
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
}
