
#define F_CPU 16000000UL	// Define processor clock speed for compiler

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();

//This function is used to initialize the USART
//at a given UBRR value

void USARTInit(uint16_t ubrr_value);
char USARTReadChar( void );
void USARTWriteChar(char data);

void chip_init (void);
void can_init (uint16_t Baud);
void can_rx(uint16_t NODE_ID);
void can_tx(uint16_t Adress);

volatile int8_t OntvangData[8];
volatile int8_t ZendData[8];
#define ADRESS 0x201 // Transmit Adress NODE ID 1
//#define ADRESS 0x317 // Transmit Adress Master
//#define NODEID 0x200 // NODE ID 2
//#define NODEID 0x201 //NODE ID 1
#define NODEID 0x317 //Master ID
#define BAUD 500

int main( void )
{
	char data;

	USARTInit(51);    //UBRR = 51
	chip_init();	// Chip initialization
	can_init(500); 	// Can initialization
	can_rx(NODEID);
	//void can_tx();
	DDRE = 0x10;
	//PORTE &= ~(1<<PORTE4);
	uint16_t toggle = 0;

	//Loop forever
	while(1)
	{
		
		if ((PINE & (1<<PE5)) == 0){	// If button "But" is pressed (Grounded) then:
			if(toggle == 0)
			{
				ZendData[0] = 0x3D;
				ZendData[1] = 0x01;
				ZendData[2] = 0x00;
				ZendData[3] = 0x00;
				//PORTE ^= (1<<PE4);			// LED TOOGLE
				can_tx(ADRESS);
			}
			toggle = 1;
		}
		else{							// Else if not button "But" is pressed(High) then:
			toggle = 0;
			PORTE |= (1<<PE4); 			// LED OFF
		}
		
		
		
		//data = USARTReadChar();
		
		//USARTWriteChar(data);

		
		
	}
return(0);
}


//***** Reception ISR **********************************
ISR(CANIT_vect){  				// use interrupts

	int8_t length, savecanpage;
	savecanpage = CANPAGE;			// Save current MOB
	CANPAGE = CANHPMOB & 0xF0;		// Selects MOB with highest priority interrupt

	length = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received
	for ( int8_t i = 0; i < length; i++ ){
		OntvangData[i] = CANMSG; 		// Get data, INDX auto increments CANMSG
		USARTWriteChar(OntvangData[i]);
	}
	if (OntvangData[0] == 0x3D) {
		if (OntvangData[1] == 0x01){
			//ZendData[0] = 0x3D;
			//ZendData[1] = 0x01;
			PORTE &= ~(1<<PE4);
		}
		
		if (OntvangData[2] == 0x02){
			ZendData[0] = 0x3D;
			ZendData[2] = 0x02;
			PORTE &= ~(1<<PE4);
		}
		
		if (OntvangData[3] == 0x03){
			ZendData[3] == 0x07;
		}
		//can_tx(ADRESS);
		for (int8_t i = 0; i < 8; i++){
			OntvangData[i] = 0;
			ZendData[i] = 0;
		}
	}
	
	
	CANSTMOB = 0x00;
	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));
	
	CANPAGE = savecanpage;		// Restore original MOB
}


void USARTInit(uint16_t ubrr_value){

	//Set Baud rate

	UBRR0L = ubrr_value;
	UBRR0H = (ubrr_value>>8);

	/*Set Frame Format


	>> Asynchronous mode
	>> No Parity
	>> 1 StopBit

	>> char size 8

	*/

	UCSR0C=(0<<UMSEL0)|(1<<UCSZ00)|(1<<UCSZ01);


	//Enable The receiver and transmitter

	UCSR0B=(1<<RXEN0)|(1<<TXEN0);


}


//This function is used to read the available data
//from USART. This function will wait until data is
//available.
char USARTReadChar( void )
{
	//Wait until a data is available

	while(!(UCSR0A & (1<<RXC0)))
	{
	}

	//Now USART has got data from host
	//and is available is buffer
	
	
	//USARTWriteChar(UDR0);
	USARTWriteChar('\n');

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

	DDRE = 0x10;	// stat led output
	PORTE |= (1<<PE4);

	//PORTB = 0x00;	// ALL LEDs OFF

	//PORTC = 0x00;	// Inputs, not used

	//PORTD = 0x00;	// Inputs, not used

	//PORTE = 0x00;	// Inputs, not used

	//PRR = 0x00;	// Individual peripheral clocks enabled

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
	
	CANIE2 = (( 1 << IEMOB1 ) | ( 1 << IEMOB0 ));   	// IEMOB1 = MOB1 Enable interrupts on mob1 for reception and transmission

	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 0 << ENTX );   // Enable interrupts on receive
	
	CANGCON |= ( 1 << 1 );		// ENASTB = 1	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read

	sei();

}
void can_rx(uint16_t NODE_ID){
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
	
	CANIDT1 = NODE_ID >> 3;   	// Ontvangst adress
	CANIDT2 = NODE_ID << 5; 		//
	CANIDT3 = 0x00; 	// ""
	CANIDT4 = 0x00; 	// ""
	
	CANIDM1 = 0xFF;   	// Ontvangst adress
	CANIDM2 = 0xFF; 		//
	CANIDM3 = 0x00; 	// ""
	CANIDM4 = 0x00; 	// ""

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));  // Enable Reception | 11 bit | IDE DLC8

}


//***** transmit *****************************************************
void can_tx(uint16_t Adress) {

	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = MOB0

	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANSTMOB = 0x00;    	// Clear mob status register
	
	CANIDT4 = 0x00;     		//
	CANIDT3 = 0x00;				//
	CANIDT2 = Adress << 5;		//
	CANIDT1 = Adress >> 3;		//
	
	for ( int8_t i = 0; i < 8; i++ ){
		CANMSG = ZendData[i];
	}
	
	CANCDMOB = (( 1 << CONMOB0 ) | ( 0 << IDE ) | ( 3 << DLC0));

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) );	// wait for TXOK flag set
	
	CANCDMOB = 0x00;

	CANSTMOB = 0x00;	// Clear TXOK flag
}

