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
	light(wnd.Gfx(), { 0.0f,60.0f,0.0f }),
	p(wnd.Gfx(), rg, &wnd.kbd, &wnd.mouse, character_Player, { 4.0f,20.0f ,4.0f })
{
	cameras.AddCamera(light.ShareCamera());

	
	light.LinkTechniques(rg);
	//sponza.LinkTechniques(rg);
	cameras.LinkTechniques(rg);

	rg.BindShadowCamera(*light.ShareCamera());

	b = ObjectCodex::Acquire<Block>(environment_Block, wnd.Gfx(), rg, environment_Block, XMFLOAT3(0.0f, -5.0f, 0.0f), XMFLOAT3(200.0f, 10.0f, 200.0f), XMFLOAT3(200.0f, 10.0f, 200.0f), true);
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
	cameras.Update(dt, p.GetPosition(), &wnd);
	light.Update(dt, p.GetPosition());

	p.Update(dt);
	b->Update(dt);

	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());
}

void Game::Draw()
{
	// Main Channel
	light.Submit(Chan::main);
	//sponza.Submit(Chan::main);
	cameras.Submit(Chan::main);


	// Shadow Channel
	//sponza.Submit(Chan::shadow);


	p.Submit();
	b->Submit();

	rg.Execute(wnd.Gfx());

#ifdef _DEBUG
	// imgui windows
	//static MP sponzeProbe{ "Sponza" };
	//sponzeProbe.SpawnWindow(sponza);
	cameras.SpawnWindow(wnd.Gfx());
	light.SpawnControlWindow();

	rg.RenderWindows(wnd.Gfx());
#endif
}