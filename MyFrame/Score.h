#pragma once
#include "Graphics.h"
#include "Sprite2D.h"
#include "Channels.h"

class Score
{
public:
	Score(Graphics& gfx, Rgph::RenderGraph& rg)
	{
		for (int i = 0; i < digitsNum; i++)
		{
			num.push_back(std::make_unique<Sprite2D>(gfx, std::vector<std::string>{ "asset\\Images\\Game_UI\\Number.png" }));
			num[i]->SetPosition(250.0f - 30.0f * i, 50.0f);
			num[i]->SetScale(50.0f, 50.0f);
			num[i]->LinkTechniques(rg);
		}
	}

	~Score() = default;

	void Update(void)
	{
		unsigned int divide = 5;
		// gradually add up the score
		if (ScoreCumulated > ScoreDraw) ScoreDraw += (ScoreCumulated - ScoreDraw) / divide;
		// ensure the ScoreDraw does not exceed the ScoreCumulated
		if (ScoreCumulated - ScoreDraw < divide) ScoreDraw = ScoreCumulated;

		int ScoreValue = ScoreDraw;
		for (int i = 0; i < digitsNum; i++)
		{
			int Num = ScoreValue % 10;
			ScoreValue /= 10;

			num[i]->SetFrame(5, 5, Num + 1, 1, 1, 0, false);
		}
	}

	void Submit(void)
	{
		for(auto& n : num)
		{
			n->Submit(Chan::ui);
		}
	}

	void CalculateScore(const int value) { ScoreCumulated += value; }
	void Reset(void)
	{
		ScoreCumulated = 0;
		ScoreDraw = 0;
	}
private:
	static constexpr int digitsNum = 8;
	std::vector<std::unique_ptr<Sprite2D>> num;
	unsigned int ScoreCumulated{ 0 };	// 溜まっているスコア
	unsigned int ScoreDraw{ 0 };		// 描く用のスコア
};