#include "WaveShare42B.h"
#include <SPI.h>
#include <pgmspace.h>


WaveShare42B::WaveShare42B(uint8_t pinCS, uint8_t pinRST, uint8_t pinDC, uint8_t pinBUSY)
{
	this->pinCS = pinCS;
	this->pinRST = pinRST;
	this->pinDC = pinDC;
	this->pinBUSY = pinBUSY;
}

WaveShare42B::~WaveShare42B()
{
}

const int WaveShare42B::getWidth() const	{	return 400;	}
const int WaveShare42B::getHeight() const	{	return 300;	}

void WaveShare42B::begin()
{
	//initialize pins
	pinMode(pinCS, OUTPUT);
	pinMode(pinRST, OUTPUT);
	pinMode(pinDC, OUTPUT);
	pinMode(pinBUSY, INPUT);
	SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
	SPI.begin();
	//initialize display
	reset();
	sendCommand(Command::BOOSTER_SOFT_START);
	sendData(0x17);
	sendData(0x17);
	sendData(0x17);     //07 0f 17 1f 27 2F 37 2f
	sendCommand(Command::POWER_ON);
	waitUntilIdle();
	sendCommand(Command::PANEL_SETTING);
	sendData(0x0F);     // LUT from OTP

	//this will eat up 30kb of memory!!!
	bufferBlack = new uint8_t[getWidth() * getHeight() / 8];
	bufferRed = new uint8_t[getWidth() * getHeight() / 8];

	int limit = getWidth() * getHeight() / 8;
	for (int i = 0; i < limit; i++)
	{
		bufferBlack[i] = 0xff;
		bufferRed[i] = 0xff;
	}
}

void WaveShare42B::reset()
{
	digitalWrite(pinRST, LOW);
	delay(200);
	digitalWrite(pinRST, HIGH);
	delay(200);
}

void WaveShare42B::spiTransfer(uint8_t data)
{
	digitalWrite(pinCS, LOW);
	SPI.transfer(data);
	digitalWrite(pinCS, HIGH);
}

uint8_t pinStatus = LOW;
void WaveShare42B::sendCommand(Command command)
{
	if(pinStatus != LOW)
		digitalWrite(pinDC, pinStatus = LOW);
	spiTransfer((uint8_t)command);
}

void WaveShare42B::sendData(uint8_t data)
{
	if(pinStatus != HIGH)
		digitalWrite(pinDC, pinStatus = HIGH);
	spiTransfer(data);
}

void WaveShare42B::waitUntilIdle()
{
	while (digitalRead(pinBUSY) == 0) {      //0: busy, 1: idle
		delay(10); //original: 100
	}
}



void WaveShare42B::setPartialWindow(const uint8_t* buffer_black, const uint8_t* buffer_red, int x, int y, int w, int l) {
	sendCommand(Command::PARTIAL_IN);
	sendCommand(Command::PARTIAL_WINDOW);
	sendData(x >> 8);
	sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
	sendData(((x & 0x1f8) + w - 1) >> 8);
	sendData(((x & 0x1f8) + w - 1) | 0x07);
	sendData(y >> 8);
	sendData(y & 0xff);
	sendData((y + l - 1) >> 8);
	sendData((y + l - 1) & 0xff);
	sendData(0x01);         // Gates scan both inside and outside of the partial window. (default) 
	delay(2);
	sendCommand(Command::DATA_START_TRANSMISSION_1);
	if (buffer_black != NULL) {
		for (int i = 0; i < w / 8 * l; i++) {
			sendData(buffer_black[i]);
		}
	}
	delay(2);
	sendCommand(Command::DATA_START_TRANSMISSION_2);
	if (buffer_red != NULL) {
		for (int i = 0; i < w / 8 * l; i++) {
			sendData(buffer_red[i]);
		}
	}
	delay(2);
	sendCommand(Command::PARTIAL_OUT);
}

