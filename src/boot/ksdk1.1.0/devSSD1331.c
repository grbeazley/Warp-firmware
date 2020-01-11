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
	OSA_TimeDelay(0.1);
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
	OSA_TimeDelay(0.1);
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
writeLine(uint8_t commandByte[8], int transfer_size)
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
	OSA_TimeDelay(0.1);
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



	/*
	 *	Any post-initialization drawing commands go here.
	 */
	//...

	//writeCommand(kSSD1331CommandDISPLAYALLON);
	//writeCommand(kSSD1331CommandDISPLAYON);

	// Draw Rectangle

	/*
	writeCommand(0x22); // Enters draw rectangle mode
	writeCommand(0x03); // Sets starting column as column 1
	writeCommand(0x02); // Sets starting row as rows 1
	writeCommand(0x5F); // Sets the end Column
	writeCommand(0x3F); // Sets the end row

	writeCommand(0x00); // sets outline colours
	writeCommand(0x3F); 
	writeCommand(0x00);

	writeCommand(0x00); // sets fill colours
	writeCommand(0x3F);
	writeCommand(0x00);
	*/


	writeCommand(0x22); // Enters draw rectangle mode
	writeCommand(0x00); // Sets starting column as column 1
	writeCommand(0x00); // Sets starting row as rows 1
	writeCommand(0x5F); // Sets the end Column
	writeCommand(0x3F); // Sets the end row

	writeCommand(0x00); // sets outline colours
	writeCommand(0x3F); 
	writeCommand(0x00);

	writeCommand(0x00); // sets fill colours
	writeCommand(0x3F);
	writeCommand(0x00);

	

	return 0;
}

uint8_t numberColorR = 0x00;
uint8_t numberColorG = 0x3F;
uint8_t numberColorB = 0x00;

uint8_t numberColor[3] = {0x00, 0x3F, 0x00};

uint8_t backgrColorR = 0x00;
uint8_t backgrColorG = 0x00;
uint8_t backgrColorB = 0x00;

uint8_t backgrColor[3] = {0x00, 0x00, 0x00};



void drawRect(uint8_t positionOffset)
{
	writeCommand(0x22); // Draw rectangle mode
	writeCommand(0x03+positionOffset); // set column
	writeCommand(0x07); // set first row
	writeCommand(0x18+positionOffset); // finish column
	writeCommand(0x38); // finish row
	
	writeColor(numberColor, 3); // set outline of number 		
	//writeCommand(numberColorR); // set outline of number red
	//writeCommand(numberColorG); // set outline of number green
	//writeCommand(numberColorB); // set outline of number blue
	
	writeColor(backgrColor, 3); // set fill of number same as background
	
}
	
void 
drawSide(uint8_t positionOffset)
{
	writeCommand(0x21); // Enter draw mode	
	writeCommand(0x03+positionOffset); // Start at column 3 + positionOffset
	writeCommand(0x07); // Start at row 7
	writeCommand(0x03+positionOffset); // End at same column 3 + positionOffset
	writeCommand(0x38); // Finish at row 56
	
	writeColor(numberColor, 3); // set outline of number 		
	//writeCommand(numberColorR); // set outline of number red
	//writeCommand(numberColorG); // set outline of number green
	//writeCommand(numberColorB); // set outline of number blue
}

void
drawBar(uint8_t positionOffset)
{
	writeCommand(0x21); // Enter draw mode	
	writeCommand(0x03+positionOffset); // Start at column 3 + positionOffset
	writeCommand(0x1F); // Start at row 0x1F
	writeCommand(0x18+positionOffset); // End at same column 24 + positionOffset
	writeCommand(0x1F); // Finish at row 31
	
	writeColor(numberColor, 3); // set outline of number 		
	//writeCommand(numberColorR); // set outline of number red
	//writeCommand(numberColorG); // set outline of number green
	//writeCommand(numberColorB); // set outline of number blue
}

