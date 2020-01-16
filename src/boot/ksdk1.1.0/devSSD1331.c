#include <stdint.h>

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devSSD1331.h"



/*
 *	Override Warp firmware's use of these pins and define new aliases.
 */
enum
{
	kSSD1331PinMOSI		= GPIO_MAKE_PIN(HW_GPIOA, 8),
	kSSD1331PinSCK		= GPIO_MAKE_PIN(HW_GPIOA, 9),
	kSSD1331PinCSn		= GPIO_MAKE_PIN(HW_GPIOB, 13),
	kSSD1331PinDC		= GPIO_MAKE_PIN(HW_GPIOA, 12),
	kSSD1331PinRST		= GPIO_MAKE_PIN(HW_GPIOB, 0),
};

static int
writeCommand(uint8_t commandByte)
{
	spi_status_t status;


	volatile uint8_t	inBuffer[1];
	volatile uint8_t	payloadBytes[1];

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(0.1);//reduce to 0.1ms from 10ms for speed
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);
	
	payloadBytes[1] = commandByte;
	
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes[1],
					(uint8_t * restrict)&inBuffer[1],
					1		/* transfer size */,
					1		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}

static int
writeColor(uint8_t commandByte[3], int transfer_size)
{
	spi_status_t status;


	volatile uint8_t	inBuffer[transfer_size];
	volatile uint8_t	payloadBytes[transfer_size];

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(0.1);//reduce to 0.1ms from 10ms for speed
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);
	
	for (int i = 0; i < transfer_size; i++)
	{ 
		payloadBytes[i] = commandByte[i];


	}
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes,
					(uint8_t * restrict)&inBuffer,
					transfer_size	/* transfer size */,
					1		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}

static int
writeLine(uint8_t commandByte[8])
{
	spi_status_t status;

	int 				transfer_size = 8;
	volatile uint8_t	inBuffer[transfer_size];
	volatile uint8_t	payloadBytes[transfer_size];
	

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(0.1);//reduce to 0.1ms from 10ms for speed
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);
	
	for (int i = 0; i < transfer_size; i++)
	{ 
		payloadBytes[i] = commandByte[i];


	}
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes,
					(uint8_t * restrict)&inBuffer,
					transfer_size	/* transfer size */,
					1		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}


// Set initial number colour to be cyan
uint8_t numberColorR = 0x00;
uint8_t numberColorG = 0x35;
uint8_t numberColorB = 0x36;

//uint8_t numberColor[3] = {0x00, 0x00, 0x00};

// Set background colour as black
uint8_t backgrColorR = 0x00;
uint8_t backgrColorG = 0x00;
uint8_t backgrColorB = 0x00;

uint8_t backgrColor[3] = {0x00, 0x00, 0x00};


static int
writeWatts()
{
	// Write the W denoting the measurement of Watts
	spi_status_t status;

	int 				transfer_size = 32;
	volatile uint8_t	inBuffer[transfer_size];
	
	

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(0.1); //reduce to 0.1ms from 10ms for speed
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);

	volatile uint8_t payloadBytes[32] = {0x21, 0x51, 0x07, 0x51, 0x38, numberColorR, numberColorG, numberColorB,  /* Draw left hand line */
					     0x21, 0x51, 0x38, 0x56, 0x2E, numberColorR, numberColorG, numberColorB,  /* Draw left upward diagonal */
					     0x21, 0x5C, 0x38, 0x57, 0x2E, numberColorR, numberColorG, numberColorB,  /* Draw right downward diagonal */
					     0x21, 0x5C, 0x07, 0x5C, 0x38, numberColorR, numberColorG, numberColorB}; /* Draw right hand line */


	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes,
					(uint8_t * restrict)&inBuffer,
					32	/* transfer size */,
					1000	/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}

static int
writeKilo()
{
	// Write a K to denote measuring kilo watts
	spi_status_t status;

	int 				transfer_size = 24;
	volatile uint8_t	inBuffer[transfer_size];	

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(0.1);//reduce to 0.1ms from 10ms for speed
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);

	volatile uint8_t payloadBytes[24] = {0x21, 0x54, 0x12, 0x54, 0x29, numberColorR, numberColorG, numberColorB,   /* Draw left hand line    */
					     0x21, 0x55, 0x1C, 0x59, 0x12, numberColorR, numberColorG, numberColorB,   /* Draw upward diagonal   */
					     0x21, 0x55, 0x1D, 0x59, 0x29, numberColorR, numberColorG, numberColorB};  /* Draw downward diagonal */


	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes,
					(uint8_t * restrict)&inBuffer,
					24	/* transfer size */,
					1000		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}


