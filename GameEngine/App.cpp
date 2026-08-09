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

		if (const auto eCode = Window::ProcessMessages())
		{
			return *eCode;
		}

		DoFrame();
	}
}

void App::DoFrame()
{
	const float time = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision(1) << std::fixed << time << "s" << std::endl;
	window.setTitle(oss.str());
}
