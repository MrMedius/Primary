#pragma once
#include <memory>
#include <unordered_map>
#include "Graphics.h"
#include "Button.h"
#include "Sprite2D.h"
#include "Sprite2DPolygon.h"
#include "Channels.h"

#include "GameStatsCodex.h"

class UI_Result
{
public:
	UI_Result(Graphics& gfxIn, Rgph::RenderGraph& rgIn)
		:
		gfx(gfxIn),
		rg(rgIn)
	{
		// Background
		{
			pBackground = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\UI_Result.png" });
			pBackground->SetPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			pBackground->SetScale(SCREEN_WIDTH, SCREEN_HEIGHT);
			pBackground->SetFrame(1, 1, 1, 1, 1, 0, false);
			pBackground->LinkTechniques(rg);
		}
		// Clear
		{
			pClear = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{
				"asset\\Images\\UI_Result\\victory.png" ,
				"asset\\Images\\UI_Result\\game_over.png"
			});
			pClear->SetPosition(300.0f, 120.0f);
			pClear->SetRotation(-15.0f);
			pClear->SetScale(200.0f, 100.0f);
			pClear->LinkTechniques(rg);
		}
		// Score
		for (int i = 0; i < scoreDigitsNum; i++)
		{
			scoreNum.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Number_No_2.png" }));
			scoreNum[i]->SetPosition(800.0f - 30.0f * i, 120.0f);
			scoreNum[i]->SetScale(50.0f, 50.0f);
			scoreNum[i]->LinkTechniques(rg);
		}
		// Time (mm : ss)
		{
			pColon = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Colon.png" });
			pColon->SetPosition(1100.0f - 30.0f * 2, 120.0f);
			pColon->SetScale(50.0f, 50.0f);
			pColon->SetFrame(1, 1, 1, 1, 1, 0, false);
			pColon->LinkTechniques(rg);
			for (int i = 0; i < timeDigitsNum; i++)
			{
				timeNum.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Number_No_2.png" }));
				float x = 1100.0f - 30.0f * (i >= 2 ? (i + 1) : i);
				float y = 120.0f;
				timeNum[i]->SetPosition(x, y);
				timeNum[i]->SetScale(50.0f, 50.0f);
				timeNum[i]->LinkTechniques(rg);
			}
		}
		// Enemy
		{
			float size = 120.0f;
			float ringRate = 2.0f;
			float posX = 250.0f;
			float posY = 300.0f;
			float offset = 150.0f;
			enemyR = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\enemy.png" });
			enemyR->SetPosition(posX, posY);
			enemyR->SetScale(size, size);
			enemyR->SetFrame(3, 1, 3, 1, 1, 0, false);
			enemyR->LinkTechniques(rg);
			enemyRBar = std::make_unique<Sprite2DPolygon>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\ring.png" });
			enemyRBar->SetPosition(posX, posY);
			enemyRBar->SetScale(size * ringRate, size * ringRate);
			enemyRBar->SetFrameAuto(8, 3, 17, 2, 0, 30.0f);
			enemyRBar->SetRingRange(30.0f, 330.0f);
			enemyRBar->LinkTechniques(rg);
			enemyG = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\enemy.png" });
			enemyG->SetPosition(posX, posY + offset);
			enemyG->SetScale(size, size);
			enemyG->SetFrame(3, 1, 2, 1, 1, 0, false);
			enemyG->LinkTechniques(rg);
			enemyGBar = std::make_unique<Sprite2DPolygon>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\ring.png" });
			enemyGBar->SetPosition(posX, posY + offset);
			enemyGBar->SetScale(size * ringRate, size * ringRate);
			enemyGBar->SetFrameAuto(8, 3, 9, 2, 0, 30.0f);
			enemyGBar->SetRingRange(30.0f, 330.0f);
			enemyGBar->LinkTechniques(rg);
			enemyB = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\enemy.png" });
			enemyB->SetPosition(posX, posY + offset * 2.0f);
			enemyB->SetScale(size, size);
			enemyB->SetFrame(3, 1, 1, 1, 1, 0, false);
			enemyB->LinkTechniques(rg);
			enemyBBar = std::make_unique<Sprite2DPolygon>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\ring.png" });
			enemyBBar->SetPosition(posX, posY + offset * 2.0f);
			enemyBBar->SetScale(size * ringRate, size * ringRate);
			enemyBBar->SetFrameAuto(8, 3, 1, 2, 0, 30.0f);
			enemyBBar->SetRingRange(30.0f, 330.0f);
			enemyBBar->SetRingRange();
			enemyBBar->LinkTechniques(rg);
		}
		// Weapon
		{
			float size = 120.0f;
			float ringRate = 2.0f;
			float posX = 450.0f;
			float posY = 300.0f;
			float offset = 150.0f;
			weaponR = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\weapon.png" });
			weaponR->SetPosition(posX, posY);
			weaponR->SetScale(size, size);
			weaponR->SetFrameAuto(29, 21, 29 * 15 + 1, 29 * 4, 0, 30.0f);
			weaponR->LinkTechniques(rg);
			weaponRBar = std::make_unique<Sprite2DPolygon>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\ring.png" });
			weaponRBar->SetPosition(posX, posY);
			weaponRBar->SetScale(size * ringRate, size * ringRate);
			weaponRBar->SetFrameAuto(8, 3, 17, 2, 0, 30.0f);
			weaponRBar->SetRingRange(30.0f, 330.0f);
			weaponRBar->LinkTechniques(rg);
			weaponG = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\weapon.png" });
			weaponG->SetPosition(posX, posY + offset);
			weaponG->SetScale(size, size);
			weaponG->SetFrameAuto(29, 21, 29 * 8 + 1, 29 * 4, 0, 30.0f);
			weaponG->LinkTechniques(rg);
			weaponGBar = std::make_unique<Sprite2DPolygon>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\ring.png" });
			weaponGBar->SetPosition(posX, posY + offset);
			weaponGBar->SetScale(size * ringRate, size * ringRate);
			weaponGBar->SetFrameAuto(8, 3, 9, 2, 0, 30.0f);
			weaponGBar->SetRingRange(30.0f, 330.0f);
			weaponGBar->LinkTechniques(rg);
			weaponB = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\weapon.png" });
			weaponB->SetPosition(posX, posY + offset * 2.0f);
			weaponB->SetScale(size, size);
			weaponB->SetFrameAuto(29, 21, 29 * 1 + 1, 29 * 4, 0, 30.0f);
			weaponB->LinkTechniques(rg);
			weaponBBar = std::make_unique<Sprite2DPolygon>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\ring.png" });
			weaponBBar->SetPosition(posX, posY + offset * 2.0f);
			weaponBBar->SetScale(size * ringRate, size * ringRate);
			weaponBBar->SetFrameAuto(8, 3, 1, 2, 0, 30.0f);
			weaponBBar->SetRingRange(30.0f, 330.0f);
			weaponBBar->SetRingRange();
			weaponBBar->LinkTechniques(rg);
		}
		// Combo
		{
			float size = 120.0f;
			float posX = 650.0f;
			float posY = 300.0f;
			float offset = 150.0f;
			comboR = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\combo.png" });
			comboR->SetPosition(posX, posY);
			comboR->SetScale(size, size);
			comboR->SetFrame(3, 1, 1, 1, 1, 0);
			comboR->LinkTechniques(rg);
			comboG = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\combo.png" });
			comboG->SetPosition(posX, posY + offset);
			comboG->SetScale(size, size);
			comboG->SetFrame(3, 1, 2, 1, 1, 0);
			comboG->LinkTechniques(rg);
			comboB = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\combo.png" });
			comboB->SetPosition(posX, posY + offset * 2.0f);
			comboB->SetScale(size, size);
			comboB->SetFrame(3, 1, 3, 1, 1, 0);
			comboB->LinkTechniques(rg);
			for (int i = 0; i < comboDigitsNum; i++)
			{
				comboRNum.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Number_No_2.png" }));
				comboRNum[i]->SetPosition(posX + 90.0f - 30.0f * i, posY);
				comboRNum[i]->SetScale(50.0f, 50.0f);
				comboRNum[i]->LinkTechniques(rg);
				comboGNum.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Number_No_2.png" }));
				comboGNum[i]->SetPosition(posX + 90.0f - 30.0f * i, posY + offset);
				comboGNum[i]->SetScale(50.0f, 50.0f);
				comboGNum[i]->LinkTechniques(rg);
				comboBNum.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Number_No_2.png" }));
				comboBNum[i]->SetPosition(posX + 90.0f - 30.0f * i, posY + offset * 2.0f);
				comboBNum[i]->SetScale(50.0f, 50.0f);
				comboBNum[i]->LinkTechniques(rg);
			}
		}
		// Output
		for (int i = 0; i < outputDigitsNum; i++)
		{
			outputNum.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Number_No_2.png" }));
			outputNum[i]->SetPosition(1100.0f - 50.0f * i, 300.0f);
			outputNum[i]->SetScale(80.0f, 80.0f);
			outputNum[i]->LinkTechniques(rg);
		}
		// Input
		for (int i = 0; i < inputDigitsNum; i++)
		{
			inputNum.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Number_No_2.png" }));
			inputNum[i]->SetPosition(1100.0f - 50.0f * i, 400.0f);
			inputNum[i]->SetScale(80.0f, 80.0f);
			inputNum[i]->LinkTechniques(rg);
		}
		// Button
		backButton = std::make_unique<Button>(gfx, rg, XMFLOAT2(SCREEN_WIDTH - 100.0f, SCREEN_HEIGHT - 100.0f), XMFLOAT2(100.0f, 50.0f), XMFLOAT2(100.0f, 50.0f), 3, 1, 1, 3, 2, "asset\\Images\\UI_Result\\UI_Result_Back.png");
	}
	~UI_Result() = default;

	void Update(float dt)
	{
		auto data = GameStatsCodex::Get();
		auto SmoothTo = [](float& cur, float target, float k, float dt)
			{
				if (dt <= 0.0f) { cur = target; return; }
				const float a = 1.0f - std::exp(-k * dt);   // 0..1 the bigger k is, the faster number changes
				cur += (target - cur) * a;
			};
		float rate = 10.0f;
		auto SnapIfClose = [](float& cur, float target, float eps = 0.05f)
			{
				if (std::fabs(cur - target) <= eps) cur = target;
			};
		auto Reached = [](float cur, float target, float eps = 0.05f)
			{
				return std::fabs(cur - target) <= eps;
			};

		// Score
		if (showPhase >= 0)
		{
			SmoothTo(scoreDraw, (float)data.score, rate, dt);
			SnapIfClose(scoreDraw, (float)data.score);

			const int sd = (int)(scoreDraw + 0.5f);
			int v = sd;
			for (int i = 0; i < scoreDigitsNum; i++)
			{
				const int d = v % 10; v /= 10;
				scoreNum[i]->SetFrame(5, 2, d + 1, 1, 1, 0, false);
			}

			// Next Phase
			if (Reached(scoreDraw, (float)data.score) && showPhase == 0)
				showPhase = 1;
		}
		// Time
		if (showPhase >= 1)
		{
			SmoothTo(timeDraw, data.lifeTime, rate, dt); // second
			SnapIfClose(timeDraw, data.lifeTime);

			const int td = (int)(timeDraw + 0.5f);
			// turn second to minute
			int totalSec = (int)td;
			int mm = totalSec / 60;
			int ss = totalSec % 60;
			// limit to two-digit minutes
			if (mm > 99) mm = 99;
			int m0 = mm % 10; int m1 = (mm / 10) % 10;
			int s0 = ss % 10; int s1 = (ss / 10) % 10;
			// array the four digits
			int digits[4] = { s0, s1, m0, m1 };
			for (int i = 0; i < timeDigitsNum; i++)
			{
				timeNum[i]->SetFrame(5, 2, digits[i] + 1, 1, 1, 0, false);
			}

			// Next Phase
			if (Reached(timeDraw, (float)data.lifeTime) && showPhase == 1)
				showPhase = 2;
		}
		// Enemy
		if (showPhase >= 2)
		{
			SmoothTo(enemyRDraw, enemyRTarget, rate, dt);
			SnapIfClose(enemyRDraw, enemyRTarget);

			const float rGate = enemyRTarget * 0.25f;
			if (enemyRTarget <= 0.0f || enemyRDraw >= rGate)
			{
				SmoothTo(enemyGDraw, enemyGTarget, rate, dt);
				SnapIfClose(enemyGDraw, enemyGTarget);
			}

			const float gGate = enemyGTarget * 0.25f;
			if ((enemyRTarget <= 0.0f && enemyGTarget <= 0.0f) || enemyGDraw >= gGate)
			{
				SmoothTo(enemyBDraw, enemyBTarget, rate, dt);
				SnapIfClose(enemyBDraw, enemyBTarget);
			}

			enemyRBar->SetRingRatio(std::clamp(enemyRDraw, 0.0f, 1.0f));
			enemyGBar->SetRingRatio(std::clamp(enemyGDraw, 0.0f, 1.0f));
			enemyBBar->SetRingRatio(std::clamp(enemyBDraw, 0.0f, 1.0f));

			enemyRBar->Update(dt);
			enemyGBar->Update(dt);
			enemyBBar->Update(dt);

			// Next Phase
			if (Reached(enemyRDraw, enemyRTarget) &&
				Reached(enemyGDraw, enemyGTarget) &&
				Reached(enemyBDraw, enemyBTarget) && 
				showPhase >= 2) 
				showPhase = 3;
		}
		// Weapon
		if (showPhase == 3)
		{
			SmoothTo(weaponRDraw, weaponRTarget, rate, dt);
			SnapIfClose(weaponRDraw, weaponRTarget);

			const float rGate = weaponRTarget * 0.25f;
			if (weaponRTarget <= 0.0f || weaponRDraw >= rGate)
			{
				SmoothTo(weaponGDraw, weaponGTarget, rate, dt);
				SnapIfClose(weaponGDraw, weaponGTarget);
			}

			const float gGate = weaponGTarget * 0.25f;
			if ((weaponRTarget <= 0.0f && weaponGTarget <= 0.0f) || weaponGDraw >= gGate)
			{
				SmoothTo(weaponBDraw, weaponBTarget, rate, dt);
				SnapIfClose(weaponBDraw, weaponBTarget);
			}

			weaponRBar->SetRingRatio(std::clamp(weaponRDraw, 0.0f, 1.0f));
			weaponGBar->SetRingRatio(std::clamp(weaponGDraw, 0.0f, 1.0f));
			weaponBBar->SetRingRatio(std::clamp(weaponBDraw, 0.0f, 1.0f));

			weaponR->Update(dt);
			weaponG->Update(dt);
			weaponB->Update(dt);
			weaponRBar->Update(dt);
			weaponGBar->Update(dt);
			weaponBBar->Update(dt);

			// Next Phase
			if (Reached(weaponRDraw, weaponRTarget) &&
				Reached(weaponGDraw, weaponGTarget) &&
				Reached(weaponBDraw, weaponBTarget) && 
				showPhase >= 3) 
				showPhase = 4;
		}
		// Combo
		if (showPhase >= 4)
		{
			SmoothTo(comboRDraw, (float)data.rHighestCombo, rate, dt);
			SnapIfClose(comboRDraw, (float)data.rHighestCombo);

			const float rGate = (float)data.rHighestCombo * 0.25f;
			if ((float)data.rHighestCombo <= 0.0f || comboRDraw >= rGate)
			{
				SmoothTo(comboGDraw, (float)data.gHighestCombo, rate, dt);
				SnapIfClose(comboGDraw, (float)data.gHighestCombo);
			}

			const float gGate = (float)data.gHighestCombo * 0.25f;
			if (((float)data.rHighestCombo <= 0.0f && (float)data.gHighestCombo <= 0.0f) || comboGDraw >= gGate)
			{
				SmoothTo(comboBDraw, (float)data.bHighestCombo, rate, dt);
				SnapIfClose(comboBDraw, (float)data.bHighestCombo);
			}

			int r = (int)(comboRDraw + 0.5f);
			int g = (int)(comboGDraw + 0.5f);
			int b = (int)(comboBDraw + 0.5f);
			r = std::clamp(r, 0, 99);
			g = std::clamp(g, 0, 99);
			b = std::clamp(b, 0, 99);

			auto SetDigits2 = [](std::vector<std::unique_ptr<Sprite2D>>& digits, int value)
				{
					for (int i = 0; i < 2; i++)
					{
						const int d = value % 10;
						value /= 10;
						digits[i]->SetFrame(5, 2, d + 1, 1, 1, 0, false);
					}
				};

			SetDigits2(comboRNum, r);
			SetDigits2(comboGNum, g);
			SetDigits2(comboBNum, b);

			// Next Phase
			if (Reached(comboRDraw, (float)data.rHighestCombo) &&
				Reached(comboGDraw, (float)data.gHighestCombo) &&
				Reached(comboBDraw, (float)data.bHighestCombo) &&
				showPhase == 4) 
				showPhase = 5;
		}
		// Output
		if (showPhase >= 5)
		{
			SmoothTo(outputDraw, (float)data.outputDamage, rate, dt);
			SnapIfClose(outputDraw, (float)data.outputDamage);

			const int sd = (int)(outputDraw + 0.5f);
			int v = sd;
			for (int i = 0; i < outputDigitsNum; i++)
			{
				const int d = v % 10; v /= 10;
				outputNum[i]->SetFrame(5, 2, d + 1, 1, 1, 0, false);
			}

			// Next Phase
			if (Reached(outputDraw, (float)data.outputDamage) && showPhase == 5) 
				showPhase = 6;
		}
		// Input
		if (showPhase >= 6)
		{
			SmoothTo(inputDraw, (float)data.inputDamage, rate, dt);
			SnapIfClose(inputDraw, (float)data.inputDamage);

			const int sd = (int)(inputDraw + 0.5f);
			int v = sd;
			for (int i = 0; i < inputDigitsNum; i++)
			{
				const int d = v % 10; v /= 10;
				inputNum[i]->SetFrame(5, 2, d + 1, 1, 1, 0, false);
			}

			// Next Phase
			if (Reached(inputDraw, (float)data.inputDamage) && showPhase == 6)
				showPhase = 7;
		}
		// Total
		if (showPhase >= 7)
		{
			SmoothTo(totalDraw, totalScore, rate, dt);
			SnapIfClose(totalDraw, totalScore);

			int td = (int)(totalDraw + 0.5f);
			if (td < 0) td = 0;

			int v = td;
			for (size_t i = 0; i < totalNum.size(); i++)
			{
				const int d = v % 10; v /= 10;
				totalNum[i]->SetFrame(5, 2, d + 1, 1, 1, 0, false);
			}

			// Next Phase
			if (totalDraw >= totalScore && showPhase == 7) showPhase = 8;
		}
		// Button
		if (showPhase >= 8)
		{
			auto& input = InputCodex::Get();
			XMFLOAT2 mousePos = { (float)input.MouseX(), (float)input.MouseY() };

			const bool mouseChoose = backButton->MouseEnterCheck(mousePos);

			const bool keyClick = input.KeyPressed(KK_ENTER) || input.GP_Pressed(0, Gamepad::GP_A);
			const bool keyExec = input.KeyReleased(KK_ENTER) || input.GP_Released(0, Gamepad::GP_A);

			backButton->SetOnChoose(mouseChoose || keyClick || keyExec);

			backButton->SetOnClick((mouseChoose && input.MouseLeftPressed()) || keyClick);

			const bool execute = (mouseChoose && input.MouseLeftReleased()) || keyExec;

			if (execute)
			{
				backButton->SetOnExecute(true);
				backButton->SetOnExecute(false);
				toTitle = true;
			}
			else
			{
				backButton->SetOnExecute(false);
			}
		}
	}

	void Submit(void)
	{
		// Background
		//pBackground->Submit(Chan::ui);
		// Clear
		if (showPhase >= 0)
		{
			pClear->Submit(Chan::ui);
		}
		// Score
		if (showPhase >= 0)
		{
			for (auto& n : scoreNum)
			{
				n->Submit(Chan::ui);
			}
		}
		// Time
		if (showPhase >= 1)
		{
			pColon->Submit(Chan::ui);
			for (auto& t : timeNum)
			{
				t->Submit(Chan::ui);
			}
		}
		// Enemy
		if (showPhase >= 2)
		{
			{
				enemyRBar->Submit(Chan::ui);
				enemyGBar->Submit(Chan::ui);
				enemyBBar->Submit(Chan::ui);
				enemyR->Submit(Chan::ui);
				enemyG->Submit(Chan::ui);
				enemyB->Submit(Chan::ui);
			}
		}
		// Weapon
		if (showPhase >= 3)
		{
			weaponRBar->Submit(Chan::ui);
			weaponGBar->Submit(Chan::ui);
			weaponBBar->Submit(Chan::ui);
			weaponR->Submit(Chan::ui);
			weaponG->Submit(Chan::ui);
			weaponB->Submit(Chan::ui);
		}
		// Combo
		if (showPhase >= 4)
		{
			comboR->Submit(Chan::ui);
			comboG->Submit(Chan::ui);
			comboB->Submit(Chan::ui);
			for (auto& n : comboRNum)
			{
				n->Submit(Chan::ui);
			}
			for (auto& n : comboGNum)
			{
				n->Submit(Chan::ui);
			}
			for (auto& n : comboBNum)
			{
				n->Submit(Chan::ui);
			}
		}
		// Output
		if (showPhase >= 5)
		{
			for (auto& n : outputNum)
			{
				n->Submit(Chan::ui);
			}
		}
		// Input
		if (showPhase >= 6)
		{
			for (auto& n : inputNum)
			{
				n->Submit(Chan::ui);
			}
		}
		// Total
		if (showPhase >= 7)
		{
			for (auto& n : totalNum)
			{
				n->Submit(Chan::ui);
			}
		}
		// Button
		if (showPhase >= 8)
		{
			backButton->Submit();
		}
	}

	void Reset(void)
	{
		showPhase = 0;
		auto data = GameStatsCodex::Get();
		// Clear
		pClear->SetFrame(1, 1, 1, 1, 1, data.gameClear ? 0 : 1);
		// Score
		scoreDraw = 0.0f;
		// Time
		timeDraw = 0.0f;
		// Enemy
		{
			const int total = data.totalDefeat;
			const float invTotal = (total > 0) ? (1.0f / (float)total) : 0.0f;
			enemyRDraw = enemyGDraw = enemyBDraw = 0.0f;
			enemyRTarget = (float)data.rDefeat * invTotal;
			enemyGTarget = (float)data.gDefeat * invTotal;
			enemyBTarget = (float)data.bDefeat * invTotal;
		}
		// Weapon
		{
			const int total = data.totalWeapon;
			const float invTotal = (total > 0) ? (1.0f / (float)total) : 0.0f;
			weaponRDraw = weaponGDraw = weaponBDraw = 0.0f;
			weaponRTarget = (float)data.rWeapon * invTotal;
			weaponGTarget = (float)data.gWeapon * invTotal;
			weaponBTarget = (float)data.bWeapon * invTotal;
		}
		// Combo
		comboRDraw = comboGDraw = comboBDraw = 0.0f;
		// Output
		outputDraw = 0.0f;
		// Input
		inputDraw = 0.0f;
		// Total
		totalDraw = 0.0f;
		totalScore += data.gameClear ? 3000.0f : 0.0f;
		totalScore += (float)data.score;
		totalScore += data.lifeTime * 10.0f;
		totalScore += (float)data.totalWeapon * 10.0f;
		totalScore += std::max(std::max(data.rHighestCombo, data.gHighestCombo), data.bHighestCombo) * 100.0f;
		totalScore += data.outputDamage * 5.0f;
		totalScore += data.inputDamage * 10.0f;
		int totalInt = (int)(totalScore + 0.5f);
		if (totalInt < 0) totalInt = 0;
		RebuildTotalDigits(totalScore);
		// Button
		backButton->Reset();
		toTitle = false;
	}

	void RebuildTotalDigits(int value)
	{
		totalNum.clear();
		// digits count
		int v = std::max(0, value);
		int digits = 1;
		while (v >= 10)
		{
			v /= 10;
			++digits;
		}

		// layout params
		const float angleDeg = -15.0f;
		const float angleRad = angleDeg * PI / 180.0f;
		const float step = 50.0f;
		const float sx = std::cos(angleRad) * step;
		const float sy = std::sin(angleRad) * step;

		// center point
		const float baseX = 1030.0f;
		const float baseY = 550.0f;

		// center index -> integer for odd digits, 0.5 for even digits
		const float center = (digits - 1) * 0.5f;

		totalNum.reserve(digits);
		for (int i = 0; i < digits; ++i)
		{
			const float t = (float)i - center;
			const float x = baseX + sx * t;
			const float y = baseY + sy * t;

			auto sp = std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\UI_Result\\Number_No_2.png" });

			sp->SetPosition(x, y);
			sp->SetRotation(angleDeg);
			sp->SetScale(100.0f, 100.0f);
			sp->LinkTechniques(rg);

			totalNum.push_back(std::move(sp));
		}
	}

	bool GetToTitle() const { return toTitle; }