int
devSSD1331init(void)
{
	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Re-configure SPI to be on PTA8 and PTA9 for MOSI and SCK respectively.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 8u, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9u, kPortMuxAlt3);

	enableSPIpins();

	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Reconfigure to use as GPIO.
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 13u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 12u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 0u, kPortMuxAsGpio);


	/*
	 *	RST high->low->high.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_ClearPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);

	/*
	 *	Initialization sequence, borrowed from https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino
	 */
	writeCommand(kSSD1331CommandDISPLAYOFF);	// 0xAE
	writeCommand(kSSD1331CommandSETREMAP);		// 0xA0
	writeCommand(0x72);				// RGB Color
	writeCommand(kSSD1331CommandSTARTLINE);		// 0xA1
	writeCommand(0x0);
	writeCommand(kSSD1331CommandDISPLAYOFFSET);	// 0xA2
	writeCommand(0x0);
	writeCommand(kSSD1331CommandNORMALDISPLAY);	// 0xA4
	writeCommand(kSSD1331CommandSETMULTIPLEX);	// 0xA8
	writeCommand(0x3F);				// 0x3F 1/64 duty
	writeCommand(kSSD1331CommandSETMASTER);		// 0xAD
	writeCommand(0x8E);
	writeCommand(kSSD1331CommandPOWERMODE);		// 0xB0
	writeCommand(0x0B);
	writeCommand(kSSD1331CommandPRECHARGE);		// 0xB1
	writeCommand(0x31);
	writeCommand(kSSD1331CommandCLOCKDIV);		// 0xB3
	writeCommand(0xF0);				// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8A
	writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGEB);	// 0x8B
	writeCommand(0x78);
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8C
	writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGELEVEL);	// 0xBB
	writeCommand(0x3A);
	writeCommand(kSSD1331CommandVCOMH);		// 0xBE
	writeCommand(0x3E);
	writeCommand(kSSD1331CommandMASTERCURRENT);	// 0x87
	writeCommand(0x08);
	writeCommand(kSSD1331CommandCONTRASTA);		// 0x81
	writeCommand(0x91);
	writeCommand(kSSD1331CommandCONTRASTB);		// 0x82
	writeCommand(0xFF);
	writeCommand(kSSD1331CommandCONTRASTC);		// 0x83
	writeCommand(0x7D);
	writeCommand(kSSD1331CommandDISPLAYON);		// Turn on oled panel

	/*
	 *	To use fill commands, you will have to issue a command to the display to enable them. See the manual.
	 */
	writeCommand(kSSD1331CommandFILL);
	writeCommand(0x01);

	/*
	 *	Clear Screen
	 */
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);	

	return 0;
}




void drawRect(uint8_t positionOffset)
{
	// Using the line drawing tool to write the first part of the rectangle
	uint8_t line_buffer[8] = {0x22, 0x03+positionOffset, 0x07, 0x18+positionOffset, 0x38, numberColorR, numberColorG, numberColorB};
		
	writeLine(line_buffer);
	
	writeColor(backgrColor, 3); // set fill of number same as background	
}
	
void 
drawSide(uint8_t positionOffset)
{
	uint8_t line_buffer[8] = {0x21, 0x03+positionOffset, 0x07, 0x03+positionOffset, 0x38, numberColorR, numberColorG, numberColorB};
		
	writeLine(line_buffer);
}

void
drawBar(uint8_t positionOffset)
{
	uint8_t line_buffer[8] = {0x21, 0x03+positionOffset, 0x1F, 0x18+positionOffset, 0x1F, numberColorR, numberColorG, numberColorB};
		
	writeLine(line_buffer);
}

void
drawShortSideLower(uint8_t positionOffset)
{
	uint8_t line_buffer[8] = {0x21, 0x03+positionOffset, 0x1F, 0x03+positionOffset, 0x38, numberColorR, numberColorG, numberColorB};
		
	writeLine(line_buffer);
}

void
drawShortSideUpper(uint8_t positionOffset)
{
	uint8_t line_buffer[8] = {0x21, 0x03+positionOffset, 0x07, 0x03+positionOffset, 0x1F, numberColorR, numberColorG, numberColorB};
		
	writeLine(line_buffer);
}

void
drawBottom(uint8_t positionOffset)
{
	uint8_t line_buffer[8] = {0x21, 0x03+positionOffset, 0x38, 0x18+positionOffset, 0x38, numberColorR, numberColorG, numberColorB};
		
	writeLine(line_buffer);
}

