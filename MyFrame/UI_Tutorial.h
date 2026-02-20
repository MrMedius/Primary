#pragma once
#include "Graphics.h"
#include "Sprite2D.h"
#include "Channels.h"

#include "Player.h"
#include "GameStatsCodex.h"
#include "ObjectCodex.h"

#include "Math.h"

class UI_Tutorial
{
public:
	UI_Tutorial(Graphics& gfx, Rgph::RenderGraph& rg)
	{
		pPlayer = ObjectCodex::FindFirstActiveObjectByTag<Player>(character_Player);

		// Tutorials
		{
			XMFLOAT2 size = { 260.0f,60.0f };
			float baseY = 50.0f;
			float offset = 65.0f;
			for (int i = 0;i < tutorialNum;i++)
			{
				pTutorials.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Tutorial\\Tutorial_Sheet.png" }));
				pTutorials[i]->SetPosition(startX, baseY + i * offset);
				pTutorials[i]->SetScale(size);
				pTutorials[i]->SetFrame(2, 6, 1 + 2 * i, 1, 1, 0, false);
				pTutorials[i]->LinkTechniques(rg);
			}
		}
		// Checks
		{
			XMFLOAT2 size = { 50.0f,50.0f };
			for (int i = 0;i < tutorialNum;i++)
			{
				auto pos = pTutorials[i]->GetPosition();

				pChecks.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Tutorial\\Tutorial_Check.png" }));
				pChecks[i]->SetPosition(pos.x - offsetCheck, pos.y);
				pChecks[i]->SetScale(size);
				pChecks[i]->SetFrame(1, 1, 1, 1, 1, 0, false);
				pChecks[i]->LinkTechniques(rg);
			}
		}
		// GameStart
		{
			pGameStart = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Tutorial\\Tutorial_Game_Start.png" });
			pGameStart->SetPosition(SCREEN_WIDTH / 2.0f, gStartY);
			pGameStart->SetScale(500.0f, 500.0f);
			pGameStart->SetFrame(1, 1, 1, 1, 1, 0, false);
			pGameStart->LinkTechniques(rg);
		}
	}

	~UI_Tutorial() = default;

	void Update(float dt)
	{
		// Tutorials && Checks && GameStart
		if (finished && finishedCnt > 0)
		{
			// GameStart
			if (gameStartRate < 1.0f)
			{
				auto pos = pGameStart->GetPosition();

				gameStartRate += dt * 0.5f;
				gameStartRate = std::clamp(gameStartRate, 0.0f, 1.0f);

				float y = (gEndY - gStartY) * ease_in_pow(gameStartRate, 1.5f) + gStartY;
				pGameStart->SetPosition(pos.x, y);
			}
			if (gameStartRate > 0.9f)
			{
				for (int i = finishedCnt - 1; i >= 0; i--)
				{
					// Tutorials
					auto pos = pTutorials[i]->GetPosition();
					if (i == finishedCnt - 1)
					{
						// last one
						if (pos.x < startX)
							pos.x += (startX - pos.x) * 0.05f;
					}
					else
					{
						auto posLast = pTutorials[i + 1]->GetPosition();
						if (startX - posLast.x < (startX - endX) * 0.75f)
							pos.x += (startX - pos.x) * 0.05f;
					}
					if (startX - pos.x < (startX - endX) * 0.05f) finishedCnt--;
					pTutorials[i]->SetPosition(pos);

					// Checks
					pChecks[i]->SetPosition(pos.x - offsetCheck, pos.y);
				}
			}

			if(finishedCnt <= 0) GameStatsCodex::SetFinishTutorial();
		}
		else if (!finished)
		{
			auto in = pPlayer->Input();
			// Move
			if (!GameStatsCodex::GetIsMoved())
			{
				if (in.moveHeld)
				{
					GameStatsCodex::SetMoved();
					finishedCnt++;
				}
			}
			// Dash
			if (GameStatsCodex::GetIsMoved() && !GameStatsCodex::GetIsDashed())
			{
				if (in.dash)
				{
					GameStatsCodex::SetDashed();
					finishedCnt++;
				}
			}
			// Attack
			if (GameStatsCodex::GetIsDashed() && !GameStatsCodex::GetIsAttacked())
			{
				if (in.attack)
				{
					GameStatsCodex::SetAttacked();
					finishedCnt++;
				}
			}
			// Switch
			if (GameStatsCodex::GetIsAttacked() && !GameStatsCodex::GetIsSwitched())
			{
				if (in.slotL || in.slotR)
				{
					GameStatsCodex::SetSwitched();
					finishedCnt++;
				}
			}
			// Skill
			if (GameStatsCodex::GetIsSwitched() && !GameStatsCodex::GetIsSkilled())
			{
				if (in.skill)
				{
					GameStatsCodex::SetSkilled();
					finishedCnt++;
					finished = true;
				}
			}

			for (int i = 0;i <= finishedCnt;i++)
			{
				if (i == tutorialNum) break;

				// Tutorials
				auto pos = pTutorials[i]->GetPosition();
				if (pos.x == endX) continue;
				if (pos.x > endX) pos.x -= (pos.x - endX) * 0.15f;
				if (pos.x < endX) pos.x = endX;
				pTutorials[i]->SetPosition(pos);

				// Checks
				pChecks[i]->SetPosition(pos.x - offsetCheck, pos.y);
			}
		}

	}

	void Submit(void)
	{
		// Tutorials
		for (auto& t : pTutorials)	t->Submit(Chan::ui);
		// Checks
		for (int i = 0;i < finishedCnt;i++)	pChecks[i]->Submit(Chan::ui);
		// GameStart
		if(finished) pGameStart->Submit(Chan::ui);
	}

	void Reset(void)
	{
		finished = false;
		// Tutorials
		finishedCnt = 0;
		float baseY = 50.0f;
		float offset = 65.0f;
		for (int i = 0;i < tutorialNum;i++)	pTutorials[i]->SetPosition(startX, baseY + i * offset);
		// Checks
		for (int i = 0;i < tutorialNum;i++)
		{
			auto pos = pTutorials[i]->GetPosition();
			pChecks[i]->SetPosition(pos.x - offsetCheck, pos.y);
		}
		// GameStart
		gameStartRate = 0.0f;
		pGameStart->SetPosition(SCREEN_WIDTH / 2.0f, gStartY);
	}

private:
	Player* pPlayer;
	bool finished{ false };
	// Tutorials
	static constexpr float startX = SCREEN_WIDTH + 150.0f;
	static constexpr float endX = SCREEN_WIDTH - 150.0f;
	static constexpr int tutorialNum = 5;
	int finishedCnt{ 0 };
	std::vector<std::unique_ptr<Sprite2D>> pTutorials;
	// Checks
	static constexpr float offsetCheck = 100.0f;
	std::vector<std::unique_ptr<Sprite2D>> pChecks;
	// GameStart
	static constexpr float gStartY = -100.0f;
	static constexpr float gEndY = SCREEN_HEIGHT + 100.0f;
	float gameStartRate{ 0.0f };
	std::unique_ptr<Sprite2D> pGameStart;
};