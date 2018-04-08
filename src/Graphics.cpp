#include "Graphics.h"
#include "DisplayDriver.h"

#include <Arduino.h>
#include <pgmspace.h>

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif


// font
#define JUMPTABLE_BYTES 4

#define JUMPTABLE_LSB   1
#define JUMPTABLE_SIZE  2
#define JUMPTABLE_WIDTH 3
#define JUMPTABLE_START 4

#define WIDTH_POS 0
#define HEIGHT_POS 1
#define FIRST_CHAR_POS 2
#define CHAR_NUM_POS 3

Graphics::Graphics(DisplayDriver* driver)
{
	this->driver = driver;
}


Graphics::~Graphics()
{
}

void Graphics::begin()
{
	driver->begin();
}

void Graphics::setPixel(int x, int y)
{
	setPixel(x, y, currentColor);
}
void Graphics::setPixel(int x, int y, int color)
{
	this->driver->setPixel(x+offsetX, y+offsetY, color);
}

void Graphics::display() 
{
	this->driver->display(); 
}

int Graphics::getWidth() const
{
	return driver->getWidth();
}

int Graphics::getHeight() const
{
	return driver->getHeight();
}

void Graphics::clear(int color)
{
	for (int y = 0; y < driver->getHeight(); y++)
		for (int x = 0; x < driver->getWidth(); x++)
			setPixel(x, y, color);
}

void Graphics::drawLine(int x0, int y0, int x1, int y1)
{
	drawLine(x0, y0, x1, y1, currentColor);
}

void Graphics::drawLine(int x0, int y0, int x1, int y1, int color)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx >> 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			setPixel(y0, x0, color);
		}
		else {
			setPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void Graphics::drawRect(int x1, int y1, int x2, int y2)
{
	drawRect(x1, y1, x2, y2, currentColor);
}

void Graphics::drawRect(int x1, int y1, int x2, int y2, int color)
{
	drawLine(x1, y1, x2, y1, color);
	drawLine(x1, y2, x2, y2, color);
	drawLine(x1, y1, x1, y2, color);
	drawLine(x2, y1, x2, y2, color);
}

void Graphics::fillRect(int x1, int y1, int x2, int y2)
{
	fillRect(x1, y1, x2, y2, currentColor);
}

void Graphics::drawPicture(int xMove, int yMove, const uint8_t * image)
{

}

void Graphics::setCursor(int x, int y)
{
	fontCursorX = x;
	fontCursorY = y;
}

void Graphics::setFont(const uint8_t * fontData)
{
	this->currentFont = fontData;
}

uint8_t Graphics::readFontData(const uint8_t * start, uint32_t offset) {
	return pgm_read_byte(start + offset);
}

size_t Graphics::write(uint8_t code)
{
	uint16_t firstChar = readFontData(currentFont, FIRST_CHAR_POS);
	uint16_t lineHeight = readFontData(currentFont, HEIGHT_POS);
	uint16_t sizeOfJumpTable = readFontData(currentFont, CHAR_NUM_POS)  * JUMPTABLE_BYTES;
	uint8_t textHeight = readFontData(currentFont, HEIGHT_POS);

	if (code >= firstChar) {
		byte charCode = code - firstChar;
		// 4 Bytes per char code
		byte msbJumpToChar = readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES);                  // MSB  \ JumpAddress
		byte lsbJumpToChar = readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_LSB);   // LSB /
		byte charByteSize = readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_SIZE);  // Size
		byte currentCharWidth = readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_WIDTH); // Width

																														// Test if the char is drawable
		if (!(msbJumpToChar == 255 && lsbJumpToChar == 255)) {
			// Get the position of the char data
			uint16_t charDataPosition = JUMPTABLE_START + sizeOfJumpTable + ((msbJumpToChar << 8) + lsbJumpToChar);
			drawInternal(fontCursorX, fontCursorY, currentCharWidth, textHeight, currentFont, charDataPosition, charByteSize);
		}
		fontCursorX += currentCharWidth;
	}
		
	return 1;
}


void Graphics::drawInternal(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const uint8_t *data, uint16_t offset, uint16_t bytesInData) 
{

	if (width < 0 || height < 0) return;
	if (yMove + height < 0 || yMove > this->getHeight())  return;
	if (xMove + width  < 0 || xMove > this->getWidth())   return;

	uint8_t  rasterHeight = 1 + ((height - 1) >> 3); // fast ceil(height / 8.0)
	int8_t   yOffset = yMove & 7;

	bytesInData = bytesInData == 0 ? width * rasterHeight : bytesInData;

	int16_t initYMove = yMove;
	int8_t  initYOffset = yOffset;

	uint8_t arrayHeight = (int)ceil(height / 8.0);
	for (uint16_t i = 0; i < bytesInData; i++) {
		byte currentByte = readFontData(data, offset + i);

		for (int b = 0; b < 8; b++) {
			if (bitRead(currentByte, b)) {
				uint16_t currentBit = i * 8 + b;
				uint16_t pixelX = (i / arrayHeight);
				uint16_t pixelY = (i % arrayHeight) * 8;
				setPixel(pixelX + xMove, pixelY + yMove + b);
			}
		}
		yield();

	}
}


void Graphics::print(uint16_t x, uint16_t y, String text, Alignment alignment)
{
	uint16_t firstChar = readFontData(currentFont, FIRST_CHAR_POS);
	uint16_t lineHeight = readFontData(currentFont, HEIGHT_POS);
	uint16_t sizeOfJumpTable = readFontData(currentFont, CHAR_NUM_POS)  * JUMPTABLE_BYTES;
	uint8_t textHeight = readFontData(currentFont, HEIGHT_POS);

	if (alignment != Alignment::Left)
	{
		int textLength = 0;
		for (int i = 0; i < text.length(); i++)
		{
			uint8_t code = text[i];
			if (code >= firstChar) {
				byte charCode = code - firstChar;
				textLength += readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_WIDTH); // Width
			}
		}

		if (alignment == Alignment::Center)
			x -= textLength / 2;
		else if (alignment == Alignment::Right)
			x -= textLength;

	}
	for (int i = 0; i < text.length(); i++)
	{
		uint8_t code = text[i];
		if (code >= firstChar) {
			byte charCode = code - firstChar;
			// 4 Bytes per char code
			byte msbJumpToChar = readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES);                  // MSB  \ JumpAddress
			byte lsbJumpToChar = readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_LSB);   // LSB /
			byte charByteSize = readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_SIZE);  // Size
			byte currentCharWidth = readFontData(currentFont, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_WIDTH); // Width

																																// Test if the char is drawable
			if (!(msbJumpToChar == 255 && lsbJumpToChar == 255)) {
				// Get the position of the char data
				uint16_t charDataPosition = JUMPTABLE_START + sizeOfJumpTable + ((msbJumpToChar << 8) + lsbJumpToChar);
				drawInternal(x, y, currentCharWidth, textHeight, currentFont, charDataPosition, charByteSize);
			}
			x += currentCharWidth;
		}
	}

}

void Graphics::fillRect(int x1, int y1, int x2, int y2, int color)
{
	for (int x = x1; x <= x2; x++)
	{
		for (int y = y1; y <= y2; y++)
		{
			setPixel(x, y, color);
		}
	}
}
