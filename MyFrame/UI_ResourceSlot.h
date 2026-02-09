#pragma once
#include "Graphics.h"
#include "Sprite2D.h"
#include "Sprite2DPolygon.h"
#include "Channels.h"

class UI_ResourceSlot
{
public:
	UI_ResourceSlot(Graphics& gfx, Rgph::RenderGraph& rg, int cnt)
	{
		count = cnt;
		float posX = SCREEN_WIDTH - 360.0f + static_cast<float>(count) * 140.0f;
		float posY = 550.0f;
		float size = 150.0f;

		// ResourceSlot_Bg
		{
			Bg = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Resource_Slot_Bg.png" });
			Bg->SetPosition(posX, posY);
			Bg->SetScale(size, size);
			Bg->SetFrameAuto(10, 10, 1, 100, 0, 20.0f, false, true);
			Bg->LinkTechniques(rg);
		}
		// ResourceSlot_Icon_Bg
		{
			float scale = 0.9f;
			IconBg = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Resource_Slot_Icon_Bg.png" });
			IconBg->SetPosition(posX, posY);
			IconBg->SetScale(size * scale, size * scale);
			IconBg->SetFrame(3, 1, count + 1, 1, 1, 0, false);
			IconBg->LinkTechniques(rg);
		}
		// ResourceSlot_Icon
		{
			Icon = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Resource_Slot_Icon.png" });
			Icon->SetPosition(posX, posY);
			Icon->SetScale(size / 2.2f, size / 2.2f);
			Icon->SetFrame(3, 1, count + 1, 1, 1, 0, false);
			Icon->LinkTechniques(rg);
		}
		// ResourceSlot_Bar
		{
			Bar = std::make_unique<Sprite2DPolygon>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Resource_Slot_Bar.png" });
			Bar->SetPosition(posX, posY);
			Bar->SetScale(size, size);
			Bar->SetFrameAuto(8, 3, 17 - count * 8, 2, 0, 20.0f, false, true);
			Bar->SetRingRange(30.0f, 330.0f);
			Bar->LinkTechniques(rg);
		}
		barState = IDLE;
	}
	~UI_ResourceSlot() = default;

	void Update(float dt, float ratio, bool full)
	{
		// ResourceSlot_Bg
		Bg->Update(dt);
		// ResourceSlot_Bar
		Bar->Update(dt);
		switch (barState)
		{
		case IDLE:
			Bar->SetRingRatio(ratio);
			if (full)
			{
				barState = FULL;
				Bar->SetFrameAuto(8, 3, 17 - count * 8, 7, 0, 20.0f, false, false);
			}
			break;
		case FULL:
			if (Bar->ClipFinished())
			{
				barState = IDLE;
				Bar->SetFrameAuto(8, 3, 17 - count * 8, 2, 0, 20.0f, false, true);
				Bar->SetRingRatio(ratio);
			}
			break;
		}
	}
	void Submit(void)
	{
		// ResourceSlot_Bg
		Bg->Submit(Chan::ui);
		// ResourceSlot_Icon_Bg
		IconBg->Submit(Chan::ui);
		// ResourceSlot_Icon
		Icon->Submit(Chan::ui);
		// ResourceSlot_Bar
		Bar->Submit(Chan::ui);
	}
private:
	// ResourceSlot_Bg
	std::unique_ptr<Sprite2D> Bg;
	// ResourceSlot_Icon_Bg
	std::unique_ptr<Sprite2D> IconBg;
	// ResourceSlot_Icon
	std::unique_ptr<Sprite2D> Icon;
	// ResourceSlot_Bar
	std::unique_ptr<Sprite2DPolygon> Bar;
	int count;
	enum BarState
	{
		IDLE,
		FULL,
	} barState;
};