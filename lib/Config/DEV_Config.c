/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-07-31
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the"Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED"AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DEV_Config.h"

#if USE_LGPIO_LIB
int GPIO_Handle;
int SPI_Handle;
#endif

/**
 * GPIO
 **/
int EPD_RST_PIN;
int EPD_DC_PIN;
int EPD_CS_PIN;
int EPD_BUSY_PIN;
int EPD_PWR_PIN;
int EPD_MOSI_PIN;
int EPD_SCLK_PIN;

/**
 * GPIO read and write
 **/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
#ifdef USE_BCM2835_LIB
	bcm2835_gpio_write(Pin, Value);
#elif USE_WIRINGPI_LIB
	digitalWrite(Pin, Value);
#elif USE_LGPIO_LIB
	lgGpioWrite(GPIO_Handle, Pin, Value);
#elif USE_DEV_LIB
	GPIOD_Write(Pin, Value);
#endif
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
	UBYTE Read_value = 0;

#ifdef USE_BCM2835_LIB
	Read_value = bcm2835_gpio_lev(Pin);
#elif USE_WIRINGPI_LIB
	Read_value = digitalRead(Pin);
#elif USE_LGPIO_LIB
	Read_value = lgGpioRead(GPIO_Handle, Pin);
#elif USE_DEV_LIB
	Read_value = GPIOD_Read(Pin);
#endif

	return Read_value;
}

/**
 * SPI
 **/
void DEV_SPI_WriteByte(uint8_t Value)
{
#ifdef USE_BCM2835_LIB
	bcm2835_spi_transfer(Value);
#elif USE_WIRINGPI_LIB
	wiringPiSPIDataRW(0, &Value, 1);
#elif USE_LGPIO_LIB
	lgSpiWrite(SPI_Handle, (char *)&Value, 1);
#elif USE_DEV_LIB
	DEV_HARDWARE_SPI_TransferByte(Value);
#endif
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
#ifdef USE_BCM2835_LIB
	char rData[Len];
	bcm2835_spi_transfernb((char *)pData, rData, Len);
#elif USE_WIRINGPI_LIB
	wiringPiSPIDataRW(0, pData, Len);
#elif USE_LGPIO_LIB
	lgSpiWrite(SPI_Handle, (char *)pData, Len);
#elif USE_DEV_LIB
	DEV_HARDWARE_SPI_Transfer(pData, Len);
#endif
}

/**
 * GPIO Mode
 **/
void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
#ifdef USE_BCM2835_LIB
	if (Mode == 0 || Mode == BCM2835_GPIO_FSEL_INPT)
	{
		bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_INPT);
	}
	else
	{
		bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_OUTP);
	}
#elif USE_WIRINGPI_LIB
	if (Mode == 0 || Mode == INPUT)
	{
		pinMode(Pin, INPUT);
		pullUpDnControl(Pin, PUD_UP);
	}
	else
	{
		pinMode(Pin, OUTPUT);
		// DEBUG (" %d OUT",Pin);
	}
#elif USE_LGPIO_LIB
	if (Mode == 0 || Mode == LG_SET_INPUT)
	{
		lgGpioClaimInput(GPIO_Handle, LFLAGS, Pin);
		// DEBUG("IN Pin = %d",Pin);
	}
	else
	{
		lgGpioClaimOutput(GPIO_Handle, LFLAGS, Pin, LG_LOW);
		// DEBUG("OUT Pin = %d",Pin);
	}
#elif USE_DEV_LIB
	if (Mode == 0 || Mode == GPIOD_IN)
	{
		GPIOD_Direction(Pin, GPIOD_IN);
		// DEBUG("IN Pin = %d",Pin);
	}
	else
	{
		GPIOD_Direction(Pin, GPIOD_OUT);
		// DEBUG("OUT Pin = %d",Pin);
	}
#endif
}

/**
 * delay x ms
 **/
void DEV_Delay_ms(UDOUBLE xms)
{
#ifdef USE_BCM2835_LIB
	bcm2835_delay(xms);
#elif USE_WIRINGPI_LIB
	delay(xms);
#elif USE_LGPIO_LIB
	lguSleep(xms / 1000.0);
#elif USE_DEV_LIB
	UDOUBLE i;
	for (i = 0; i < xms; i++)
	{
		usleep(1000);
	}
#endif
}