void
drawTop(uint8_t positionOffset)
{
	uint8_t line_buffer[8] = {0x21, 0x03+positionOffset, 0x07, 0x18+positionOffset, 0x07, numberColorR, numberColorG, numberColorB};
		
	writeLine(line_buffer);
}


// Function draws each number at the required position offset (in the x direction)
void
drawNumber(int number, uint8_t positionOffset)
{

	switch(number)
	{
		case 0:
		
			// Draw 0
			drawRect(positionOffset);
			break;
			
		
		case 1:
		
			// Draw 1
			drawSide(positionOffset+0x15);
			break;
		
		case 2:
		
			// Draw 2
			drawBar(positionOffset);
			drawTop(positionOffset);
			drawBottom(positionOffset);
			drawShortSideLower(positionOffset);
			drawShortSideUpper(positionOffset+0x15);
			break;
		
		case 3:
		
			// Draw 3
			drawBar(positionOffset);
			drawTop(positionOffset);
			drawBottom(positionOffset);
			drawSide(positionOffset+0x15);
			break;
		
		case 4:
		
			// Draw 4
			drawSide(positionOffset+0x15);
			drawBar(positionOffset);
			drawShortSideUpper(positionOffset);
			break;
		
		case 5:
		
			// Draw 5
			drawBar(positionOffset);
			drawTop(positionOffset);
			drawBottom(positionOffset);
			drawShortSideLower(positionOffset+0x15);
			drawShortSideUpper(positionOffset);
			break;
		
		case 6:
		
			// Draw 6
			drawSide(positionOffset);
			drawBar(positionOffset);
			drawTop(positionOffset);
			drawBottom(positionOffset);
			drawShortSideLower(positionOffset+0x15);
			break;
		
		case 7:
		
			// Draw 7
			drawSide(positionOffset+0x15);
			drawTop(positionOffset);
			break;
		
		case 8:
		
			// Draw 8
			drawRect(positionOffset);
			drawBar(positionOffset);
			break;
		
		case 9:
		
			// Draw 9
			drawBar(positionOffset);
			drawTop(positionOffset);
			drawBottom(positionOffset);
			drawSide(positionOffset+0x15);
			drawShortSideUpper(positionOffset);
			break;
		
		default:
		
			// Should never get here
			break;
		
	}
	
}

void
drawNumbersPower(int power)
{

	
	int power_val1;
	int power_val2;
	int power_val3;
	bool isKiloWatts = 0;
	bool isValid = 0;

	// First decide if the power reading is kilo-watts or not
	if (power > 1000 && power < 10000)
	{
		isKiloWatts = 1;
	        isValid = 1;
	}
	// Then see if it is in the usable watts range otherwise it is invalid
	else if(power <= 1000 && power >= 0)
	{
		isValid = 1;
		
		// Power below 1000W, so use cyan colour
		numberColorR = 0x00;
		numberColorG = 0x35;
		numberColorB = 0x36;
	}
	
	// If it is in kilowatts get the first three significant figures
	if (isKiloWatts && isValid)
	{
		power_val1 = power / 1000;
	    	power_val2 = (power % 1000) / 100;
		    power_val3 = (power % 100) / 10;
		
		numberColorR = 0x39;
		numberColorG = 0x00;
		numberColorB = 0x00;
		
	}       
	// OtherbackgrColor[3] = {0x00, 0x00, 0x00};wise, if its valid, get all digits
	else if (isValid)
	{        
		power_val1 = power / 100;
		power_val2 = (power % 100) / 10;
		power_val3 = power % 10;
		
	}
	// Or just keep last value and don't print new numbers
	else{return;}

	
	// Clear Screen 
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);

	// Set Background color (commented to save time since background assumed black)
	//uint8_t line_buffer[8] = {0x22, 0x00, 0x00, 0x5F, 0x3F, backgrColorR, backgrColorG, backgrColorB};
		
	//writeLine(line_buffer);
	
	//writeColor(backgrColor, 3); // set background fill


	// Draw Watts, W letter
	writeWatts();
	
	if (isKiloWatts)
	{
		// Draw K for kilo 
		writeKilo();


		// Decimal Point (with a rectangle)		
		uint8_t line_buffer[8] = {0x22, 0x1A, 0x35, 0x1B, 0x36, numberColorR, numberColorG, numberColorB};
		
		writeLine(line_buffer);
	
		writeColor(backgrColor, 3); // set background fill

	}
	
	// Draw last number first as it is likely to change the most
	drawNumber(power_val3, 0x34);	
	drawNumber(power_val2, 0x1A);
	drawNumber(power_val1, 0);


	
}
