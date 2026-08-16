#pragma once
#include "Window.h"
#include "Timer.h"
#include <random>
#include "Box.h"

class App
{
public:
	App();
	int Go();

private:
	void DoFrame();

private:
	Window window;
	Timer timer;
	std::vector<std::unique_ptr<Box>> boxes;
};

