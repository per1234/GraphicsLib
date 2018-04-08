#pragma once

#include <stdint.h>

class DisplayDriver;

class Graphics
{
	DisplayDriver* driver;
	uint8_t currentColor;
	uint8_t brushSize;

public:
	Graphics(DisplayDriver* driver);
	~Graphics();

	void begin();


	void clear(int color);
	inline void setColor(int color) {	this->currentColor = color;	}
	inline void setBrushSize(int size) { this->brushSize = size; }
	void setPixel(int x, int y);
	void setPixel(int x, int y, int color);
	void display();
	int getWidth() const;
	int getHeight() const;

	void drawLine(int x1, int y1, int x2, int y2, int color);
	void drawLine(int x1, int y1, int x2, int y2);
	void drawRect(int x1, int y1, int x2, int y2);
	void drawRect(int x1, int y1, int x2, int y2, int color);
	void fillRect(int x1, int y1, int x2, int y2);

	

};