/**
*  @brief: transmit partial data to the black part of SRAM
*/
void WaveShare42B::setPartialWindowBlack(const uint8_t* buffer_black, int x, int y, int w, int l) {
	sendCommand(Command::PARTIAL_IN);
	sendCommand(Command::PARTIAL_WINDOW);
	sendData(x >> 8);
	sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
	sendData(((x & 0x1f8) + w - 1) >> 8);
	sendData(((x & 0x1f8) + w - 1) | 0x07);
	sendData(y >> 8);
	sendData(y & 0xff);
	sendData((y + l - 1) >> 8);
	sendData((y + l - 1) & 0xff);
	sendData(0x01);         // Gates scan both inside and outside of the partial window. (default) 
	delay(2);
	sendCommand(Command::DATA_START_TRANSMISSION_1);
	if (buffer_black != NULL) {
		for (int i = 0; i < w / 8 * l; i++) {
			sendData(buffer_black[i]);
		}
	}
	delay(2);
	sendCommand(Command::PARTIAL_OUT);
}

/**
*  @brief: transmit partial data to the red part of SRAM
*/
void WaveShare42B::setPartialWindowRed(const uint8_t* buffer_red, int x, int y, int w, int l) {
	sendCommand(Command::PARTIAL_IN);
	sendCommand(Command::PARTIAL_WINDOW);
	sendData(x >> 8);
	Serial.print(x >> 8, DEC);
	sendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
	sendData(((x & 0x1f8) + w - 1) >> 8);
	sendData(((x & 0x1f8) + w - 1) | 0x07);
	sendData(y >> 8);
	sendData(y & 0xff);
	sendData((y + l - 1) >> 8);
	sendData((y + l - 1) & 0xff);
	sendData(0x01);         // Gates scan both inside and outside of the partial window. (default) 
	delay(2);
	sendCommand(Command::DATA_START_TRANSMISSION_2);
	if (buffer_red != NULL) {
		for (int i = 0; i < w / 8 * l; i++) {
			sendData(buffer_red[i]);
		}
	}
	delay(2);
	sendCommand(Command::PARTIAL_OUT);
}

/**
* @brief: refresh and displays the frame
*/
void WaveShare42B::displayFrame(const uint8_t* frame_black, const uint8_t* frame_red) {
	if (frame_black != NULL) {
		sendCommand(Command::DATA_START_TRANSMISSION_1);
		delay(2);
		sendFrame(frame_black);
		delay(2);
	}
	if (frame_red != NULL) {
		sendCommand(Command::DATA_START_TRANSMISSION_2);
		delay(2);
		int limit = getWidth() / 8 * getHeight();
		sendFrame(frame_red);
		delay(2);
	}
	sendCommand(Command::DISPLAY_REFRESH);
	waitUntilIdle();
}

void WaveShare42B::sendFrame(const uint8_t* frame)
{
	int limit = getWidth() / 8 * getHeight();
	for (int i = 0; i < limit; i++) {
		uint8_t a = frame[i]; //wtfap?
		sendData(a);
	}
}

/**
* @brief: clear the frame data from the SRAM, this won't refresh the display
*/
void WaveShare42B::clearFrame(void) {
	sendCommand(Command::DATA_START_TRANSMISSION_1);
	delay(2);
	int limit = getWidth() / 8 * getHeight();
	for (int i = 0; i < limit; i++) {
		sendData(0xFF);
	}
	delay(2);
	sendCommand(Command::DATA_START_TRANSMISSION_2);
	delay(2);
	for (int i = 0; i < limit; i++) {
		sendData(0xFF);
	}
	delay(2);
}

/**
* @brief: This displays the frame data from SRAM
*/
void WaveShare42B::displayFrame(void) {
	sendCommand(Command::DISPLAY_REFRESH);
	delay(100);
	waitUntilIdle();
}

void WaveShare42B::setPixel(int x, int y, uint8_t color)
{
	if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight())
	{
		Serial.println("Error in screen bounds");
		return;
	}
	if (color == White)
	{
		bufferBlack[(x + y * this->getWidth()) / 8] |= 0x80 >> (x % 8);
		bufferRed[(x + y * this->getWidth()) / 8] |= 0x80 >> (x % 8);
	}
	else if (color == Black)
	{
		bufferBlack[(x + y * this->getWidth()) / 8] &= ~(0x80 >> (x % 8));
		bufferRed[(x + y * this->getWidth()) / 8] |= 0x80 >> (x % 8);
	}
	else if (color == Red)
	{
		bufferBlack[(x + y * this->getWidth()) / 8] |= 0x80 >> (x % 8);
		bufferRed[(x + y * this->getWidth()) / 8] &= ~(0x80 >> (x % 8));
	}
}

void WaveShare42B::display()
{
	displayFrame(bufferBlack, bufferRed);
}
