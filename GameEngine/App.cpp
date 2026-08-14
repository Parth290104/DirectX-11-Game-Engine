#include "App.h"
#include <sstream>
#include <iomanip>

App::App() : window(800, 600, "My Window")
{

}

int App::Go()
{
	while (true)
	{
		// process all messages pending, but not block for new messages
		if (const auto eCode = Window::ProcessMessages())
		{
			// if return optional has value, means we are quitting so return exit code
			return *eCode;
		}

		DoFrame();
	}
}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	window.getGraphicsObject().ClearBuffer(c, c, 1.0f);
	window.getGraphicsObject().DrawTestTriangle(timer.Peek(), window.mouse.GetPosX() / 400.0f - 1.0f, -window.mouse.GetPosY() / 300.0 + 1.0f);
	window.getGraphicsObject().EndFrame();
}
