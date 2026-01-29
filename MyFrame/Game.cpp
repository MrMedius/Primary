#include "Game.h"
#include <memory>
#include <algorithm>
#include "Math.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "Util.h"
#include "PerfLog.h"
#include "Camera.h"
#include "Channels.h"
#include "ModelWindow.h"
#include "ObjectBase.h"

#include "ObjectCodex.h"

namespace dx = DirectX;

Game::Game(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, "Hello World"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx(), { 0.0f,25.0f,0.0f })
{
	cameras.AddCamera(light.ShareCamera());

	light.LinkTechniques(rg);
	cameras.LinkTechniques(rg);

	pEnvironmentManager = std::make_unique<EnvironmentManager>(wnd.Gfx(), rg);
	pEnemyManager = std::make_unique<EnemyManager>(wnd.Gfx(), rg, &wnd.kbd);

	rg.BindShadowCamera(*light.ShareCamera());

	pPlayer = ObjectCodex::Acquire<Player>(character_Player, wnd.Gfx(), rg, &wnd.kbd, &wnd.mouse, character_Player, &cameras, XMFLOAT3{ 4.0f,10.0f ,4.0f });
}

Game::~Game()
{}

int Game::RunGame()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		if (timer_update.Peek() >= 1.0f / 60.0f) // Execute at max 60 FPS
		{
#ifdef _DEBUG
			FrameCounter++;
			// FPS Calculation and display in Debug Title
			if (timer_FPS.Peek() >= 1.0f)
			{
				wnd.SetDebugTitle(FrameCounter);
				FrameCounter = 0;
				timer_FPS.Mark();
			}
#endif
			/********************************/
			/*　　	   　 Update            */
			wnd.Gfx().BeginFrame();
			Update(timer_update.Mark() * speed_factor);
			wnd.kbd.Update();
			wnd.mouse.Update();
			/********************************/

			/********************************/
			/*　　	     　Draw             */
			Draw();
			wnd.Gfx().EndFrame(); // present
			rg.Reset();
			/********************************/
		}
	}
}

void Game::Update(float dt)
{
	if (!Pause)
	{
		//// ゲームオーバーの判断
		//if (pPlayer->GetIsGameOver())	SetScene(SCENE_RESULT);

		cameras.Update(dt, pPlayer->GetPosition(), &wnd);
		if (!cameras.GetScreenFroze())
		{
			light.Update(dt, pPlayer->GetPosition());

			pPlayer->Update(dt);

			if (!pPlayer->GetIsChange())
			{
				pEnvironmentManager->Update(dt);
				pEnemyManager->Update(dt, pPlayer->GetPosition());
			}
		}
	}
}

void Game::Draw()
{
	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());

	light.Submit(Chan::main);
	cameras.Submit(Chan::main);

	pPlayer->Submit();

	pEnvironmentManager->Submit();
	pEnemyManager->Submit();

	rg.Execute(wnd.Gfx());

#ifdef _DEBUG
	// imgui windows
	cameras.SpawnWindow(wnd.Gfx());
	light.SpawnControlWindow();

	rg.RenderWindows(wnd.Gfx());
#endif
}