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

	Graphics::NewWindow(UVector2(200u, 200u), UVector2(200u, 200u));

	GameLoop oGameLoop;

	oGameLoop.Loop();

	Graphics::Quit();

	return 0;
}