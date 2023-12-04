#include <string>
#include <iostream>

#include "vld.h"
#include "Constants.hpp"
#include "SDL.h"
#include "Timer.h"
#include "Renderer.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* args[])
{
	SDL_Init(SDL_INIT_VIDEO);

	const std::string windowTitle{ "Rasterizer - Fratczak Jakub (2DAE10)" };

	SDL_Window* pWindow = SDL_CreateWindow(
		windowTitle.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, NULL);

	if (!pWindow)
		return 1;

	SDL_SetRelativeMouseMode(SDL_bool(true));

	Renderer renderer{ pWindow };

	std::cout << CONTROLS;

	Timer timer{};
	timer.Start();

	bool
		isLooping{ true },
		takeScreenshot{};
	float printTimer{};
	while (isLooping)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_X:
					takeScreenshot = true;
					break;

				case SDL_SCANCODE_F3:
					renderer.ToggleBilinearTextureInterpolation();
					break;

				case SDL_SCANCODE_F4:
					renderer.ToggleRenderDepthBuffer();
					break;

				case SDL_SCANCODE_F5:
					renderer.ToggleRotateMeshes();
					break;

				case SDL_SCANCODE_F6:
					renderer.ToggleUseNormalTextures();
					break;

				case SDL_SCANCODE_F7:
					renderer.CycleShadingMode();
					break;
				}
				break;

			case SDL_MOUSEWHEEL:
				renderer.m_Camera.IncrementFieldOfViewAngle(-float(event.wheel.y) / 20.0f);
				break;
			}
		}

		renderer.Update(timer);
		renderer.Render();

		timer.Update();
		printTimer += timer.GetElapsed();
		if (printTimer >= 1.0f)
		{
			printTimer = 0.0f;
			SDL_SetWindowTitle(pWindow, (windowTitle + " - dFPS: " + std::to_string(timer.GetdFPS())).c_str());
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!renderer.SaveBufferToImage())
			{
				system("CLS");
				std::cout
					<< CONTROLS
					<< "--------\n"
					<< "SCREENSHOT SAVED\n"
					<< "--------\n";
			}
			else
			{
				system("CLS");
				std::cout
					<< CONTROLS
					<< "--------\n"
					<< "SCREENSHOT ERROR\n"
					<< "--------\n";
			}

			takeScreenshot = false;
		}
	}
	timer.Stop();

	SDL_DestroyWindow(pWindow);
	SDL_Quit();
	return 0;
}