#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "CameraContainer.h"
#include "PointLight.h"
#include "CubeBase.h"
#include "Model.h"
#include "ScriptCommander.h"
#include "BlurOutlineRenderGraph.h"

#include "Player.h"
#include "Block.h"

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
	CameraContainer cameras{ wnd.Gfx() };

	Rgph::BlurOutlineRenderGraph rg{ wnd.Gfx() };
	//CubeBase cube{ wnd.Gfx(),{4.0f,4.0f,4.0f} };
	//Model sponza{ wnd.Gfx(),"asset\\Models\\sponza\\sponza.obj",1.0f / 20.0f };
	
	Player p;
	Block b;
};