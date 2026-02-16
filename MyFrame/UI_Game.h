#pragma once
#include "Graphics.h"
#include "ObjectCodex.h"
#include "Player.h"
#include "Sprite2D.h"
#include "Sprite2DPolygon.h"
#include "UI_ResourceSlot.h"
#include "UI_WeaponSlot.h"
#include "UI_Score.h"
#include "Channels.h"

class UI_Game
{
public:
	UI_Game(Graphics& gfx, Rgph::RenderGraph& rg)
	{
		// Bg
		{
			pBg = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Hp_Low.png" });
			pBg->SetPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
			pBg->SetScale(SCREEN_WIDTH, SCREEN_HEIGHT);
			pBg->SetFrame(1, 1, 1, 1, 1, 0, false);
			pBg->LinkTechniques(rg);
		}
		// PlayerHpBar_Frame
		{
			pPlayerHpBar_Frame = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Player_HP_Bar.png" });
			pPlayerHpBar_Frame->SetPosition(200.0f, SCREEN_HEIGHT - 150.0f);
			pPlayerHpBar_Frame->SetScale(500.0f, 500.0f);
			pPlayerHpBar_Frame->SetFrame(12, 16, 12 * 16 - 1, 1, 1, 0, false);
			pPlayerHpBar_Frame->LinkTechniques(rg);
		}
		// PlayerHpBar_Bar
		{
			pPlayerHpBar_Bar = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Player_HP_Fill.png",
																						 "asset\\Images\\UI_Game\\Player_HP_Fever.png" });
			pPlayerHpBar_Bar->SetPosition(200.0f, SCREEN_HEIGHT - 68.0f);
			pPlayerHpBar_Bar->SetScale(300.0f, 40.0f);
			pPlayerHpBar_Bar->SetFrameAuto(8, 6, 1, 48, 0, 20.0f);
			pPlayerHpBar_Bar->LinkTechniques(rg);
		}
		// PlayerHpBar_Fire
		{
			pPlayerHpBar_Fire = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Player_HP_Bar.png" });
			pPlayerHpBar_Fire->SetPosition(200.0f, SCREEN_HEIGHT - 150.0f);
			pPlayerHpBar_Fire->SetScale(500.0f, 500.0f);
			pPlayerHpBar_Fire->SetFrameAuto(12, 16, 145, 46, 0, 20.0f);
			pPlayerHpBar_Fire->LinkTechniques(rg);
		}
		// PlayerHpBar_Side
		{
			pPlayerHpBar_Side = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Player_HP_Side.png" });
			pPlayerHpBar_Side->SetScale(25.0f, 50.0f);
			pPlayerHpBar_Side->SetFrameAuto(8, 6, 1, 48, 0, 30.0f);
			pPlayerHpBar_Side->LinkTechniques(rg);
		}
		// ResourceSlot
		for (size_t i = 0; i < slotNum; i++)
		{
			pResourceSlot[i] = std::make_unique<UI_ResourceSlot>(gfx, rg, static_cast<int>(i));
		}
		// WeaponSlot
		{
			pWeaponSlot = std::make_unique<UI_WeaponSlot>(gfx, rg);
		}
		// Combo
		comboNum.resize(slotNum);
		comboActive.resize(slotNum);
		pCombo.resize(slotNum);
		for (size_t i = 0; i < slotNum; i++)
		{
			pCombo[i].reserve(maxDigit);
			comboNum[i].resize(maxDigit, 0);
			for (size_t j = 0;j < maxDigit;j++)
			{
				pCombo[i].push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Number_No_2.png" }));
				pCombo[i][j]->LinkTechniques(rg);
			}
		}
		// Score
		pScore = std::make_unique<UI_Score>(gfx, rg);
		// CountDown
		{
			cdRemain = cdTotal;
			pCd_Frame = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Bars.png" });
			pCd_Frame->SetPosition(50.0f, 300.0f);
			pCd_Frame->SetScale(50.0f, 200.0f);
			pCd_Frame->SetFrame(2, 1, 1, 1, 1, 0, false);
			pCd_Frame->LinkTechniques(rg);
			pCd_Bar = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Game\\Bars.png" });
			pCd_Bar->SetPosition(50.0f, 300.0f);
			pCd_Bar->SetRotation(180.0f);
			pCd_Bar->SetScale(50.0f, 200.0f);
			pCd_Bar->SetFrame(2, 1, 2, 1, 1, 0, false);
			pCd_Bar->LinkTechniques(rg);
		}
	}
	~UI_Game() = default;

	void Update(float dt)
	{
		auto* player = ObjectCodex::FindFirstActiveObjectByTag<Player>(character_Player);
		bool fever = player->GetIsFever();

		// PlayerHpBar_Fire
		if (player->GetHpDrawParameter() < 0.5)
		{
			pPlayerHpBar_Fire->Update(dt);
		}

		if(!fever)
		{
			// PlayerHpBar_Frame
			pPlayerHpBar_Frame->SetFrame(12, 16, 12 * 16 - 1, 1, 1, 0, false);
			// PlayerHpBar_Bar
			pPlayerHpBar_Bar->SetTextureFrameIndex(0);
			pPlayerHpBar_Bar->Update(dt);
			pPlayerHpBar_Bar->SetRatioOffset(player->GetHpDrawParameter(), 1.0f);
			// PlayerHpBar_Side
			auto pos = pPlayerHpBar_Bar->GetPosition();
			auto size = pPlayerHpBar_Bar->GetScale();
			pPlayerHpBar_Side->SetPosition(pos.x + size.x / 2.0f, pos.y);
			pPlayerHpBar_Side->Update(dt);
			// ResourceSlot
			auto EnergyDrawParameters = player->GetResourceBars()->GetEnergyDrawParameters();
			auto EnergyFull = player->GetResourceBars()->GetEnergyFull();
			for (size_t i = 0; i < slotNum; i++)
			{
				pResourceSlot[i]->Update(dt, EnergyDrawParameters[i], EnergyFull[i], player->GetResourceBars()->GetEnergyChangedAt(i));
				if (EnergyFull[i]) player->GetResourceBars()->SetEnergyFullAt(i, false);
			}
			// WeaponSlot
			pWeaponSlot->Update(dt, player->GetWeaponSlots());
		}
		else
		{
			// PlayerHpBar_Frame
			pPlayerHpBar_Frame->SetFrame(12, 16, 12 * 16, 1, 1, 0, false);
			// PlayerHpBar_Bar
			pPlayerHpBar_Bar->SetTextureFrameIndex(1);
			pPlayerHpBar_Bar->Update(dt);
			pPlayerHpBar_Bar->SetRatioOffset(player->GetWeaponSlots()->GetFeverTimeDraw(), 1.0f);
			// PlayerHpBar_Side
			auto pos = pPlayerHpBar_Bar->GetPosition();
			auto size = pPlayerHpBar_Bar->GetScale();
			pPlayerHpBar_Side->SetPosition(pos.x + size.x / 2.0f, pos.y);
			pPlayerHpBar_Side->Update(dt);
		}

		// Combo
		{
			auto combos = player->GetResourceBars()->GetAttackCombos();
			auto countdowns = player->GetResourceBars()->GetAttackComboCountdowns();
			for (size_t i = 0; i < slotNum; i++)
			{
				comboActive[i] = false;
				std::fill(comboNum[i].begin(), comboNum[i].end(), 0);

				int combo = combos[i];
				float countdown = countdowns[i];

				if (combo == 0) continue;
				else
				{
					comboActive[i] = true;

					int maxCombo = 1;
					for (int t = 0; t < maxDigit; ++t) maxCombo *= 10;
					if (combo >= maxCombo) combo = maxCombo - 1;

					size_t k = 0;
					while (combo > 0 && k < maxDigit)
					{
						comboNum[i][k] = combo % 10;
						combo /= 10;
						++k;
					}

					float posX;
					float posY = 300.0f + static_cast<float>(i) * 50.0f;
					float size = countdown * 10.0f;
					for (size_t j = 0; j < maxDigit; j++)
					{
						posX = 200.0f + countdown * (20.0f - j * 5.0f);
						pCombo[i][j]->SetPosition(posX, posY);
						pCombo[i][j]->SetScale(size, size);
						pCombo[i][j]->SetFrame(5, 2, comboNum[i][j] + 1, 1, 1, 0, false);
					}
				}
			}
		}
		// Score
		pScore->Update(dt);
		// CountDown
		{
			cdRemain -= dt;
			if (cdRemain <= 0.0f) isTimeUp = true;
			pCd_Bar->SetRatioOffset(1.0f, cdRemain / cdTotal);
		}
	}

	void Submit(void)
	{
		auto* player = ObjectCodex::FindFirstActiveObjectByTag<Player>(character_Player);
		bool fever = player->GetIsFever();

		if (player->GetHpDrawParameter() < 0.5)
		{
			// Bg
			pBg->Submit(Chan::ui);
			// PlayerHpBar_Fire
			pPlayerHpBar_Fire->Submit(Chan::ui);
		}
		// PlayerHpBar_Frame
		pPlayerHpBar_Frame->Submit(Chan::ui);
		// PlayerHpBar_HP
		pPlayerHpBar_Bar->Submit(Chan::ui);
		// PlayerHpBar_Side
		pPlayerHpBar_Side->Submit(Chan::ui);
		if (!fever)
		{
			// ResourceSlot
			for (size_t i = 0; i < slotNum; i++)
			{
				pResourceSlot[i]->Submit();
			}
			// WeaponSlot
			pWeaponSlot->Submit();
		}

		// Combo
		for (size_t i = 0; i < slotNum; i++)
		{
			if (!comboActive[i]) continue;

			int highest = maxDigit - 1;
			while (highest > 0 && comboNum[i][highest] == 0) --highest;

			for (int j = 0; j <= highest; ++j)
				pCombo[i][j]->Submit(Chan::ui);
		}
		// Score
		pScore->Submit();
		// CountDown
		pCd_Frame->Submit(Chan::ui);
		pCd_Bar->Submit(Chan::ui);
	}

	void Reset(void)
	{
		// Score
		pScore->Reset();
		// CountDown
		cdRemain = cdTotal;
		isTimeUp = false;
		pCd_Bar->SetRatioOffset(1.0f, 1.0f);
	}

	bool GetIsTimeUp() const 
	{
		return isTimeUp;
	}

