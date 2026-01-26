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

namespace dx = DirectX;


Game::Game(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, "Hello World"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx(), { 0.0f,5.0f,0.0f })
{
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "A", dx::XMFLOAT3{ -13.5f,6.0f,3.5f }, 0.0f, PI / 2.0f));
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "B", dx::XMFLOAT3{ -13.5f,28.8f,-6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f));
	cameras.AddCamera(light.ShareCamera());

	cube.SetPos({ 10.0f,5.0f,6.0f });
	cube2.SetPos({ 10.0f,5.0f,14.0f });
	nano.SetRootTransform(dx::XMMatrixRotationY(PI / 2.f) * dx::XMMatrixTranslation(27.f, -0.56f, 1.7f));
	gobber.SetRootTransform(dx::XMMatrixRotationY(-PI / 2.f) * dx::XMMatrixTranslation(-8.f, 10.f, 0.f));

	cube.LinkTechniques(rg);
	cube2.LinkTechniques(rg);
	light.LinkTechniques(rg);
	sponza.LinkTechniques(rg);
	gobber.LinkTechniques(rg);
	nano.LinkTechniques(rg);
	cameras.LinkTechniques(rg);

	spr.SetPos({ 0.0f,5.0f,6.0f });
	spr.LinkTechniques(rg);
	spr.SetAtlas(12, 16, 1, 48);
	spr.SetFPS(12.0f);


	rg.BindShadowCamera(*light.ShareCamera());
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
	cameras.Update(dt, &wnd);
	cube.Update(dt);

	spr.Update(dt);

	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());
}

void Game::Draw()
{
	// Main Channel
	light.Submit(Chan::main);
	sponza.Submit(Chan::main);
	cube.Submit(Chan::main);
	cube2.Submit(Chan::main);
	gobber.Submit(Chan::main);
	nano.Submit(Chan::main);
	cameras.Submit(Chan::main);

	spr.Submit(Chan::main);

	// Shadow Channel
	spr.Submit(Chan::shadow);
	
	sponza.Submit(Chan::shadow);
	cube.Submit(Chan::shadow);
	cube2.Submit(Chan::shadow);
	gobber.Submit(Chan::shadow);
	nano.Submit(Chan::shadow);



	rg.Execute(wnd.Gfx());

#ifdef _DEBUG
	// imgui windows
	static MP sponzeProbe{ "Sponza" };
	static MP gobberProbe{ "Gobber" };
	static MP nanoProbe{ "Nano" };
	sponzeProbe.SpawnWindow(sponza);
	gobberProbe.SpawnWindow(gobber);
	nanoProbe.SpawnWindow(nano);
	cameras.SpawnWindow(wnd.Gfx());
	light.SpawnControlWindow();
	cube.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	cube2.SpawnControlWindow(wnd.Gfx(), "Cube 2");


	spr.SpawnControlWindow(wnd.Gfx(), "S3D");

	rg.RenderWindows(wnd.Gfx());
#endif
}