void
drawShortSideLower(uint8_t positionOffset)
{
	writeCommand(0x21); // Enter draw mode	
	writeCommand(0x03+positionOffset); // Start at column 3 + positionOffset
	writeCommand(0x1F); // Start at row 31
	writeCommand(0x03+positionOffset); // End at same column 24 + positionOffset
	writeCommand(0x38); // Finish at row 56
	
	writeColor(numberColor, 3); // set outline of number 	
	//writeCommand(numberColorR); // set outline of number red
	//writeCommand(numberColorG); // set outline of number green
	//writeCommand(numberColorB); // set outline of number blue
}

void
drawShortSideUpper(uint8_t positionOffset)
{
	writeCommand(0x21); // Enter draw mode	
	writeCommand(0x03+positionOffset); // Start at column 3 + positionOffset
	writeCommand(0x07); // Start at row 7
	writeCommand(0x03+positionOffset); // End at same column 3 + positionOffset
	writeCommand(0x1F); // Finish at row 31
	
	writeColor(numberColor, 3); // set outline of number 	
	//writeCommand(numberColorR); // set outline of number red
	//writeCommand(numberColorG); // set outline of number green
	//writeCommand(numberColorB); // set outline of number blue
}

void
drawBottom(uint8_t positionOffset)
{
	writeCommand(0x21); // Enter draw mode	
	writeCommand(0x03+positionOffset); // Start at column 3 + positionOffset
	writeCommand(0x38); // Start at row 31
	writeCommand(0x18+positionOffset); // End at column 24 + positionOffset
	writeCommand(0x38); // Finish at row 31
	
	writeColor(numberColor, 3); // set outline of number 
	//writeCommand(numberColorR); // set outline of number red
	//writeCommand(numberColorG); // set outline of number green
	//writeCommand(numberColorB); // set outline of number blue
}

void
drawTop(uint8_t positionOffset)
{
	writeCommand(0x21); // Enter draw mode	
	writeCommand(0x03+positionOffset); // Start at column 3 + positionOffset
	writeCommand(0x07); // Start at row 7
	writeCommand(0x18+positionOffset); // End at column 24 + positionOffset
	writeCommand(0x07); // Finish at row 7
	
	writeColor(numberColor, 3); // set outline of number 		
	//writeCommand(numberColorR); // set outline of number red
	//writeCommand(numberColorG); // set outline of number green
	//writeCommand(numberColorB); // set outline of number blue
}


// Function draws each number in turn
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
			drawSide(positionOffset);
			drawBar(positionOffset);
			drawTop(positionOffset);
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
	}
	// If it is in kilowatts get the first three significant figures
	if (isKiloWatts && isValid)
	{
		power_val1 = power / 1000;
	        power_val2 = (power % 1000) / 100;
	        power_val3 = (power % 100) / 10;
	    
	    
	}
	// Otherwise, if its valid, get all digits
	else if (isValid)
	{        
		power_val1 = power / 100;
		power_val2 = (power % 100) / 10;
		power_val3 = power % 10;
		
	}
	// Or just keep last value and don't print new numbers
	else{return;}

	
	// Clear Screen and draw Watts, W letter
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);


	
	if (isKiloWatts)
	{
		// Draw K for kilo 



		// Decimal Point (with a rectangle)
		writeCommand(0x22); // Draw rectangle mode
		writeCommand(0x1A); // set column
		writeCommand(0x35); // set first row
		writeCommand(0x1B); // finish column
		writeCommand(0x36); // finish row
		
		writeColor(numberColor, 3); // set outline of number 
		//writeCommand(numberColorR); // set outline of number R
		//writeCommand(numberColorG); // set outline of number G
		//writeCommand(numberColorB); // set outline of number B
		
		writeColor(backgrColor, 3); // set fill of number same as background 		
		//writeCommand(backgrColorR); // set fill of number same as background R
		//writeCommand(backgrColorG); // set fill of number same as background G
		//writeCommand(backgrColorB); // set fill of number same as background B

	}

	drawNumber(power_val1, 0);
	drawNumber(power_val2, 0x1A);
	drawNumber(power_val3, 0x34);


	
}
