
/* SD_RAW.C
This file contains the basic functions necessary for sending raw data to the SD card.

The original file is Copyright (c) 2006-2012 by Roland Riegel <feedback@roland-riegel.de>, distributed under the GNU GPL version 2 or the GNU LGPL version 2.1
Modifications 2017 are by Richard Kokx, Formula Cruisers Team, The Hague University of Applied Sciences
The SPI_MasterInit() function is originally Copyright (c) 2008 Atmel Corporation, and has been copied from the AT90CAN128 datasheet (version 7679H-CAN-08/08)
 */

#include "sd_raw.h"
#include "Error.h"
#include <string.h>
#include <avr/eeprom.h>
#include <stdio.h>

extern volatile enum _error _errorcode;
extern uint8_t sdbuffer[BLOCK_SIZE];
extern uint16_t sd_current_pos;
uint32_t EEMEM ee_sd_start_block = 0;
uint32_t sd_next_block;
extern uint16_t boot_count;

/* commands available in SPI mode */

/* CMD0: response R1 */
#define CMD_GO_IDLE_STATE 0x00
/* CMD1: response R1 */
#define CMD_SEND_OP_COND 0x01
/* CMD9: response R1 */
#define CMD_SEND_CSD 0x09
/* CMD10: response R1 */
#define CMD_SEND_CID 0x0a
/* CMD12: response R1 */
#define CMD_STOP_TRANSMISSION 0x0c
/* CMD13: response R2 */
#define CMD_SEND_STATUS 0x0d
/* CMD16: arg0[31:0]: block length, response R1 */
#define CMD_SET_BLOCKLEN 0x10
/* CMD17: arg0[31:0]: data address, response R1 */
#define CMD_READ_SINGLE_BLOCK 0x11
/* CMD18: arg0[31:0]: data address, response R1 */
#define CMD_READ_MULTIPLE_BLOCK 0x12
/* CMD24: arg0[31:0]: data address, response R1 */
#define CMD_WRITE_SINGLE_BLOCK 0x18
/* CMD25: arg0[31:0]: data address, response R1 */
#define CMD_WRITE_MULTIPLE_BLOCK 0x19
/* CMD27: response R1 */
#define CMD_PROGRAM_CSD 0x1b
/* CMD28: arg0[31:0]: data address, response R1b */
#define CMD_SET_WRITE_PROT 0x1c
/* CMD29: arg0[31:0]: data address, response R1b */
#define CMD_CLR_WRITE_PROT 0x1d
/* CMD30: arg0[31:0]: write protect data address, response R1 */
#define CMD_SEND_WRITE_PROT 0x1e
/* CMD32: arg0[31:0]: data address, response R1 */
#define CMD_TAG_SECTOR_START 0x20
/* CMD33: arg0[31:0]: data address, response R1 */
#define CMD_TAG_SECTOR_END 0x21
/* CMD34: arg0[31:0]: data address, response R1 */
#define CMD_UNTAG_SECTOR 0x22
/* CMD35: arg0[31:0]: data address, response R1 */
#define CMD_TAG_ERASE_GROUP_START 0x23
/* CMD36: arg0[31:0]: data address, response R1 */
#define CMD_TAG_ERASE_GROUP_END 0x24
/* CMD37: arg0[31:0]: data address, response R1 */
#define CMD_UNTAG_ERASE_GROUP 0x25
/* CMD38: arg0[31:0]: stuff bits, response R1b */
#define CMD_ERASE 0x26
/* CMD42: arg0[31:0]: stuff bits, response R1b */
#define CMD_LOCK_UNLOCK 0x2a
/* CMD58: response R3 */
#define CMD_READ_OCR 0x3a
/* CMD59: arg0[31:1]: stuff bits, arg0[0:0]: crc option, response R1 */
#define CMD_CRC_ON_OFF 0x3b

