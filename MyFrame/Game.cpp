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
#include "SoundCodex.h"

namespace dx = DirectX;

Game::Game(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, "Hello World"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx(), { 0.0f,25.0f,0.0f })
{
#ifdef _DEBUG
	cameras.AddCamera(light.ShareCamera());
#endif
	gameRG.BindShadowCamera(*light.ShareCamera());
	light.LinkTechniques(gameRG);
	cameras.LinkTechniques(gameRG);

	// UI
	pScore = std::make_unique<Score>(wnd.Gfx(), gameRG);
	pUI_Title = std::make_unique<UI_Title>(wnd.Gfx(), UIRG, &wnd.kbd, &wnd.mouse);
	pUI_Game = std::make_unique<UI_Game>(wnd.Gfx(), gameRG, pScore.get());
	pUI_Result = std::make_unique<UI_Result>(wnd.Gfx(), UIRG);
	// Objects
	pPlayer = ObjectCodex::Acquire<Player>(character_Player, wnd.Gfx(), gameRG, &wnd.kbd, &wnd.mouse, &cameras, XMFLOAT3{ 0.0f,5.0f,0.0f });
	pEnvironmentManager = std::make_unique<EnvironmentManager>(wnd.Gfx(), gameRG);
	pEnemyManager = std::make_unique<EnemyManager>(wnd.Gfx(), gameRG, &wnd.kbd, pScore.get());
	pEffectManager = std::make_unique<EffectManager>(wnd.Gfx(), gameRG);

	// Sound Base Setting
	SoundCodex::Get().PlayBGM(SndPath::BGM_Main, -1);
	SoundCodex::Get().SetBgmVolume(0.5f);
	SoundCodex::Get().SetSeVolume(1.0f);
	SoundCodex::Get().SetListenerTransform(0.0f, 0.0f, 0.0f, 0, 0, 1, 0, 1, 0);
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
			SoundCodex::Get().Update();
			/********************************/

			/********************************/
			/*　　	     　Draw             */
			Draw();
			wnd.Gfx().EndFrame(); // present
			gameRG.Reset();
			UIRG.Reset();
			/********************************/
		}
	}
}

void Game::Update(float dt)
{
	switch (Scene)
	{
	case SCENE_TITLE:
		pUI_Title->Update();

		if (pUI_Title->GetIsStart())
		{
			pUI_Title->Reset();
			SetScene(SCENE_GAME);
		}
		if (pUI_Title->GetIsEnd())
			PostQuitMessage(0);
		break;
	case SCENE_GAME:
		if (!Pause)
		{
			// Check If Game Over
			if (pPlayer->GetIsGameOver())	SetScene(SCENE_RESULT);

			// true loop
			auto playerPos = pPlayer->GetPosition();
			cameras.Update(dt, playerPos, &wnd);
			if (!cameras.GetScreenFroze())
			{
				light.Update(dt, playerPos);

				pPlayer->Update(dt);
				pUI_Game->Update(dt);

				if (!pPlayer->GetIsChange())
				{
					pEnvironmentManager->Update(dt);
					pEnemyManager->Update(dt, pPlayer->GetPosition());
					pEffectManager->Update(dt);
				}
			}
			gameRG.Update(dt);
			SoundCodex::Get().SetListenerPosition(playerPos);
		}
		break;
	case SCENE_RESULT:
		pUI_Result->Update();

		if (wnd.kbd.KeyIsTriggered(KK_SPACE))
		{
			SetScene(SCENE_TITLE);

			pPlayer->OnEnable();
			pEnemyManager->Reset();
			pEnvironmentManager->Reset();
			pEffectManager->Reset();
		}
		break;
	}	
}

void Game::Draw()
{
	switch (Scene)
	{
	case SCENE_TITLE:
	{
		// UI
		pUI_Title->Submit();

		UIRG.Execute(wnd.Gfx());
		break;
	}
	case SCENE_GAME:
	{
		light.Bind(wnd.Gfx(), cameras->GetMatrix());
		gameRG.BindMainCamera(cameras.GetActiveCamera());
#ifdef _DEBUG
		light.Submit(Chan::main);
#endif
		cameras.Submit(Chan::main);
		// Objects
		pEnvironmentManager->Submit();
		pEnemyManager->Submit();
		pEffectManager->Submit();
		pPlayer->Submit();
		// UI
		pUI_Game->Submit();

		gameRG.Execute(wnd.Gfx());

#ifdef _DEBUG
		// imgui windows
		cameras.SpawnWindow(wnd.Gfx());
		light.SpawnControlWindow();
		SoundCodex::Get().SpawnWindow();

		gameRG.RenderWindows(wnd.Gfx());
#endif
		break;
	}
	case SCENE_RESULT:
	{
		pUI_Result->Submit();

		UIRG.Execute(wnd.Gfx());
		break;
	}
	}
}