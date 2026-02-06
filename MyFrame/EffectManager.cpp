#include "EffectManager.h"
#include "ObjectCodex.h"
#include <random>

#include "Enemy.h"
#include "Effect_Player_Remote_1.h"
#include "Effect_Player_Remote_2.h"
#include "Effect_Player_Remote_3.h"
#include "Effect_Player_Hit.h"

EffectManager::EffectManager(Graphics& gfx, Rgph::RenderGraph& rg)
	:
	gfx(gfx),
	rg(rg)
{
	pPlayer = ObjectCodex::FindFirstActiveObjectByTag<Player>(character_Player);

	for (int i = 0;i < 30;i++)
	{
		ObjectCodex::Acquire<Effect_Player_Remote_1>(effect_Player_Remote_Attack_1, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f }, false);
		ObjectCodex::Acquire<Effect_Player_Remote_2>(effect_Player_Remote_Attack_2, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f }, false);
		ObjectCodex::Acquire<Effect_Player_Remote_3>(effect_Player_Remote_Attack_3, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f }, false);
		ObjectCodex::Acquire<Effect_Player_Hit>(effect_Player_Hit, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f }, false);
	}
	for (int i = 0;i < 30;i++)
	{
		ObjectCodex::FindFirstActiveObjectByTag<Effect_Player_Remote_1>(effect_Player_Remote_Attack_1)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Effect_Player_Remote_2>(effect_Player_Remote_Attack_2)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Effect_Player_Remote_3>(effect_Player_Remote_Attack_3)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Effect_Player_Hit>(effect_Player_Hit)->Deactivate();
	}
}

void EffectManager::Update(float dt)
{
	if (pPlayer->GetIsAttack())
	{
		// create player's remote attack effect
		if (pPlayer->GetWeaponSlots()->GetCurrentSlot().GetType() == WEAPON_TYPE_2)
		{
			auto type = pPlayer->GetDoAttackType();

			if (type != Attack_Type_None && playerAttackType != type)
			{
				playerAttackType = type;
				auto pos = pPlayer->GetAttackPosition();
				auto flip = pPlayer->GetIsFlip();

				switch (playerAttackType)
				{
				case Player_Attack_1: effects.push_back(ObjectCodex::Acquire<Effect_Player_Remote_1>(effect_Player_Remote_Attack_1, gfx, rg, pos, flip)); break;
				case Player_Attack_2: effects.push_back(ObjectCodex::Acquire<Effect_Player_Remote_2>(effect_Player_Remote_Attack_2, gfx, rg, pos, flip)); break;
				case Player_Attack_3: effects.push_back(ObjectCodex::Acquire<Effect_Player_Remote_3>(effect_Player_Remote_Attack_3, gfx, rg, pos, flip)); break;
				}
				effects.back()->SpawnAt(pos, flip);
			}
		}

		// create player's hit effect
		if (pPlayer->GetWeaponSlots()->GetCurrentSlot().GetType() == WEAPON_TYPE_3)
		{
			if (pPlayer->GetCollisionOnOff())
			{
				std::vector<Enemy*> enemies;
				for (auto tag : {
					character_Enemy_Red_0,
					character_Enemy_Red_1,
					character_Enemy_Green_0,
					character_Enemy_Green_1,
					character_Enemy_Blue_0,
					character_Enemy_Blue_1
					}) {
					// プレイヤーを攻撃できる全ての敵を探す
					auto found = ObjectCodex::FindActiveObjectsByTag<Enemy>(tag);
					// 全ての敵を整理する
					enemies.reserve(enemies.size() + found.size());
					enemies.insert(enemies.end(), found.begin(), found.end());
				}

				for (auto e : enemies)
				{
					if (e->GetIsHurt() && e->GetWasHurt())
					{
						auto pos = e->GetPosition();
						pos = { pos.x,pos.y, pos.z - 0.1f };
						auto flip = e->GetIsFlip();
						effects.push_back(ObjectCodex::Acquire<Effect_Player_Hit>(effect_Player_Hit, gfx, rg, pos, flip));
						effects.back()->SpawnAt(pos, flip);
					}
				}
			}
		}
	}
	else
	{
		playerAttackType = Attack_Type_None;
	}

	// update all effects
	for (int i = 0; i < effects.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (effects[i]->IsActive())
			effects[i]->Update(dt);
		else
			effects.erase(effects.begin() + i);
	}
}

void EffectManager::Submit(void)
{
	for (int i = 0; i < effects.size(); i++)
		if (effects[i]->IsActive())		
			effects[i]->Submit();
}

void EffectManager::Reset(void)
{
	for (int i = 0; i < effects.size(); i++)
		if (effects[i]->IsActive())		
			effects[i]->Deactivate();

	effects.clear();
}