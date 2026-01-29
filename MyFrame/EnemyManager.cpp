#include "EnemyManager.h"
#include "ObjectCodex.h"
#include <random>

EnemyManager::EnemyManager(Graphics& gfx, Rgph::RenderGraph& rg, Keyboard* kbd)//, Score* score)
	:
	gfx(gfx),
	rg(rg),
	pKbd(kbd)
	//pScore(score)
{}

void EnemyManager::Update(float dt, XMFLOAT3 position)
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> x(-15, 15);
	std::uniform_real_distribution<float> z(-5, 5);
	XMFLOAT3 pos = { position.x + x(rng),20.0f,position.z + z(rng) };


	if (CountDown < Interval) CountDown++;
	if (CountDown >= Interval)
	{
		CountDown = 0;
		NewEnemy = true;
	}
	int type = 0;
	//if (NewEnemy)
	//{
	//	std::uniform_int_distribution<int> enemyType(1, 3);
	//	type = enemyType(rng);
	//	NewEnemy = false;
	//}

	#ifdef _DEBUG
	assert(pKbd);
	// 新しEnemyTestを生成する
	if (pKbd->KeyIsTriggered(KK_NUMPAD1) || type == 1)
		EnemyTests.push_back(ObjectCodex::Acquire<EnemyTest>(character_Enemy_Test, gfx, rg, character_Enemy_Test, pos));
	if (pKbd->KeyIsTriggered(KK_NUMPAD2) || type == 2)
		EnemyTestsTwo.push_back(ObjectCodex::Acquire<EnemyTestTwo>(character_Enemy_Test_Two, gfx, rg, character_Enemy_Test_Two, pos));
	if (pKbd->KeyIsTriggered(KK_NUMPAD3) || type == 3)
		EnemyTestsThree.push_back(ObjectCodex::Acquire<EnemyTestThree>(character_Enemy_Test_Three, gfx, rg, character_Enemy_Test_Three, pos));
	#endif

	for (int i = 0; i < EnemyTests.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EnemyTests[i]->IsActive())
			EnemyTests[i]->Update(dt);
		else
		{
			EnemyTests.erase(EnemyTests.begin() + i);
			//pScore->CalculateScore(80);
		}
	}
	for (int i = 0; i < EnemyTestsTwo.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EnemyTestsTwo[i]->IsActive())
			EnemyTestsTwo[i]->Update(dt);
		else
		{
			EnemyTestsTwo.erase(EnemyTestsTwo.begin() + i);
			//pScore->CalculateScore(100);
		}
	}
	for (int i = 0; i < EnemyTestsThree.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EnemyTestsThree[i]->IsActive())
			EnemyTestsThree[i]->Update(dt);
		else
		{
			EnemyTestsThree.erase(EnemyTestsThree.begin() + i);
			//pScore->CalculateScore(50);
		}
	}

	//if (pEnemyBoss->IsActive()) pEnemyBoss->Update();
}

void EnemyManager::Submit(void)
{
	for (int i = 0; i < EnemyTests.size(); i++)
		if (EnemyTests[i]->IsActive())		EnemyTests[i]->Submit();
	for (int i = 0; i < EnemyTestsTwo.size(); i++)
		if (EnemyTestsTwo[i]->IsActive())	EnemyTestsTwo[i]->Submit();
	for (int i = 0; i < EnemyTestsThree.size(); i++)
		if (EnemyTestsThree[i]->IsActive())	EnemyTestsThree[i]->Submit();

	//if (pEnemyBoss->IsActive()) pEnemyBoss->Draw();
}

void EnemyManager::Reset(void)
{
	EnemyTests.clear();
	EnemyTestsTwo.clear();
	EnemyTestsThree.clear();
}