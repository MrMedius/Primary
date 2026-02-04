#pragma once
#include "Graphics.h"
#include "RenderGraph.h"
#include "Keyboard.h"
#include "Score.h"

#include "Enemy_Red_0.h"
#include "Enemy_Red_1.h"
#include "Enemy_Green_0.h"
#include "Enemy_Blue_0.h"
#include "Enemy_Blue_1.h"
//#include "EnemyBoss.h"

class EnemyManager
{
public:
	EnemyManager(Graphics& gfx, Rgph::RenderGraph& rg, Keyboard* kbd, Score* score);
	~EnemyManager() = default;
	void Update(float dt, XMFLOAT3 position);
	void Submit(void);
	void Reset(void);
private:
	Graphics& gfx;
	Rgph::RenderGraph& rg;
	Keyboard* pKbd;
	Score* pScore;

	std::vector<Enemy_Red_0*>	EnemyRed0;
	std::vector<Enemy_Red_1*>	EnemyRed1;
	std::vector<Enemy_Green_0*> EnemyGreen0;
	std::vector<Enemy_Blue_0*>	EnemyBlue0;
	std::vector<Enemy_Blue_1*>	EnemyBlue1;
	//EnemyBoss* pEnemyBoss;

	int Interval{ 100 };
	int CountDown{ 0 };
	bool NewEnemy{ false };

private:
	float MapEdgeLeftAndRight{ 0.0f };
	float MapEdgeFrontAndBack{ 0.0f };
};