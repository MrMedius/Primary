#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "CameraContainer.h"
#include "PointLight.h"
#include "TestCube.h"
#include "Model.h"
#include "ScriptCommander.h"
#include "BlurOutlineRenderGraph.h"

#include "Player.h"

class Game
{
public:
	Game( const std::string& commandLine = "" );
	int RunGame();
	~Game();
private:
	void Update(float dt);
	void Draw(void);
	/********************************/
	/*         Game Related         */
	/********************************/
private:
	// FPS calculation
	std::chrono::steady_clock::time_point LastFrameTime;
	std::chrono::steady_clock::time_point StartFrameTime;
	int FrameCounter{ 0 };
	Timer timer_update;
	Timer timer_FPS;
	float speed_factor = 1.0f;
	// Core components
	std::string commandLine;
	ImguiManager imgui;
	Window wnd;
	ScriptCommander scriptCommander;
	/********************************/
	/*         Game Related         */
	/********************************/
	PointLight light;
	CameraContainer cameras;

	Rgph::BlurOutlineRenderGraph rg{ wnd.Gfx() };
	TestCube cube{ wnd.Gfx(),4.0f };
	TestCube cube2{ wnd.Gfx(),4.0f };
	Model sponza{ wnd.Gfx(),"asset\\Models\\sponza\\sponza.obj",1.0f / 20.0f };
	//Model gobber{ wnd.Gfx(),"asset\\Models\\gobber\\GoblinX.obj",4.0f };
	//Model nano{ wnd.Gfx(),"asset\\Models\\nano_textured\\nanosuit.obj",2.0f };

	Player p;
};