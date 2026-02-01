#pragma once
#include <memory>
#include <unordered_map>
#include "Graphics.h"
#include "Sprite2D.h"
#include "Channels.h"

class UI_Result
{
public:
	UI_Result(Graphics& gfx, Rgph::RenderGraph& rg)
	{
		// Background
		pBackground = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game_Result.png" });
		pBackground->SetPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		pBackground->SetScale(SCREEN_WIDTH, SCREEN_HEIGHT);
		pBackground->SetFrame(1, 1, 1, 1, 1, 0, false);
		pBackground->LinkTechniques(rg);
	}
	~UI_Result() = default;

	void Update(void)
	{}

	void Submit(void)
	{
		pBackground->Submit(Chan::ui);
	}

	void Reset(void)
	{}
private:
	// Background
	std::unique_ptr<Sprite2D> pBackground;
};
