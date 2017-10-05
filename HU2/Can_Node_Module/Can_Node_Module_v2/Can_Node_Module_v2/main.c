
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
void can_tx(uint16_t Adress, uint8_t DLC);

volatile int8_t OntvangData[8];
volatile int8_t ZendData[8];

#define MASTERID	0x317 // Transmit Adress Master
#define NODEID1		0x201 // NODE ID 1
#define NODEID2		0x202 // NODE ID 1
#define NODEID3		0x203 // NODE ID 1
#define NODEID4		0x204 // NODE ID 1
#define NODEID5		0x205 // NODE ID 5
#define NODEID6		0x206 // NODE ID 6

#define FUNCTION NODEID1

#define BAUD 500

int main( void )
{
	USARTInit(51);    //UBRR = 51  19200 Baud
	chip_init();	// Chip initialization
	can_init(500); 	// Can initialization
	
	can_rx(FUNCTION);
	if(FUNCTION == MASTERID){
		while(1){}
	}
	else{
		uint16_t toggle = 0;
		while(1){
			if ((PINE & (1<<PE5)) == 0){	// If button "But" is pressed (Grounded) then:
				if(toggle == 0){
					
					ZendData[0] = 0x3D;
					ZendData[1] = 0x01;
					ZendData[2] = 0x02;
					ZendData[3] = 0x03;
					can_tx(MASTERID, 3);
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
		return(0);
	}
}


//***** Reception ISR **********************************
ISR(CANIT_vect){  					// use interrupts

	int8_t length, savecanpage;
	savecanpage = CANPAGE;			// Save current MOB
	CANPAGE = CANHPMOB & 0xF0;		// Selects MOB with highest priority interrupt

	length = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received
	for ( int8_t i = 0; i < length; i++ ){
		OntvangData[i] = CANMSG; 		// Get data, INDX auto increments CANMSG
		USARTWriteChar(OntvangData[i]);
	}
	
	uint16_t OntvangAdress = (CANIDT1 << 3) | ((CANIDT2 & 0b11100000) >> 5);
	
	USARTWriteChar((OntvangAdress & 0xFF00) >> 8);
	USARTWriteChar(OntvangAdress & 0x00FF);
	
	
	if(OntvangAdress == MASTERID){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	if(OntvangAdress == NODEID1){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			if (OntvangData[1] == 0x01){
				ZendData[1] = 0x01;
			}
			if (OntvangData[2] == 0x02){
				ZendData[2] = 0x02;
			}
			if (OntvangData[3] == 0x03){
				ZendData[3] = 0x03;
			}
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	if(OntvangAdress == NODEID2){
		if (OntvangData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			if (OntvangData[1] == 0x01){
				ZendData[1] = 0x01;
			}
			if (OntvangData[2] == 0x02){
				ZendData[2] = 0x02;
			}
			if (OntvangData[3] == 0x03){
				ZendData[3] = 0x03;
			}
		}
		else{
			PORTE |= (1<<PE4);
		}
		if(OntvangAdress == NODEID3){
			if (OntvangData[0] == 0x3D) {
				PORTE &= ~(1<<PE4);
				if (OntvangData[1] == 0x01){
					ZendData[1] = 0x01;
				}
				if (OntvangData[2] == 0x02){
					ZendData[2] = 0x02;
				}
				if (OntvangData[3] == 0x03){
					ZendData[3] = 0x03;
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
					ZendData[1] = 0x01;
				}
				if (OntvangData[2] == 0x02){
					ZendData[2] = 0x02;
				}
				if (OntvangData[3] == 0x03){
					ZendData[3] = 0x03;
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
					ZendData[1] = 0x01;
				}
				if (OntvangData[2] == 0x02){
					ZendData[2] = 0x02;
				}
				if (OntvangData[3] == 0x03){
					ZendData[3] = 0x03;
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
					ZendData[1] = 0x01;
				}
				if (OntvangData[2] == 0x02){
					ZendData[2] = 0x02;
				}
				if (OntvangData[3] == 0x03){
					ZendData[3] = 0x03;
				}
			}
			else{
				PORTE |= (1<<PE4);
			}
		}
		
		
		for (int8_t i = 0; i < 8; i++){
			OntvangData[i] = 0;
			ZendData[i] = 0;
		}
		
		CANSTMOB = 0x00;
		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));
		
		CANPAGE = savecanpage;		// Restore original MOB
	}
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
	
	CANIE2 = (( 1 << IEMOB1 ) | ( 1 << IEMOB0 ));   	// IEMOB1 = MOB1 Enable interrupts on mob1 for reception and transmission
	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 0 << ENTX );   // Enable interrupts on receive
	CANGCON |= ( 1 << 1 );		// ENASTB = 1	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
	
	sei();
}

//***** CAN Creating RX *****************************************************
void can_rx(uint16_t NODE_ID){
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
	
	CANIDT1 = NODE_ID >> 3;   	// Ontvangst adress
	CANIDT2 = NODE_ID << 5;		//
	CANIDT3 = 0x00; 			// ""
	CANIDT4 = 0x00; 			// ""
	
	CANIDM1 = 0b11111111;   // Ontvangst adress
	CANIDM2 = 0b11100000;	//
	//CANIDM1 = 0b00000000;   // Ontvangst adress
	//CANIDM2 = 0b00000000;	//
	
	CANIDM3 = 0b00000000; 	// ""
	CANIDM4 = 0b00000000; 	// ""

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));  // Enable Reception | 11 bit | IDE DLC8

}


//***** CAN Creating TX *****************************************************
void can_tx(uint16_t Adress, uint8_t DLC) {

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
	
	CANCDMOB = (( 1 << CONMOB0 ) | ( 0 << IDE ) | ( DLC << DLC0));

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) );	// wait for TXOK flag set
	
	CANCDMOB = 0x00;

	CANSTMOB = 0x00;	// Clear TXOK flag
}