private:
	// Bg
	std::unique_ptr<Sprite2D> pBg;
	// PlayerHpBar_Frame
	std::unique_ptr<Sprite2D> pPlayerHpBar_Frame;
	// PlayerHpBar_HP
	std::unique_ptr<Sprite2D> pPlayerHpBar_Bar;
	// PlayerHpBar_Fire
	std::unique_ptr<Sprite2D> pPlayerHpBar_Fire;
	// PlayerHpBar_Side
	std::unique_ptr<Sprite2D> pPlayerHpBar_Side;
	// ResourceSlot
	static constexpr int slotNum = 3;
	std::unique_ptr<UI_ResourceSlot> pResourceSlot[slotNum];
	// WeaponSlot
	std::unique_ptr<UI_WeaponSlot> pWeaponSlot;
	// Combo
	static constexpr int maxDigit = 3;
	std::vector<std::vector<int>> comboNum;
	std::vector<bool> comboActive;
	std::vector<std::vector<std::unique_ptr<Sprite2D>>> pCombo;
	// Score
	std::unique_ptr<UI_Score> pScore;
	// CountDown
	static constexpr float cdTotal = 300.0f;
	float cdRemain{ 0.0f };
	bool isTimeUp{ false };
	std::unique_ptr<Sprite2D> pCd_Frame;
	std::unique_ptr<Sprite2D> pCd_Bar;
};