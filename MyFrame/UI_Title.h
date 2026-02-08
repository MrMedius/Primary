#pragma once
#include <memory>
#include <unordered_map>
#include "Graphics.h"
#include "Button.h"
#include "Sprite2D.h"
#include "Channels.h"

#include "InputCodex.h"

class UI_Title
{
private:
	enum Title_Button_Type_Tag
	{
		Title_Button_None,

		Title_Button_Start,
		Title_Button_Fullscreen,
		Title_Button_End,
	};
public:
	UI_Title(Graphics& gfx, Rgph::RenderGraph& rg)
	{
		// Background
		pBackground = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game_Title.png" });
		pBackground->SetPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		pBackground->SetScale(SCREEN_WIDTH, SCREEN_HEIGHT);
		pBackground->SetFrame(1, 1, 1, 1, 1, 0, false);
		pBackground->LinkTechniques(rg);
		// Buttons
		pButtons[Title_Button_Start] = std::make_unique<Button>(gfx, rg, XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 190.0f), XMFLOAT2(100.0f, 50.0f), 3, 1, 1, 3, 2, "asset\\Images\\Game_UI\\UI_Title_Start.png");
		pButtons[Title_Button_Fullscreen] = std::make_unique<Button>(gfx, rg, XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 120.0f), XMFLOAT2(100.0f, 50.0f), 3, 1, 1, 3, 2, "asset\\Images\\Game_UI\\UI_Title_Fullscreen.png");
		pButtons[Title_Button_End] = std::make_unique<Button>(gfx, rg, XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50.0f), XMFLOAT2(100.0f, 50.0f), 3, 1, 1, 3, 2, "asset\\Images\\Game_UI\\UI_Title_End.png");
	}
	~UI_Title() = default;

	void Update(void)
	{
		auto& input = InputCodex::Get();

		if (input.MouseMovedOrButtons()) // mouse
		{
			XMFLOAT2 mousePos = { static_cast<float>(input.MouseX()),static_cast<float>(input.MouseY()) };

			bool haveChoose = false;
			for (auto& b : pButtons)
				if (b.second->MouseEnterCheck(mousePos))
				{
					chosenButtonTag = b.first;
					b.second->SetOnChoose(true);
					b.second->SetOnClick(input.MouseLeftPressed());

					haveChoose = true;
				}
				else
					b.second->SetOnChoose(false);

			if (!haveChoose) chosenButtonTag = Title_Button_None;
		}
		else // keyboard
		{
			if (input.KeyTriggered(KK_S) || input.GP_Triggered(0, Gamepad::GP_DPAD_DOWN) || input.GP_LeftY(0) < 0)
				if (chosenButtonTag != Title_Button_End && chosenButtonTag < Title_Button_End)
					chosenButtonTag = static_cast<Title_Button_Type_Tag>(chosenButtonTag + 1);
			if (input.KeyTriggered(KK_W) || input.GP_Triggered(0, Gamepad::GP_DPAD_UP) || input.GP_LeftY(0) > 0)
				if (chosenButtonTag != Title_Button_Start && chosenButtonTag > Title_Button_Start)
					chosenButtonTag = static_cast<Title_Button_Type_Tag>(chosenButtonTag - 1);
			for (auto& b : pButtons)
			{
				if (b.first == chosenButtonTag)
				{
					b.second->SetOnChoose(true);
					b.second->SetOnClick(input.KeyPressed(KK_ENTER) || input.GP_Pressed(0, Gamepad::GP_A));
				}
				else
					b.second->SetOnChoose(false);
			}
		}
		if (chosenButtonTag != Title_Button_None)
		{
			if (input.MouseLeftReleased() || input.KeyReleased(KK_ENTER) || input.GP_Released(0, Gamepad::GP_A))
			{
				pButtons[chosenButtonTag]->SetOnExecute(true);
				OnExecute = true;
			}
		}
		if (OnExecute)
		{
			switch (chosenButtonTag)
			{
			case UI_Title::Title_Button_Start:
				IsStart = true;
				break;
			case UI_Title::Title_Button_Fullscreen:
				IsFullscreen = true;
				break;
			case UI_Title::Title_Button_End:
				IsEnd = true;
				break;
			}
			OnExecute = false;
		}
	}

	void Submit(void)
	{
		pBackground->Submit(Chan::ui);

		for (auto& b : pButtons)
			b.second->Submit();
	}

	void Reset(void)
	{
		chosenButtonTag = Title_Button_None;
		OnExecute = false;
		IsStart = false;
		IsFullscreen = false;
		IsEnd = false;
		for (auto& b : pButtons) b.second->Reset();
	}

	bool GetIsStart() const { return IsStart; }
	bool GetIsFullscreen() const { return IsFullscreen; }
	void ResetFullscreen() { IsFullscreen = false; }
	bool GetIsEnd() const { return IsEnd; }
private:
	// Background
	std::unique_ptr<Sprite2D> pBackground;
	// Buttons
	std::unordered_map<Title_Button_Type_Tag, std::unique_ptr<Button>> pButtons;
	Title_Button_Type_Tag chosenButtonTag{ Title_Button_None };
	bool OnExecute{ false };
	bool IsStart{ false };
	bool IsFullscreen{ false };
	bool IsEnd{ false };
};
