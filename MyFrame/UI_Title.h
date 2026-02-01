#pragma once
#include <memory>
#include <unordered_map>
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Button.h"
#include "Sprite2D.h"
#include "Channels.h"

class UI_Title
{
private:
	enum Title_Button_Type_Tag
	{
		Title_Button_None,

		Title_Button_Start,
		Title_Button_End,
	};
public:
	UI_Title(Graphics& gfx, Rgph::RenderGraph& rg, Keyboard* kbd, Mouse* mouse)
		:
		pKbd(kbd),
		pMouse(mouse)
	{
		// Background
		pBackground = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game_Title.png" });
		pBackground->SetPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		pBackground->SetScale(SCREEN_WIDTH, SCREEN_HEIGHT);
		pBackground->SetFrame(1, 1, 1, 1, 1, 0, false);
		pBackground->LinkTechniques(rg);
		// Buttons
		pButtons[Title_Button_Start] = std::make_unique<Button>(gfx, rg, XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 200.0f), XMFLOAT2(100.0f, 50.0f), 3, 1, 1, 3, 2, "asset\\Images\\UI_Start_Button.png");
		pButtons[Title_Button_End] = std::make_unique<Button>(gfx, rg, XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100.0f), XMFLOAT2(100.0f, 50.0f), 3, 1, 1, 3, 2, "asset\\Images\\UI_End_Button.png");
	}
	~UI_Title() = default;

	void Update(void)
	{
		if (pMouse->IsMove()) // mouse
		{
			XMFLOAT2 mousePos = { static_cast<float>(pMouse->GetPosX()),static_cast<float>(pMouse->GetPosY()) };

			bool haveChoose = false;
			for (auto& b : pButtons)
				if (b.second->MouseEnterCheck(mousePos))
				{
					chosenButtonTag = b.first;
					b.second->SetOnChoose(true);
					b.second->SetOnClick(pMouse->LeftIsPressed());

					haveChoose = true;
				}
				else
					b.second->SetOnChoose(false);

			if (!haveChoose) chosenButtonTag = Title_Button_None;
		}
		else // keyboard
		{
			if (pKbd->KeyIsTriggered(KK_S))
				if (chosenButtonTag != Title_Button_End)
					chosenButtonTag = static_cast<Title_Button_Type_Tag>(chosenButtonTag + 1);
			if (pKbd->KeyIsTriggered(KK_W))
				if (chosenButtonTag != Title_Button_Start)
					chosenButtonTag = static_cast<Title_Button_Type_Tag>(chosenButtonTag - 1);
			for (auto& b : pButtons)
			{
				if (b.first == chosenButtonTag)
				{
					b.second->SetOnChoose(true);
					b.second->SetOnClick(pKbd->KeyIsPressed(KK_ENTER));
				}
				else
					b.second->SetOnChoose(false);
			}
		}
		if (chosenButtonTag != Title_Button_None)
		{
			if (pMouse->LeftIsReleased() || pKbd->KeyIsReleased(KK_ENTER))
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
			case UI_Title::Title_Button_End:
				IsEnd = true;
				break;
			}
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
		IsEnd = false;
		for (auto& b : pButtons) b.second->Reset();
	}

	bool GetIsStart() const { return IsStart; }
	bool GetIsEnd() const { return IsEnd; }
private:
	Keyboard* pKbd;
	Mouse* pMouse;
	// Background
	std::unique_ptr<Sprite2D> pBackground;
	// Buttons
	std::unordered_map<Title_Button_Type_Tag, std::unique_ptr<Button>> pButtons;
	Title_Button_Type_Tag chosenButtonTag{ Title_Button_None };
	bool OnExecute{ false };
	bool IsStart{ false };
	bool IsEnd{ false };
};
