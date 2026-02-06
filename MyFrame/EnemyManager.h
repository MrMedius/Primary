#pragma once
#include "Graphics.h"
#include "RenderGraph.h"
#include "Keyboard.h"
#include "Score.h"

#include "Enemy.h"

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

	std::vector<Enemy*> enemies;

	//EnemyBoss* pEnemyBoss;

	int Interval{ 100 };
	int CountDown{ 0 };
	bool NewEnemy{ false };

private:
	float MapEdgeLeftAndRight{ 0.0f };
	float MapEdgeFrontAndBack{ 0.0f };
};