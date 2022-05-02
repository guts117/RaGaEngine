#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "Game.h"

int main()
{
	Game game;
	game.init();

	double lastTime = glfwGetTime();
	float nbFrames = 0.0f;

	do {

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%f ms/frame\n", nbFrames);      //1000.0/double(nbFrames)for frametime/ nbFrames for fps
			nbFrames = 0;
			lastTime += 1.0;
		}
		game.update(nbFrames);
		//loop until window closed

		
	} while (!game.mainWindow.getShouldClose());

	return 0;
}