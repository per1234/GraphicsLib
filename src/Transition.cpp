#include "Transition.h"
#include "Graphics.h"

#include <Arduino.h>

Transition::Transition(Graphics& graphics) : graphics(graphics)
{
}


Transition::~Transition()
{
}

void Transition::begin(std::function<void()> callback)
{
	this->drawCallback = callback;
}

void Transition::slideLeft(std::function<void()> callback)
{
	transitionStyle = Style::SlideLeft;
	transitionTime = 128;
	newdrawCallback = callback;
}

void Transition::draw()
{
	int x = 0;
	int y = 0;
	if (transitionStyle == Style::SlideLeft)
		x = 128-transitionTime;
	graphics.setOffset(x, y);
	drawCallback();

	if (transitionStyle != Style::None)
	{
		if (transitionStyle == Style::SlideLeft)
			x = - transitionTime;
		graphics.setOffset(x, y);
		newdrawCallback();
		graphics.setOffset(0, 0);
		
		Serial.println(x);

		transitionTime-=16;
		if (transitionTime == 0)
		{
			transitionStyle = Style::None;
			drawCallback = newdrawCallback;
			newdrawCallback = nullptr;
		}

	}
}