static int DEV_Equipment_Testing(void)
{
	FILE *fp;
	char issue_str[64];

	fp = fopen("/etc/issue", "r");
	if (fp == NULL)
	{
		DEBUG("Unable to open /etc/issue");
		return -1;
	}
	if (fread(issue_str, 1, sizeof(issue_str), fp) <= 0)
	{
		DEBUG("Unable to read from /etc/issue");
		return -1;
	}
	issue_str[sizeof(issue_str) - 1] = '\0';
	fclose(fp);

	DEBUG("Current environment:");

	char systems[][9] = {"Raspbian", "Debian", "NixOS"};
	int detected = 0;
	for (int i = 0; i < 3; i++)
	{
		if (strstr(issue_str, systems[i]) != NULL)
		{
			DEBUG("%s", systems[i]);
			detected = 1;
		}
	}
	if (!detected)
	{
		DEBUG("not recognized");
		DEBUG("Built for Raspberry Pi, but unable to detect environment.");
		DEBUG("Perhaps you meant to 'make JETSON' instead?");
		return -1;
	}

	return 0;
}

void DEV_GPIO_Init(void)
{
	EPD_RST_PIN = 17;
	EPD_DC_PIN = 25;
	EPD_CS_PIN = 8;
	EPD_PWR_PIN = 18;
	EPD_BUSY_PIN = 24;
	EPD_MOSI_PIN = 10;
	EPD_SCLK_PIN = 11;

	DEV_GPIO_Mode(EPD_BUSY_PIN, 0);
	DEV_GPIO_Mode(EPD_RST_PIN, 1);
	DEV_GPIO_Mode(EPD_DC_PIN, 1);
	DEV_GPIO_Mode(EPD_CS_PIN, 1);
	DEV_GPIO_Mode(EPD_PWR_PIN, 1);
	// DEV_GPIO_Mode(EPD_MOSI_PIN, 0);
	// DEV_GPIO_Mode(EPD_SCLK_PIN, 1);

	DEV_Digital_Write(EPD_CS_PIN, 1);
	DEV_Digital_Write(EPD_PWR_PIN, 1);
}

void DEV_SPI_SendnData(UBYTE *Reg)
{
	UDOUBLE size;
	size = sizeof(Reg);
	for (UDOUBLE i = 0; i < size; i++)
	{
		DEV_SPI_SendData(Reg[i]);
	}
}

void DEV_SPI_SendData(UBYTE Reg)
{
	UBYTE i, j = Reg;
	DEV_GPIO_Mode(EPD_MOSI_PIN, 1);
	DEV_Digital_Write(EPD_CS_PIN, 0);
	for (i = 0; i < 8; i++)
	{
		DEV_Digital_Write(EPD_SCLK_PIN, 0);
		if (j & 0x80)
		{
			DEV_Digital_Write(EPD_MOSI_PIN, 1);
		}
		else
		{
			DEV_Digital_Write(EPD_MOSI_PIN, 0);
		}

		DEV_Digital_Write(EPD_SCLK_PIN, 1);
		j = j << 1;
	}
	DEV_Digital_Write(EPD_SCLK_PIN, 0);
	DEV_Digital_Write(EPD_CS_PIN, 1);
}

UBYTE DEV_SPI_ReadData()
{
	UBYTE i, j = 0xff;
	DEV_GPIO_Mode(EPD_MOSI_PIN, 0);
	DEV_Digital_Write(EPD_CS_PIN, 0);
	for (i = 0; i < 8; i++)
	{
		DEV_Digital_Write(EPD_SCLK_PIN, 0);
		j = j << 1;
		if (DEV_Digital_Read(EPD_MOSI_PIN))
		{
			j = j | 0x01;
		}
		else
		{
			j = j & 0xfe;
		}
		DEV_Digital_Write(EPD_SCLK_PIN, 1);
	}
	DEV_Digital_Write(EPD_SCLK_PIN, 0);
	DEV_Digital_Write(EPD_CS_PIN, 1);
	return j;
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_Module_Init(void)
{
	if (DEV_Equipment_Testing() < 0)
	{
		return 1;
	}

#ifdef USE_BCM2835_LIB
	if (!bcm2835_init())
	{
		DEBUG("bcm2835 init failed  !!!");
		return 1;
	}
	else
	{
		DEBUG("bcm2835 init success !!!");
	}

	// GPIO Config
	DEV_GPIO_Init();

	bcm2835_spi_begin();										// Start spi interface, set spi pin for the reuse function
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);	// High first transmission
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);					// spi mode 0
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128); // Frequency
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);					// set CE0
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);	// enable cs0

