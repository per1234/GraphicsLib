#pragma once

#include "DisplayDriver.h"

class SH1106 : public DisplayDriver
{
	uint8_t address, sda, scl;
	uint8_t* buffer;

public:
	enum Color
	{
		Black,
		White
	};

	SH1106(uint8_t address, uint8_t sda, uint8_t scl);

	// Inherited via DisplayDriver
	virtual const int getWidth() const override { return 128; }
	virtual const int getHeight() const override { return 64; }
	virtual void begin() override;
	virtual void setPixel(int x, int y, uint8_t color) override;
	virtual void display() override;
	virtual void setPower(bool power);
private:
	virtual void sendCommand(uint8_t data);
	void sendInitCommands();
};