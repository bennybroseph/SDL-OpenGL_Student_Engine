//////////////////////////////////////////////////////////////
// Project: Student Graphics Engine
// Author: Ben Odom
//////////////////////////////////////////////////////////////

#include "GameLoop.h"

int wmain()
{
	Graphics::Init();

	Graphics::NewWindow(UVector2(1600u, 900u), UVector2(1600u, 900u), false, "Graphics Engine");

	Graphics::NewCamera();

	GameLoop gameLoop;

	gameLoop.Run();

	Graphics::Quit();

	return 0;
}