/* command responses */
/* R1: size 1 byte */
#define R1_IDLE_STATE 0
#define R1_ERASE_RESET 1
#define R1_ILL_COMMAND 2
#define R1_COM_CRC_ERR 3
#define R1_ERASE_SEQ_ERR 4
#define R1_ADDR_ERR 5
#define R1_PARAM_ERR 6
/* R1b: equals R1, additional busy bytes */
/* R2: size 2 bytes */
#define R2_CARD_LOCKED 0
#define R2_WP_ERASE_SKIP 1
#define R2_ERR 2
#define R2_CARD_ERR 3
#define R2_CARD_ECC_FAIL 4
#define R2_WP_VIOLATION 5
#define R2_INVAL_ERASE 6
#define R2_OUT_OF_RANGE 7
#define R2_CSD_OVERWRITE 7
#define R2_IDLE_STATE (R1_IDLE_STATE + 8)
#define R2_ERASE_RESET (R1_ERASE_RESET + 8)
#define R2_ILL_COMMAND (R1_ILL_COMMAND + 8)
#define R2_COM_CRC_ERR (R1_COM_CRC_ERR + 8)
#define R2_ERASE_SEQ_ERR (R1_ERASE_SEQ_ERR + 8)
#define R2_ADDR_ERR (R1_ADDR_ERR + 8)
#define R2_PARAM_ERR (R1_PARAM_ERR + 8)
/* R3: size 5 bytes */
#define R3_OCR_MASK (0xffffffffUL)
#define R3_IDLE_STATE (R1_IDLE_STATE + 32)
#define R3_ERASE_RESET (R1_ERASE_RESET + 32)
#define R3_ILL_COMMAND (R1_ILL_COMMAND + 32)
#define R3_COM_CRC_ERR (R1_COM_CRC_ERR + 32)
#define R3_ERASE_SEQ_ERR (R1_ERASE_SEQ_ERR + 32)
#define R3_ADDR_ERR (R1_ADDR_ERR + 32)
#define R3_PARAM_ERR (R1_PARAM_ERR + 32)
/* Data Response: size 1 byte */
#define DR_STATUS_MASK 0x0e
#define DR_STATUS_ACCEPTED 0x05
#define DR_STATUS_CRC_ERR 0x0a
#define DR_STATUS_WRITE_ERR 0x0c

/* private helper functions */
static void sd_raw_send_byte(uint8_t b);
static uint8_t sd_raw_rec_byte();
static uint8_t sd_raw_send_command_r1(uint8_t command, uint32_t arg);
static uint16_t sd_raw_send_command_r2(uint8_t command, uint32_t arg);

/**
 * Initializes memory card communication.
 *
 * \returns 0 on failure, 1 on success.
 */

uint8_t sd_flush_buffer(){
	if(sd_raw_write_block(sd_next_block, sdbuffer, sd_current_pos + 1)){
		memset(sdbuffer, 0xff, BLOCK_SIZE);
		sd_current_pos = 0;
		sd_next_block++;
		eeprom_write_dword(&ee_sd_start_block, sd_next_block);
		return 1;
	}
	return 0; //Error, _errorcode has already been set by sd_raw_write_block()
}

uint8_t sd_write(char* buffer, int len){
	for(int i = 0; i < len; i++){
		sd_check_and_flush();
		sdbuffer[sd_current_pos] = buffer[i];
		sd_current_pos++;
	}
	return 1;
}

uint8_t sd_check_and_flush(){
	if(sd_current_pos >= BLOCK_SIZE){
		return sd_flush_buffer();
	}
	return 1;
}

uint8_t sd_write_nullterminated(char* buffer){ //for zero-terminated strings
	return sd_write(buffer, strlen((buffer) + 1));
}

