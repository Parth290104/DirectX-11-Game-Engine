#include "App.h"
#include "Box.h"
#include <memory>

App::App() : window(800, 600, "My Window")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);

	for (auto i = 0; i < 60; ++i)
	{
		boxes.push_back(std::unique_ptr<Box>(new Box(window.getGraphicsObject(), rng, adist, ddist, odist, rdist)));
	}

	window.getGraphicsObject().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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
	auto dt = timer.Mark();
	window.getGraphicsObject().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& box : boxes)
	{
		box->Update(dt);
		box->Draw(window.getGraphicsObject());
	}
	window.getGraphicsObject().EndFrame();
}
