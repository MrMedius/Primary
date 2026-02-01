#include "EnemyManager.h"
#include "ObjectCodex.h"
#include <random>

EnemyManager::EnemyManager(Graphics& gfx, Rgph::RenderGraph& rg, Keyboard* kbd, Score* score)
	:
	gfx(gfx),
	rg(rg),
	pKbd(kbd),
	pScore(score)
{
	for (int i = 0;i < 10;i++)
	{
		ObjectCodex::Acquire<Enemy_Red_0>(character_Enemy_Red_0, gfx, rg, character_Enemy_Red_0, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Enemy_Red_1>(character_Enemy_Red_1, gfx, rg, character_Enemy_Red_1, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Enemy_Green_0>(character_Enemy_Green_0, gfx, rg, character_Enemy_Green_0, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Enemy_Blue_0>(character_Enemy_Blue_0, gfx, rg, character_Enemy_Blue_0, XMFLOAT3{ 0.0f,0.0f,0.0f });
	}
	for (int i = 0;i < 10;i++)
	{
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Red_0>(character_Enemy_Red_0)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Red_1>(character_Enemy_Red_1)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Green_0>(character_Enemy_Green_0)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Blue_0>(character_Enemy_Blue_0)->Deactivate();
	}
}

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

	//#ifdef _DEBUG
	assert(pKbd);
	// 新しEnemyTestを生成する
	if (pKbd->KeyIsTriggered(KK_NUMPAD1) || type == 1)
		EnemyRed0.push_back(ObjectCodex::Acquire<Enemy_Red_0>(character_Enemy_Red_0, gfx, rg, character_Enemy_Red_0, pos));
	if (pKbd->KeyIsTriggered(KK_NUMPAD4) || type == 4)
		EnemyRed1.push_back(ObjectCodex::Acquire<Enemy_Red_1>(character_Enemy_Red_1, gfx, rg, character_Enemy_Red_1, pos));
	if (pKbd->KeyIsTriggered(KK_NUMPAD2) || type == 2)
		EnemyGreen0.push_back(ObjectCodex::Acquire<Enemy_Green_0>(character_Enemy_Green_0, gfx, rg, character_Enemy_Green_0, pos));
	if (pKbd->KeyIsTriggered(KK_NUMPAD3) || type == 3)
		EnemyBlue0.push_back(ObjectCodex::Acquire<Enemy_Blue_0>(character_Enemy_Blue_0, gfx, rg, character_Enemy_Blue_0, pos));
	//#endif

	for (int i = 0; i < EnemyRed0.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EnemyRed0[i]->IsActive())
			EnemyRed0[i]->Update(dt);
		else
		{
			EnemyRed0.erase(EnemyRed0.begin() + i);
			pScore->CalculateScore(40);
		}
	}
	for (int i = 0; i < EnemyRed1.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EnemyRed1[i]->IsActive())
			EnemyRed1[i]->Update(dt);
		else
		{
			EnemyRed1.erase(EnemyRed1.begin() + i);
			pScore->CalculateScore(80);
		}
	}
	for (int i = 0; i < EnemyGreen0.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EnemyGreen0[i]->IsActive())
			EnemyGreen0[i]->Update(dt);
		else
		{
			EnemyGreen0.erase(EnemyGreen0.begin() + i);
			pScore->CalculateScore(20);
		}
	}
	for (int i = 0; i < EnemyBlue0.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EnemyBlue0[i]->IsActive())
			EnemyBlue0[i]->Update(dt);
		else
		{
			EnemyBlue0.erase(EnemyBlue0.begin() + i);
			pScore->CalculateScore(50);
		}
	}

	//if (pEnemyBoss->IsActive()) pEnemyBoss->Update();
}

void EnemyManager::Submit(void)
{
	for (int i = 0; i < EnemyRed0.size(); i++)
		if (EnemyRed0[i]->IsActive())		EnemyRed0[i]->Submit();
	for (int i = 0; i < EnemyRed1.size(); i++)
		if (EnemyRed1[i]->IsActive())		EnemyRed1[i]->Submit();
	for (int i = 0; i < EnemyGreen0.size(); i++)
		if (EnemyGreen0[i]->IsActive())		EnemyGreen0[i]->Submit();
	for (int i = 0; i < EnemyBlue0.size(); i++)
		if (EnemyBlue0[i]->IsActive())	EnemyBlue0[i]->Submit();

	//if (pEnemyBoss->IsActive()) pEnemyBoss->Draw();
}

void EnemyManager::Reset(void)
{
	EnemyRed0.clear();
	EnemyRed1.clear();
	EnemyGreen0.clear();
	EnemyBlue0.clear();
}