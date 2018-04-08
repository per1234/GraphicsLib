#include "Graphics.h"
#include "DisplayDriver.h"

#include <Arduino.h>

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif


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
	this->driver->setPixel(x, y, color);
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
			driver->setPixel(x, y, color);
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