uint8_t sd_raw_init()
{
	/* enable outputs for MOSI, SCK, SS, input for MISO */
	SPI_MasterInit();

	unselect_card();

	/* initialization procedure */
	
	/* card needs 74 cycles minimum to start up */
	for(uint8_t i = 0; i < 10; ++i)
	{
		/* wait 8 clock cycles */
		sd_raw_rec_byte();
	}

	/* address card */
	select_card();

	/* reset card */
	uint8_t response;
	for(uint16_t i = 0; ; ++i)
	{
		response = sd_raw_send_command_r1(CMD_GO_IDLE_STATE, 0);
		if(response == (1 << R1_IDLE_STATE))
			break;

		if(i == 0x1ff)
		{
			unselect_card();
			_errorcode = ERROR_SD_INIT_RESET;
			return 0;
		}
	}
	
	/* wait for card to get ready */
	for(uint16_t i = 0; ; ++i)
	{
		response = sd_raw_send_command_r1(CMD_SEND_OP_COND, 0);
		if(!(response & (1 << R1_IDLE_STATE)))
			break;

		if(i == 0x7fff)
		{
			unselect_card();
			_errorcode = ERROR_SD_INIT_READY;
			return 0;
		}
	}

	/* set block size to 512 bytes */
	if(sd_raw_send_command_r1(CMD_SET_BLOCKLEN, BLOCK_SIZE))
	{
		unselect_card();
		_errorcode = ERROR_SD_BLOCK;
		return 0;
	}
	
	/* Get the start block from EEPROM*/
	sd_next_block = eeprom_read_dword(&ee_sd_start_block);
	char buffer[8];
	snprintf(buffer, sizeof(buffer), "BOOT%03d", boot_count % 1000);
	sd_write_nullterminated(buffer);

	/* deaddress card */
	unselect_card();
	return 1;
}

void sd_raw_wait_ready(){
	uint8_t response;
	select_card();
	
	/* wait for card to get ready */
	for(uint16_t i = 0; ; ++i)
	{
		response = sd_raw_send_command_r1(CMD_SEND_OP_COND, 0);
		if(!(response & (1 << R1_IDLE_STATE)))
			break;

		if(i == 0x7fff)
		{
			unselect_card();
			_errorcode = ERROR_SD_NOTREADY;
			return;
		}
	}
	
	unselect_card();
}

/**
 * \ingroup sd_raw
 * Sends a raw byte to the memory card.
 *
 * \param[in] b The byte to sent.
 * \see sd_raw_rec_byte
 */
void sd_raw_send_byte(uint8_t b)
{
	SPDR = b;
	/* wait for byte to be shifted out */
	while(!(SPSR & (1 << SPIF)));
	SPSR &= ~(1 << SPIF);
}

/**
 * \ingroup sd_raw
 * Receives a raw byte from the memory card.
 *
 * \returns The byte which should be read.
 * \see sd_raw_send_byte
 */
uint8_t sd_raw_rec_byte()
{
	/* send dummy data for receiving some */
	SPDR = 0xff;
	while(!(SPSR & (1 << SPIF)));
	SPSR &= ~(1 << SPIF);

	return SPDR;
}

/**
 * \ingroup sd_raw
 * Send a command to the memory card which responses with a R1 response.
 *
 * \param[in] command The command to send.
 * \param[in] arg The argument for command.
 * \returns The command answer.
 */
uint8_t sd_raw_send_command_r1(uint8_t command, uint32_t arg)
{
	uint8_t response;

	/* wait some clock cycles */
	sd_raw_rec_byte();

	/* send command via SPI */
	sd_raw_send_byte(0x40 | command);
	sd_raw_send_byte((arg >> 24) & 0xff);
	sd_raw_send_byte((arg >> 16) & 0xff);
	sd_raw_send_byte((arg >> 8) & 0xff);
	sd_raw_send_byte((arg >> 0) & 0xff);
	sd_raw_send_byte(command == CMD_GO_IDLE_STATE ? 0x95 : 0xff);
	
	/* receive response */
	for(uint8_t i = 0; i < 10; ++i)
	{
		response = sd_raw_rec_byte();
		if(response != 0xff)
			break;
	}

	return response;
}

