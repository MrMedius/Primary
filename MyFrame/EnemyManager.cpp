#include "EnemyManager.h"
#include "ObjectCodex.h"
#include <random>

#include "InputCodex.h"
#include "SoundCodex.h"
#include "GameStatsCodex.h"

#include "Enemy_Red_0.h"
#include "Enemy_Red_1.h"
#include "Enemy_Green_0.h"
#include "Enemy_Green_1.h"
#include "Enemy_Blue_0.h"
#include "Enemy_Blue_1.h"
//#include "EnemyBoss.h"

EnemyManager::EnemyManager(Graphics& gfx, Rgph::RenderGraph& rg)
	:
	gfx(gfx),
	rg(rg)
{
	auto mapSize = ObjectCodex::FindFirstActiveObjectByTag<ObjectBase>(environment_BlockTiled)->GetSize();
	MapEdgeLeftAndRight = mapSize.x / 2.0f;
	MapEdgeFrontAndBack = mapSize.z / 2.0f;
	for (int i = 0;i < 10;i++)
	{
		ObjectCodex::Acquire<Enemy_Red_0>(character_Enemy_Red_0, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Enemy_Red_1>(character_Enemy_Red_1, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Enemy_Green_0>(character_Enemy_Green_0, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Enemy_Green_1>(character_Enemy_Green_1, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Enemy_Blue_0>(character_Enemy_Blue_0, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Enemy_Blue_1>(character_Enemy_Blue_1, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
	}
	for (int i = 0;i < 10;i++)
	{
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Red_0>(character_Enemy_Red_0)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Red_1>(character_Enemy_Red_1)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Green_0>(character_Enemy_Green_0)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Green_1>(character_Enemy_Green_1)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Blue_0>(character_Enemy_Blue_0)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Enemy_Blue_1>(character_Enemy_Blue_1)->Deactivate();
	}
}

void EnemyManager::Update(float dt, XMFLOAT3 position)
{

#ifdef _DEBUG
	if (CountDown < Interval) CountDown++;
	if (CountDown >= Interval)
	{
		CountDown = 0;
		NewEnemy = true;
	}
	int type = 0;
	//if (NewEnemy)
	//{
	// // get new enemy type randomly
	//	std::uniform_int_distribution<int> enemyType(1, 6);
	//	type = enemyType(rng);
	// 
	// // get position around the player randomly
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> d(-30.0f, 30.0f);
	XMFLOAT3 pos = { position.x + d(rng),10.0f,position.z + d(rng) };
	if (pos.x > MapEdgeLeftAndRight) pos.x = MapEdgeLeftAndRight - 20.0f;
	if (pos.x < -MapEdgeLeftAndRight) pos.x = -MapEdgeLeftAndRight + 20.0f;
	if (pos.z > MapEdgeFrontAndBack) pos.z = MapEdgeFrontAndBack - 20.0f;
	if (pos.z < -MapEdgeFrontAndBack) pos.z = -MapEdgeFrontAndBack + 20.0f;

	//	NewEnemy = false;
	//}

	// 新しEnemyTestを生成する
	if (InputCodex::Get().KeyTriggered(KK_NUMPAD1) || type == 1) { enemies.push_back(ObjectCodex::Acquire<Enemy_Red_0>(character_Enemy_Red_0, gfx, rg, pos));	  Created = true; }
	if (InputCodex::Get().KeyTriggered(KK_NUMPAD4) || type == 4) { enemies.push_back(ObjectCodex::Acquire<Enemy_Red_1>(character_Enemy_Red_1, gfx, rg, pos));	  Created = true; }
	if (InputCodex::Get().KeyTriggered(KK_NUMPAD2) || type == 2) { enemies.push_back(ObjectCodex::Acquire<Enemy_Green_0>(character_Enemy_Green_0, gfx, rg, pos)); Created = true; }
	if (InputCodex::Get().KeyTriggered(KK_NUMPAD5) || type == 5) { enemies.push_back(ObjectCodex::Acquire<Enemy_Green_1>(character_Enemy_Green_1, gfx, rg, pos)); Created = true; }
	if (InputCodex::Get().KeyTriggered(KK_NUMPAD3) || type == 3) { enemies.push_back(ObjectCodex::Acquire<Enemy_Blue_0>(character_Enemy_Blue_0, gfx, rg, pos));	  Created = true; }
	if (InputCodex::Get().KeyTriggered(KK_NUMPAD6) || type == 6) { enemies.push_back(ObjectCodex::Acquire<Enemy_Blue_1>(character_Enemy_Blue_1, gfx, rg, pos));	  Created = true; }
	if (Created) 
	{
		Created = false;
		enemies.back()->SpawnAt(pos);
		SoundCodex::Get().PlaySE3D(SndPath::SE_Enemy_Create, 0, 1.0f, pos.x, pos.y, pos.z);
	}
#else
	if (CountDown < Interval) CountDown++;
	if (CountDown >= Interval)
	{
		CountDown = 0;
		NewEnemy = true;
	}
	int type = 0;
	if (NewEnemy)
	{
		std::mt19937 rng(std::random_device{}());

		// get new enemy type randomly
		std::uniform_int_distribution<int> enemyType(1, 6);
		type = enemyType(rng);

		// get position around the player randomly
		std::uniform_real_distribution<float> d(-30.0f, 30.0f);
		XMFLOAT3 pos = { position.x + d(rng),10.0f,position.z + d(rng) };
		if (pos.x > MapEdgeLeftAndRight) pos.x = MapEdgeLeftAndRight - 20.0f;
		if (pos.x < -MapEdgeLeftAndRight) pos.x = -MapEdgeLeftAndRight + 20.0f;
		if (pos.z > MapEdgeFrontAndBack) pos.z = MapEdgeFrontAndBack - 20.0f;
		if (pos.z < -MapEdgeFrontAndBack) pos.z = -MapEdgeFrontAndBack + 20.0f;

		NewEnemy = false;

		// 新しEnemyTestを生成する
		if (InputCodex::Get().KeyTriggered(KK_NUMPAD1) || type == 1) { enemies.push_back(ObjectCodex::Acquire<Enemy_Red_0>(character_Enemy_Red_0, gfx, rg, pos));	  Created = true; }
		if (InputCodex::Get().KeyTriggered(KK_NUMPAD4) || type == 4) { enemies.push_back(ObjectCodex::Acquire<Enemy_Red_1>(character_Enemy_Red_1, gfx, rg, pos));	  Created = true; }
		if (InputCodex::Get().KeyTriggered(KK_NUMPAD2) || type == 2) { enemies.push_back(ObjectCodex::Acquire<Enemy_Green_0>(character_Enemy_Green_0, gfx, rg, pos)); Created = true; }
		if (InputCodex::Get().KeyTriggered(KK_NUMPAD5) || type == 5) { enemies.push_back(ObjectCodex::Acquire<Enemy_Green_1>(character_Enemy_Green_1, gfx, rg, pos)); Created = true; }
		if (InputCodex::Get().KeyTriggered(KK_NUMPAD3) || type == 3) { enemies.push_back(ObjectCodex::Acquire<Enemy_Blue_0>(character_Enemy_Blue_0, gfx, rg, pos));	  Created = true; }
		if (InputCodex::Get().KeyTriggered(KK_NUMPAD6) || type == 6) { enemies.push_back(ObjectCodex::Acquire<Enemy_Blue_1>(character_Enemy_Blue_1, gfx, rg, pos));	  Created = true; }

		if (Created)
		{
			Created = false;
			enemies.back()->SpawnAt(pos);
			SoundCodex::Get().PlaySE3D(SndPath::SE_Enemy_Create, 0, 1.0f, pos.x, pos.y, pos.z);
		}
	}
#endif

	for (int i = 0; i < enemies.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (enemies[i]->IsActive())
			enemies[i]->Update(dt);
		else
		{
			GameStatsCodex::AddTotalDefeat();
			switch (enemies[i]->GetEnemyType())
			{
			case ENEMY_TYPE_RED:	GameStatsCodex::AddRedDefeat();		break;
			case ENEMY_TYPE_GREEN:	GameStatsCodex::AddGreenDefeat();	break;
			case ENEMY_TYPE_BLUE:	GameStatsCodex::AddBlueDefeat();	break;
			}
			GameStatsCodex::AddScore(enemies[i]->GetKillScore());

			enemies.erase(enemies.begin() + i);
			--i;
		}
	}
	//if (pEnemyBoss->IsActive()) pEnemyBoss->Update();
}

void EnemyManager::Submit(void)
{
	for (int i = 0; i < enemies.size(); i++)
		if (enemies[i]->IsActive())
			enemies[i]->Submit();
	//if (pEnemyBoss->IsActive()) pEnemyBoss->Draw();
}

void EnemyManager::Reset(void)
{
	for (int i = 0; i < enemies.size(); i++)
		if (enemies[i]->IsActive())
			enemies[i]->Deactivate();
	enemies.clear();
}