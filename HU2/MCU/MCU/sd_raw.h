/* SD_RAW.H
Header file for Sd_raw.c

The original file is Copyright (c) 2006-2012 by Roland Riegel <feedback@roland-riegel.de>, distributed under the GNU GPL version 2 or the GNU LGPL version 2.1
Modifications 2017 are by Richard Kokx, Formula Cruisers Team, The Hague University of Applied Sciences
The SPI_MasterInit() function is originally Copyright (c) 2008 Atmel Corporation, and has been copied from the AT90CAN128 datasheet (version 7679H-CAN-08/08)
*/

#ifndef SD_RAW_H
#define SD_RAW_H
#include <avr/io.h>

//#define USE_SD_CARD

//define the block size to read and write
#define BLOCK_SIZE 512

#define select_card() PORTB &= ~(1 << PB0)
#define unselect_card() PORTB |= (1 << PB0)

/**
 * This struct is used by sd_raw_get_info() to return
 * manufacturing and status information of the card.
 */
struct sd_raw_info
{
	/**
	 * A manufacturer code globally assigned by the SD card organization.
	 */
	uint8_t manufacturer;
	/**
	 * A string describing the card's OEM or content, globally assigned by the SD card organization.
	 */
	uint8_t oem[3];
	/**
	 * A product name.
	 */
	uint8_t product[6];
	/**
	 * The card's revision, coded in packed BCD.
	 *
	 * For example, the revision value \c 0x32 means "3.2".
	 */
	uint8_t revision;
	/**
	 * A serial number assigned by the manufacturer.
	 */
	uint32_t serial;
	/**
	 * The year of manufacturing.
	 *
	 * A value of zero means year 2000.
	 */
	uint8_t manufacturing_year;
	/**
	 * The month of manufacturing.
	 */
	uint8_t manufacturing_month;
	/**
	 * The card's total capacity in bytes.
	 */
	uint32_t capacity;
	/**
	 * Defines whether the card's content is original or copied.
	 *
	 * A value of \c 0 means original, \c 1 means copied.
	 */
	uint8_t flag_copy;
	/**
	 * Defines whether the card's content is write-protected.
	 *
	 * \note This is an internal flag and does not represent the
	 *	   state of the card's mechanical write-protect switch.
	 */
	uint8_t flag_write_protect;
	/**
	 * Defines whether the card's content is temporarily write-protected.
	 *
	 * \note This is an internal flag and does not represent the
	 *	   state of the card's mechanical write-protect switch.
	 */
	uint8_t flag_write_protect_temp;
	/**
	 * The card's data layout.
	 *
	 * See the \c SD_RAW_FORMAT_* constants for details.
	 *
	 * \note This value is not guaranteed to match reality.
	 */
	uint8_t format;
};

uint8_t sd_raw_init(void);
uint8_t sd_raw_get_info(struct sd_raw_info* info);
uint8_t sd_raw_read_block(uint32_t block, uint8_t* buffer,int len);
uint8_t sd_raw_write_block(uint32_t block, const uint8_t* buffer,int len);
void sd_raw_wait_ready(void);

void SPI_MasterInit(void);
uint8_t sd_prewrite_buffer(void);
uint8_t sd_flush_buffer(void);
uint8_t sd_write(char* buffer, int len);
uint8_t sd_check_and_flush(void);
uint8_t sd_write_nt_string(char* buffer);
uint8_t sd_log(char* pre, uint8_t* data, uint8_t dlen);
uint8_t sd_log_s(char* pre, char* data);

extern volatile uint32_t debugval;

#endif //SD_RAW_H