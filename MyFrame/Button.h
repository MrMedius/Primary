#pragma once
#include "Graphics.h"
#include "Sprite2D.h"
#include "Channels.h"

class Button
{
public:
	Button(Graphics& gfx, Rgph::RenderGraph& rg, XMFLOAT2 pos, XMFLOAT2 size, int numU, int numV, int idleNum, int chooseNum, int clickNum, std::string path)
		:
		NumU{ numU },
		NumV{ numV },
		IdleNum{ idleNum },
		ChooseNum{ chooseNum },
		ClickNum{ clickNum }
	{
		visualPre = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ path });
		visualPre->SetPosition(pos.x, pos.y);
		visualPre->SetScale(size.x, size.y);
		visualPre->SetFrame(NumU, NumV, IdleNum, 1, 1, 0, false);
		visualPre->LinkTechniques(rg);
	}
	~Button() = default;
	void Submit()
	{
		if (OnChoose)
			if (OnClick)
				visualPre->SetFrame(NumU, NumV, ClickNum, 1, 1, 0);
			else
				visualPre->SetFrame(NumU, NumV, ChooseNum, 1, 1, 0);
		else
			visualPre->SetFrame(NumU, NumV, IdleNum, 1, 1, 0);

		visualPre->Submit(Chan::ui);
	}
	bool MouseEnterCheck(XMFLOAT2 mousePos)
	{
		auto pos = visualPre->GetPosition();
		auto size = visualPre->GetScale();

		float right = pos.x + size.x / 2;	// âE
		float left =  pos.x - size.x / 2;	// ç∂
		float down =  pos.y + size.y / 2;	// â∫
		float up =	  pos.y - size.y / 2;	// è„

		bool onEnter = false;
		if (mousePos.x < right && mousePos.x > left && mousePos.y > up && mousePos.y < down)
			onEnter = true;

		return onEnter;
	}
	void SetOnChoose(bool choose) { OnChoose = choose; }
	bool GetOnChoose(void) const { return OnChoose; }
	void SetOnClick(bool click) { OnClick = click; }
	bool GetOnClick(void) const { return OnClick; }
	void SetOnExecute(bool execute) { OnExecute = execute; }
	bool GetOnExecute(void) const { return OnExecute; }
	void Reset(void)
	{
		OnChoose = false;
		OnClick = false;
		OnExecute = false;
	}
private:
	Graphics* pGfx;
	std::unique_ptr<Sprite2D> visualPre;
	int NumU{ 0 };
	int NumV{ 0 };
	int IdleNum{ 0 };
	int ChooseNum{ 0 };
	int ClickNum{ 0 };
	bool OnChoose{ false };
	bool OnClick{ false };
	bool OnExecute{ false };
};