/*void decodeResponse1(char response){
	Serial.println("----- SD COMMAND Response -----");
	if ((response & 0x01) != 0) 
	Serial.println("in idle state");
	if ((response & 0x02) != 0) 
	Serial.println("Erase reset");
	if ((response & 0x04) != 0) 
	Serial.println("Illegal command");
	if ((response & 0x08) != 0) 
	Serial.println("CRC error");
	if ((response & 0x10) != 0) 
	Serial.println("Erase sequence error");
	if ((response & 0x20) != 0) 
	Serial.println("Address error");
	if ((response & 0x40) != 0) 
	Serial.println("Parameter Error");  
} */

/**
 * \ingroup sd_raw
 * Send a command to the memory card which responses with a R2 response.
 *
 * \param[in] command The command to send.
 * \param[in] arg The argument for command.
 * \returns The command answer.
 */
uint16_t sd_raw_send_command_r2(uint8_t command, uint32_t arg)
{
	uint16_t response;
	
	/* wait some clock cycles */
	sd_raw_rec_byte();

	/* send command via SPI */
	sd_raw_send_byte(0x40 | command);
	sd_raw_send_byte((arg >> 24) & 0xff);
	sd_raw_send_byte((arg >> 16) & 0xff);
	sd_raw_send_byte((arg >> 8) & 0xff);
	sd_raw_send_byte((arg >> 0) & 0xff);
	sd_raw_send_byte(command == CMD_GO_IDLE_STATE ? 0x95 : 0xff);
	
	/* receive response */
	for(uint8_t i = 0; i < 10; ++i)
	{
		response = sd_raw_rec_byte();
		if(response != 0xff)
			break;
	}
	response <<= 8;
	response |= sd_raw_rec_byte();

	return response;
}

uint8_t sd_raw_read_block(uint32_t block, uint8_t* buffer, int len){
	int r;
	uint16_t i;
	select_card();
	
	if((r = sd_raw_send_command_r1(CMD_READ_SINGLE_BLOCK, BLOCK_SIZE * block )))
	{
		unselect_card();
		_errorcode = ERROR_SD_READ;
		return 0;
	}
	/* wait for data block (start byte 0xfe) */
	while(sd_raw_rec_byte() != 0xfe);

	/* read byte block */
	for(i = 0; i < len; ++i)
		*buffer++ = sd_raw_rec_byte();
	for(; i < BLOCK_SIZE; ++i)
		sd_raw_rec_byte();

	/* read crc16 */
	sd_raw_rec_byte();
	sd_raw_rec_byte();

	/* deaddress card */
	unselect_card();

	/* let card some time to finish */
	sd_raw_rec_byte();
	return 1;
}

uint8_t sd_raw_write_block(uint32_t block, const uint8_t* buffer, int len)
{  
	int r;
	uint16_t i;
	/* address card */
	select_card();

	/* send single block request */
	if((r = sd_raw_send_command_r1(CMD_WRITE_SINGLE_BLOCK, BLOCK_SIZE * block)))
	{
		unselect_card();
		switch(r){
		case 0x01:
			_errorcode = ERROR_SD_WRITE_IDLE;
			break;
		case 0x02:
			_errorcode = ERROR_SD_WRITE_ERASE_RST;
			break;
		case 0x04:
			_errorcode = ERROR_SD_WRITE_ILLEGAL;
			break;
		case 0x08:
			_errorcode = ERROR_SD_WRITE_CRC;
			break;
		case 0x10:
			_errorcode = ERROR_SD_WRITE_ERASE_SEQ;
			break;
		case 0x20:
			_errorcode = ERROR_SD_WRITE_ADDRESS;
			break;
		case 0x40:
			_errorcode = ERROR_SD_WRITE_PARAMETER;
			break;
		default:
			_errorcode = ERROR_SD_WRITE;
			break;
		}
		
		return 0;
	}

	/* send start byte */
	sd_raw_send_byte(0xfe);

	/* write byte block */
	for(i = 0; i < len; ++i)
		sd_raw_send_byte(*buffer++);
	for(; i < BLOCK_SIZE; ++i)
		sd_raw_send_byte(0);

	/* write dummy crc16 */
	sd_raw_send_byte(0xff);
	sd_raw_send_byte(0xff);

	/* wait while card is busy */
	while(sd_raw_rec_byte() != 0xff);
	sd_raw_rec_byte();

	/* deaddress card */
	unselect_card();
	return 1;
}

