#pragma once

#include <stdint.h>

class DisplayDriver
{
public:
	DisplayDriver();
	~DisplayDriver();

	virtual const int getWidth() const = 0;
	virtual const int getHeight() const = 0;

	virtual void begin() = 0;
	virtual void setPixel(int x, int y, uint8_t color) = 0;
	virtual void display() = 0;
};

