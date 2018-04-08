#pragma once

#include <functional>

class Graphics;

class Transition
{
	std::function<void()> drawCallback;
	std::function<void()> newdrawCallback;
	int transitionTime;
	
	enum class Style
	{
		None,
		SlideLeft
	} transitionStyle = Style::None;
	Graphics& graphics;

public:
	Transition(Graphics &graphics);
	~Transition();

	void begin(std::function<void()> callback);


	void slideLeft(std::function<void()> callback);

	void draw();
	
};

