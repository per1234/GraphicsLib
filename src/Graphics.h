#pragma once

#include <Print.h>
#include <stdint.h>

class DisplayDriver;

class Graphics : public Print
{
	DisplayDriver* driver;
	uint8_t currentColor;
	uint8_t brushSize;

	const uint8_t* currentFont;
	int fontCursorX = 0;
	int fontCursorY = 0;

	int offsetX = 0;
	int offsetY = 0;

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
	void fillRect(int x1, int y1, int x2, int y2, int color);
	void fillRect(int x1, int y1, int x2, int y2);
	void drawPicture(int x, int y, const uint8_t* image);

	enum class Alignment
	{
		Left,
		Right,
		Center
	};

	void setOffset(int x, int y) { this->offsetX = x; this->offsetY = y; }

	void setCursor(int x, int y);
	void setFont(const uint8_t* fontData);
	uint8_t readFontData(const uint8_t * start, uint32_t offset);
	void drawInternal(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const uint8_t *data, uint16_t offset, uint16_t bytesInData);
	void print(uint16_t x, uint16_t y, String text, Alignment alignment = Graphics::Alignment::Left);

	// Inherited via Print
	virtual size_t write(uint8_t) override;

};

