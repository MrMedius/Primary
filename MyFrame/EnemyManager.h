#pragma once
#include "Graphics.h"
#include "RenderGraph.h"
#include "ObjectCodex.h"
#include "Keyboard.h"
//#include "Score.h"

#include "EnemyTest.h"
#include "EnemyTestTwo.h"
#include "EnemyTestThree.h"
//#include "EnemyBoss.h"

class EnemyManager
{
public:
	EnemyManager(Graphics& gfx, Rgph::RenderGraph& rg, Keyboard* kbd);//, Score* score);
	~EnemyManager() = default;
	void Update(float dt, XMFLOAT3 position);
	void Submit(void);
	void Reset(void);
private:
	Graphics& gfx;
	Rgph::RenderGraph& rg;
	Keyboard* pKbd;
	//Score* pScore;

	std::vector<EnemyTest*> EnemyTests;
	std::vector<EnemyTestTwo*> EnemyTestsTwo;
	std::vector<EnemyTestThree*> EnemyTestsThree;
	//EnemyBoss* pEnemyBoss;

	int Interval{ 100 };
	int CountDown{ 0 };
	bool NewEnemy{ false };
};