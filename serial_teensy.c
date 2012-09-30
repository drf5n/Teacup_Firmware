/** \file
        \brief teensy hack of Teacup's serial.c system to rely on onboard USB buffers per 
        http://pjrc.com/teensy/usb_serial.html and the usb_serial.c code from http://www.pjrc.com/teensy/usb_serial.zip

        XON/XOFF isn't used  Maybe it would fit in the serial_rxchars routine?

*/

#include        "serial.h"
#include        "usb_serial.h"
#include	"config.h"
#include	"arduino.h"


/// size of TX and RX buffers. MUST be a \f$2^n\f$ value
#define		BUFSIZE			64

/// ascii XOFF character
#define		ASCII_XOFF	19
/// ascii XON character
#define		ASCII_XON		17


#ifdef	XONXOFF
#define		FLOWFLAG_STATE_XOFF	0
#define		FLOWFLAG_SEND_XON		1
#define		FLOWFLAG_SEND_XOFF	2
#define		FLOWFLAG_STATE_XON	4
// initially, send an XON
volatile uint8_t flowflags = FLOWFLAG_SEND_XON;
#endif

/// initialise serial subsystem
///
/// set up baud generator and interrupts, clear buffers
void serial_init()
{
  usb_init();
}


/*
	Read
*/

/// check how many characters can be read
uint8_t serial_rxchars()
{
	return usb_serial_available();
}

/// read one character
uint8_t serial_popchar()
{
	return usb_serial_getchar();
}

/*
	Write
*/

/// send one character
void serial_writechar(uint8_t data)
{
  usb_serial_putchar(data);
}

/// send a whole block
void serial_writeblock(void *data, int datalen)
{
	int i;

	for (i = 0; i < datalen; i++)
		serial_writechar(((uint8_t *) data)[i]);
}

/// send a string- look for null byte instead of expecting a length
void serial_writestr(uint8_t *data)
{
	uint8_t i = 0, r;
	// yes, this is *supposed* to be assignment rather than comparison, so we break when r is assigned zero
	while ((r = data[i++]))
		serial_writechar(r);
}

/**
	Write block from FLASH

	Extensions to output flash memory pointers. This prevents the data to
	become part of the .data segment instead of the .code segment. That means
	less memory is consumed for multi-character writes.

	For single character writes (i.e. '\n' instead of "\n"), using
	serial_writechar() directly is the better choice.
*/
void serial_writeblock_P(PGM_P data, int datalen)
{
	int i;

	for (i = 0; i < datalen; i++)
		serial_writechar(pgm_read_byte(&data[i]));
}

/// Write string from FLASH
void serial_writestr_P(PGM_P data)
{
	uint8_t r, i = 0;
	// yes, this is *supposed* to be assignment rather than comparison, so we break when r is assigned zero
	while ((r = pgm_read_byte(&data[i++])))
		serial_writechar(r);
}
