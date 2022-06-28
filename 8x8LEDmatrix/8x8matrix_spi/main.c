/*
 * Ver 2 - test run on 8x8 matrix - working 21/6/22 at 4MHz SPI on baremetal ATMEGA328P
 * Ver 1 - SD_basics_part_1_SPI.c
 *
 * Part 1 - This code is the first layer - hardware - to talk to a SD card. 
 * For proof of SPI function I created Ver 2 
 * 
 * This code is not elegant, and not compact.  Its built for ease of understanding. 
 *
 * For the purposes of this testing Im using an ARMEGA328P baremetal, programmed with USBASP 
 * along with a typical MAX7219 driving a 1088AS 8x8 LED matrix - example
 * https://www.jaycar.com.au/arduino-compatible-8-x-8-led-dot-matrix-module/p/XC4499 or
 * https://www.altronics.com.au/p/z0975-8-x-8-red-led-matrix-display/
 * the 2 displays are identical except the rows and columns are transposed
 *
 * Created: 12/06/2022 10:57:07 AM
 * Author : Happymacer
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CS   PORTB2
#define MOSI PORTB3
#define MISO PORTB4
#define SCK  PORTB5
#define DDR_SPI DDRB
#define CS_ENABLE()     PORTB &= ~(1<<CS)// make CS active - ie low
#define CS_DISABLE()    PORTB |= (1<<CS) // make CS inactive - ie high



ISR(SPI_STC_vect)
{
	// this interrupt routine is triggered at the end of a MISO transfer.  It should transfer the 
	// data in SPDR register to the input data variable.  If used, then turn on SPIE of SPCR register
	
	// not used in this case 
	
	reti();
}

uint8_t SPIexchange (uint8_t data)
	{
		// This subroutine does not trigger the data transfer - this must be done by the calling routine 
		// by taking the CS line low to transfer and high to disable the slave
		
		// To receive data from the slave, the master must send data too as that generates the clock
		
		// SPDR = SPI data register - is the data to be sent or received
		// put data into the register
		SPDR = data;
		// now poll for the receive flag being set and move on when set	
		// SPSR = SPI status register - default is 0b00000000 so when SPIF gets set the polling ends
		while (!(SPSR & (1<<SPIF)));
		// return the result
		return SPDR;
	}


void init_SPI()
{
	sei();
	//SREG = (1<<GIE); // enable global interrupts
	DDR_SPI = (1<<CS)|(1<<MOSI)|(1<<SCK);
	// SPCR = SPI Control register - set up as follows: SPI enable, MSB first, master mode, fosc/2 = 8000000/2 = 4MHz
	SPCR = (1<<SPE)|(1<<MSTR); 
	SPSR = (1<<SPI2X); // for fosc/2 freq SPI2x must be logic 1
}

int main(void)
{
    init_SPI();
    // the 8x8 matrix ROWS are connected to the digit outputs (ie each column represents 1 digit)
    // the 8x8 matrix columns are connected to the segment outputs
    // data is latched on the rising edge of /CS
    // MSB is sent first, D15-D12 dont care, D11-D8 digit address
    // LSB is segment data
    // data is sent in 2 batches of 8 bits
    
    uint8_t TX_data; // data to send

	// when the MAX7219 chip powers up it does not set the data registers to 0.
	// clear the screen
	for (int j = 1; j<9; j++)
	{
		CS_ENABLE();
		TX_data = j; 
		SPIexchange (TX_data);
		TX_data = 0b00000000; 
		SPIexchange (TX_data);
		CS_DISABLE();
	}
    
	// set the MAX7219 led intensity
	CS_ENABLE();
	TX_data = 0b00001010; // select intensity register
	SPIexchange (TX_data);
	TX_data = 0b00000000; // minimum on
	SPIexchange (TX_data);
	CS_DISABLE();	
	
    // set MAX7219 chip to no decode option
    CS_ENABLE();	
    TX_data = 0b00001001; // select decode mode register
    SPIexchange (TX_data);
    TX_data = 0b00000000; // set it to all 0 - no decode
    SPIexchange (TX_data);	
    CS_DISABLE();

    // set MAX7219 chip scan limit
    CS_ENABLE();	
    TX_data = 0b00001011; // select decode mode register
    SPIexchange (TX_data);
    TX_data = 0b00000111; // all digits 
    SPIexchange (TX_data);
    CS_DISABLE();
	
    // then setup MAX7219 chip to end Shutdown mode
    CS_ENABLE();	
    TX_data = 0b00001100; // select decode mode register
    SPIexchange (TX_data);
    TX_data = 0b00000001;
    SPIexchange (TX_data);	
    CS_DISABLE();
    
	// set the screen data
	for (int j = 1; j<9; j++)
	{
		for (int i = 0; i<256; ++i)
		{
			CS_ENABLE();	
			TX_data = j; // use digit J (COLUMN)
			SPIexchange (TX_data);
			TX_data = i; // binary count
			SPIexchange (TX_data);
			CS_DISABLE();
			_delay_ms(1);
		}
	}
	
    while (1) 
    {
		
    // Example code 1 : use Shutdown mode to flash the screen
    //CS_ENABLE();
    //TX_data = 0b00001100; // select shutdown register
    //SPIexchange (TX_data);
    //TX_data = 0b00000000;
    //SPIexchange (TX_data);
    //CS_DISABLE();			
	//_delay_ms(1000);
	//CS_ENABLE();
	//TX_data = 0b00001100; // select shutdown register
	//SPIexchange (TX_data);
	//TX_data = 0b00000001;
	//SPIexchange (TX_data);
	//CS_DISABLE();
	//_delay_ms(1000);
	
	// Example code 2: Clear the screen, then write to a specific LED
	for (int j = 1; j<9; j++)
	{
		CS_ENABLE();
		TX_data = j;
		SPIexchange (TX_data);
		TX_data = 0b00000000;
		SPIexchange (TX_data);
		CS_DISABLE();
	}
	_delay_ms(1000); //just wait so the clear is noticeable
	
	// Illuminate LED at 0 col across and 0 rows down (0,0) is top left
	CS_ENABLE();
	TX_data = 0b00000001; // use digit 0, thats register 1
	SPIexchange (TX_data);
	TX_data = 0b00000001; // use row 0
	SPIexchange (TX_data);
	CS_DISABLE();
	_delay_ms(1000);
	
	// Illuminate LED at 7 col across and 7 rows down (7,7) is bottom right
	CS_ENABLE();
	TX_data = 0b00001000; // use digit 7, thats register 8
	SPIexchange (TX_data);
	TX_data = 0b10000000; // use row 0
	SPIexchange (TX_data);
	CS_DISABLE();
	_delay_ms(1000);
			
	// Illuminate LED at 5 col across and 7 rows down - it flashes due to the while loop
	CS_ENABLE();
	TX_data = 0b00000110; // use digit 5 (column 6)
	SPIexchange (TX_data);
	TX_data = 0b10001101; // use row 7 - the bit order is bit 0 is row 0, bit 7 is row 7 
	SPIexchange (TX_data);
	CS_DISABLE();
	_delay_ms(1000);	
	
	// run down a column
	uint8_t Led[8]= {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000};
	for (int i=0; i<8; i++)
	{
		CS_ENABLE();
		TX_data = 0b00000110; // use digit 5 (column 0)
		SPIexchange (TX_data);
		TX_data = Led[i]; 
		SPIexchange (TX_data);
		CS_DISABLE();
		_delay_ms(1);		
	}
	
	// run across a row
	for (int i=1; i<9; i++)
	{
		//erase the last led
		CS_ENABLE();
		TX_data = i-1;
		SPIexchange (TX_data);
		TX_data = 0b00000000; // use row 3 - have to erase the last bit before writing the next
		SPIexchange (TX_data);
		CS_DISABLE();		
		
		CS_ENABLE();
		TX_data = i; 
		SPIexchange (TX_data);
		TX_data = 0b00000100; // use row 3 
		SPIexchange (TX_data);
		CS_DISABLE();
		_delay_ms(10);
	}	
	}
}

