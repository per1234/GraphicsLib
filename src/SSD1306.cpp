#include "SSD1306.h"
#include <Wire.h>


#define CHARGEPUMP 0x8D
#define COLUMNADDR 0x21
#define COMSCANDEC 0xC8
#define COMSCANINC 0xC0
#define DISPLAYALLON 0xA5
#define DISPLAYALLON_RESUME 0xA4
#define DISPLAYOFF 0xAE
#define DISPLAYON 0xAF
#define EXTERNALVCC 0x1
#define INVERTDISPLAY 0xA7
#define MEMORYMODE 0x20
#define NORMALDISPLAY 0xA6
#define PAGEADDR 0x22
#define SEGREMAP 0xA0
#define SETCOMPINS 0xDA
#define SETCONTRAST 0x81
#define SETDISPLAYCLOCKDIV 0xD5
#define SETDISPLAYOFFSET 0xD3
#define SETHIGHCOLUMN 0x10
#define SETLOWCOLUMN 0x00
#define SETMULTIPLEX 0xA8
#define SETPRECHARGE 0xD9
#define SETSEGMENTREMAP 0xA1
#define SETSTARTLINE 0x40
#define SETVCOMDETECT 0xDB
#define SWITCHCAPVCC 0x2

SSD1306::SSD1306(uint8_t address, uint8_t sda, uint8_t scl)
{
	this->address = address;
	this->sda = sda;
	this->scl = scl;
}


SSD1306::~SSD1306()
{
}

const int SSD1306::getWidth() const
{
	return 64;
}

const int SSD1306::getHeight() const
{
	return 48;
}

void SSD1306::begin()
{
	Wire.begin(this->sda, this->scl);
	// Let's use ~700khz if ESP8266 is in 160Mhz mode
	// this will be limited to ~400khz if the ESP8266 in 80Mhz mode.
	Wire.setClock(700000);
	buffer = new uint8_t[getWidth() * getHeight() / 8];

	for (int i = 0; i < getWidth() * getHeight() / 8; i++)
		buffer[i] = 0;

	sendInitCommands();
}

void SSD1306::setPixel(int x, int y, uint8_t color)
{
	if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight())
		return;
	if (color == White)
		buffer[x + (y / 8) * getWidth()] |= (1 << (y & 7));
	else
		buffer[x + (y / 8) * getWidth()] &= ~(1 << (y & 7));
}

void SSD1306::display()
{
	sendCommand(COLUMNADDR);
	sendCommand(0x20);
	sendCommand(0x5F);

	sendCommand(PAGEADDR);
	sendCommand(0x0);
	sendCommand(0x6);

	for (uint16_t i = 0; i < getWidth()*getHeight()/8; i++) {
		Wire.beginTransmission(this->address);
		Wire.write(0x40);
		for (uint8_t x = 0; x < 16; x++) {
			Wire.write(buffer[i]);
			i++;
		}
		i--;
		Wire.endTransmission();
	}
}


void SSD1306::sendCommand(uint8_t command) {
	Wire.beginTransmission(address);
	Wire.write(0x80);
	Wire.write(command);
	Wire.endTransmission();
}







void SSD1306::sendInitCommands(void) {
	sendCommand(DISPLAYOFF);
	sendCommand(SETDISPLAYCLOCKDIV);
	sendCommand(0xF0); // Increase speed of the display max ~96Hz
	sendCommand(SETMULTIPLEX);
	sendCommand(0x3F);
	sendCommand(SETDISPLAYOFFSET);
	sendCommand(0x00);
	sendCommand(SETSTARTLINE);
	sendCommand(CHARGEPUMP);
	sendCommand(0x14);
	sendCommand(MEMORYMODE);
	sendCommand(0x00);
	sendCommand(SEGREMAP);
	sendCommand(COMSCANINC);
	sendCommand(SETCOMPINS);
	sendCommand(0x12);
	sendCommand(SETCONTRAST);
	sendCommand(0xCF);
	sendCommand(SETPRECHARGE);
	sendCommand(0xF1);
	sendCommand(DISPLAYALLON_RESUME);
	sendCommand(NORMALDISPLAY);
	sendCommand(0x2e);            // stop scroll
	sendCommand(DISPLAYON);
}