/**
 * \ingroup sd_raw
 * Reads informational data from the card.
 *
 * This function reads and returns the card's registers
 * containing manufacturing and status information.
 *
 * \note: The information retrieved by this function is
 *		not required in any way to operate on the card,
 *		but it might be nice to display some of the data
 *		to the user.
 *
 * \param[in] info A pointer to the structure into which to save the information.
 * \returns 0 on failure, 1 on success.
 */
uint8_t sd_raw_get_info(struct sd_raw_info* info)
{
	memset(info, 0, sizeof(*info));

	select_card();

	/* read cid register */
	if(sd_raw_send_command_r1(CMD_SEND_CID, 0))
	{
		unselect_card();
		return 0;
	}
	while(sd_raw_rec_byte() != 0xfe);
	for(uint8_t i = 0; i < 18; ++i)
	{
		uint8_t b = sd_raw_rec_byte();

		switch(i)
		{
			case 0:
				info->manufacturer = b;
				break;
			case 1:
			case 2:
				info->oem[i - 1] = b;
				break;
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				info->product[i - 3] = b;
				break;
			case 8:
				info->revision = b;
				break;
			case 9:
			case 10:
			case 11:
			case 12:
				info->serial |= (uint32_t) b << ((12 - i) * 8);
				break;
			case 13:
				info->manufacturing_year = b << 4;
				break;
			case 14:
				info->manufacturing_year |= b >> 4;
				info->manufacturing_month = b & 0x0f;
				break;
		}
	}

	/* read csd register */
	uint8_t csd_read_bl_len = 0;
	uint8_t csd_c_size_mult = 0;
	uint16_t csd_c_size = 0;
	if(sd_raw_send_command_r1(CMD_SEND_CSD, 0))
	{
		unselect_card();
		return 0;
	}
	while(sd_raw_rec_byte() != 0xfe);
	for(uint8_t i = 0; i < 18; ++i)
	{
		uint8_t b = sd_raw_rec_byte();

		switch(i)
		{
			case 5:
				csd_read_bl_len = b & 0x0f;
				break;
			case 6:
				csd_c_size = (uint16_t) (b & 0x03) << 8;
				break;
			case 7:
				csd_c_size |= b;
				csd_c_size <<= 2;
				break;
			case 8:
				csd_c_size |= b >> 6;
				++csd_c_size;
				break;
			case 9:
				csd_c_size_mult = (b & 0x03) << 1;
				break;
			case 10:
				csd_c_size_mult |= b >> 7;

				info->capacity = (uint32_t) csd_c_size << (csd_c_size_mult + csd_read_bl_len + 2);

				break;
			case 14:
				if(b & 0x40)
					info->flag_copy = 1;
				if(b & 0x20)
					info->flag_write_protect = 1;
				if(b & 0x10)
					info->flag_write_protect_temp = 1;
				info->format = (b & 0x0c) >> 2;
				break;
		}
	}

	unselect_card();

	return 1;
}

void SPI_MasterInit(void)
{
	/* Set MOSI, SCK, SS output, all others input */
	PORTB = (1<<PB2) | (1<<PB1) | (1<<PB0);
	/* Enable SPI, Master, set clock rate fck/128 */
	SPCR = (1<<SPE) | (1<<MSTR)| (1<<SPR1) | (1<<SPR0);
	SPSR &= ~(1 << SPI2X); //no double clock frequency
}