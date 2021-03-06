// Copyright (C) 2019 Nomango

#include "MainScene.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	try
	{
		Application app;

		Options options(L"Box2D Demo");
		app.Init(options);

		MainScenePtr scene = new MainScene;
		app.EnterScene(scene);
		app.Run();
	}
	catch (std::exception& e)
	{
		::MessageBoxA(nullptr, e.what(), "An exception has occurred!", MB_ICONERROR | MB_OK);
	}
	return 0;
}