#elif USE_WIRINGPI_LIB
	// if(wiringPiSetup() < 0)//use wiringpi Pin number table
	if (wiringPiSetupGpio() < 0)
	{ // use BCM2835 Pin number table
		DEBUG("set wiringPi lib failed	!!!");
		return 1;
	}
	else
	{
		DEBUG("set wiringPi lib success !!!");
	}

	// GPIO Config
	DEV_GPIO_Init();
	wiringPiSPISetup(0, 10000000);
	// wiringPiSPISetupMode(0, 32000000, 0);
#elif USE_LGPIO_LIB
	char buffer[NUM_MAXBUF];
	FILE *fp;
	fp = popen("cat /proc/cpuinfo | grep 'Raspberry Pi 5'", "r");
	if (fp == NULL)
	{
		DEBUG("It is not possible to determine the model of the Raspberry PI");
		return -1;
	}

	if (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		GPIO_Handle = lgGpiochipOpen(4);
		if (GPIO_Handle < 0)
		{
			DEBUG("gpiochip4 Export Failed");
			return -1;
		}
	}
	else
	{
		GPIO_Handle = lgGpiochipOpen(0);
		if (GPIO_Handle < 0)
		{
			DEBUG("gpiochip0 Export Failed");
			return -1;
		}
	}
	SPI_Handle = lgSpiOpen(0, 0, 10000000, 0);
	DEV_GPIO_Init();
#elif USE_DEV_LIB
	DEBUG("Write and read /dev/spidev0.0");
	GPIOD_Export();
	DEV_GPIO_Init();
	DEV_HARDWARE_SPI_begin("/dev/spidev0.0");
	DEV_HARDWARE_SPI_setSpeed(10000000);
#endif
	return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void)
{

#ifdef USE_BCM2835_LIB
	DEV_Digital_Write(EPD_CS_PIN, LOW);
	DEV_Digital_Write(EPD_PWR_PIN, LOW);
	DEV_Digital_Write(EPD_DC_PIN, LOW);
	DEV_Digital_Write(EPD_RST_PIN, LOW);

	bcm2835_spi_end();
	bcm2835_close();
#elif USE_WIRINGPI_LIB
	DEV_Digital_Write(EPD_CS_PIN, 0);
	DEV_Digital_Write(EPD_PWR_PIN, 0);
	DEV_Digital_Write(EPD_DC_PIN, 0);
	DEV_Digital_Write(EPD_RST_PIN, 0);
#elif USE_LGPIO_LIB
	// DEV_Digital_Write(EPD_CS_PIN, 0);
	// DEV_Digital_Write(EPD_PWR_PIN, 0);
	// DEV_Digital_Write(EPD_DC_PIN, 0);
	// DEV_Digital_Write(EPD_RST_PIN, 0);
	// lgSpiClose(SPI_Handle);
	// lgGpiochipClose(GPIO_Handle);
#elif USE_DEV_LIB
	DEV_HARDWARE_SPI_end();
	DEV_Digital_Write(EPD_CS_PIN, 0);
	DEV_Digital_Write(EPD_PWR_PIN, 0);
	DEV_Digital_Write(EPD_DC_PIN, 0);
	DEV_Digital_Write(EPD_RST_PIN, 0);
	GPIOD_Unexport(EPD_PWR_PIN);
	GPIOD_Unexport(EPD_DC_PIN);
	GPIOD_Unexport(EPD_RST_PIN);
	GPIOD_Unexport(EPD_BUSY_PIN);
	GPIOD_Unexport_GPIO();
#endif
}