private:
	void SetToTitle(bool state) { toTitle = state; }

private:
	Graphics& gfx;
	Rgph::RenderGraph& rg;
	int showPhase{ 0 };
	// Background
	std::unique_ptr<Sprite2D> pBackground;
	// Clear
	std::unique_ptr<Sprite2D> pClear;
	// Score
	static constexpr int scoreDigitsNum = 8;
	float scoreDraw{ 0 };
	std::vector<std::unique_ptr<Sprite2D>> scoreNum;
	// Time
	static constexpr int timeDigitsNum = 4;
	float timeDraw{ 0.0f };
	std::vector<std::unique_ptr<Sprite2D>> timeNum;
	std::unique_ptr<Sprite2D> pColon;
	// Enemy
	std::unique_ptr<Sprite2D> enemyR;
	std::unique_ptr<Sprite2D> enemyG;
	std::unique_ptr<Sprite2D> enemyB;
	float enemyRTarget{ 0.0f };
	float enemyGTarget{ 0.0f };
	float enemyBTarget{ 0.0f };
	float enemyRDraw{ 0.0f };
	float enemyGDraw{ 0.0f };
	float enemyBDraw{ 0.0f };
	std::unique_ptr<Sprite2DPolygon> enemyRBar;
	std::unique_ptr<Sprite2DPolygon> enemyGBar;
	std::unique_ptr<Sprite2DPolygon> enemyBBar;
	// Weapon
	std::unique_ptr<Sprite2D> weaponR;
	std::unique_ptr<Sprite2D> weaponG;
	std::unique_ptr<Sprite2D> weaponB;
	float weaponRTarget{ 0.0f };
	float weaponGTarget{ 0.0f };
	float weaponBTarget{ 0.0f };
	float weaponRDraw{ 0.0f };
	float weaponGDraw{ 0.0f };
	float weaponBDraw{ 0.0f };
	std::unique_ptr<Sprite2DPolygon> weaponRBar;
	std::unique_ptr<Sprite2DPolygon> weaponGBar;
	std::unique_ptr<Sprite2DPolygon> weaponBBar;
	// Combo
	std::unique_ptr<Sprite2D> comboR;
	std::unique_ptr<Sprite2D> comboG;
	std::unique_ptr<Sprite2D> comboB;
	static constexpr int comboDigitsNum = 2;
	float comboRDraw{ 0.0f };
	float comboGDraw{ 0.0f };
	float comboBDraw{ 0.0f };
	std::vector<std::unique_ptr<Sprite2D>> comboRNum;
	std::vector<std::unique_ptr<Sprite2D>> comboGNum;
	std::vector<std::unique_ptr<Sprite2D>> comboBNum;
	// Output
	static constexpr int outputDigitsNum = 4;
	float outputDraw{ 0 };
	std::vector<std::unique_ptr<Sprite2D>> outputNum;
	// Input
	static constexpr int inputDigitsNum = 4;
	float inputDraw{ 0 };
	std::vector<std::unique_ptr<Sprite2D>> inputNum;
	// Total
	float totalScore{ 0.0f };
	float totalDraw{ 0.0f };
	std::vector<std::unique_ptr<Sprite2D>> totalNum;
	// Button
	bool toTitle{ false };
	std::unique_ptr<Button> backButton;
};