#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "CameraContainer.h"
#include "PointLight.h"
#include "Model.h"
#include "ScriptCommander.h"
#include "InGameRenderGraph.h"

#include "Player.h"
#include "Block.h"
#include "EnemyManager.h"

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
	// Core components
	std::string commandLine;
	ImguiManager imgui;
	Window wnd;
	ScriptCommander scriptCommander;
	bool Pause{ false };
	// FPS calculation
	std::chrono::steady_clock::time_point LastFrameTime;
	std::chrono::steady_clock::time_point StartFrameTime;
	int FrameCounter{ 0 };
	Timer timer_update;
	Timer timer_FPS;
	float speed_factor = 1.0f;
	/********************************/
	/*         Game Related         */
	/********************************/
	PointLight light;
	CameraContainer cameras{ wnd.Gfx() };

	Rgph::InGameRenderGraph rg{ wnd.Gfx() };
	
	std::unique_ptr<EnemyManager> pEnemyManager;
	Player* pPlayer;
	Block* b;
};