#include "EffectManager.h"
#include "ObjectCodex.h"
#include <random>

EffectManager::EffectManager(Graphics& gfx, Rgph::RenderGraph& rg)
	:
	gfx(gfx),
	rg(rg)
{
	pPlayer = ObjectCodex::FindFirstActiveObjectByTag<Player>(character_Player);

	for (int i = 0;i < 30;i++)
	{
		ObjectCodex::Acquire<Effect_Player_Remote_1>(effect_Player_Remote_Attack_1, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Effect_Player_Remote_2>(effect_Player_Remote_Attack_2, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
		ObjectCodex::Acquire<Effect_Player_Remote_3>(effect_Player_Remote_Attack_3, gfx, rg, XMFLOAT3{ 0.0f,0.0f,0.0f });
	}
	for (int i = 0;i < 30;i++)
	{
		ObjectCodex::FindFirstActiveObjectByTag<Effect_Player_Remote_1>(effect_Player_Remote_Attack_1)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Effect_Player_Remote_2>(effect_Player_Remote_Attack_2)->Deactivate();
		ObjectCodex::FindFirstActiveObjectByTag<Effect_Player_Remote_3>(effect_Player_Remote_Attack_3)->Deactivate();
	}
}

void EffectManager::Update(float dt)
{
	// create player's remote attack effect
	if (pPlayer->GetIsAttack() && pPlayer->GetWeaponSlots()->GetCurrentSlot().GetType() == WEAPON_TYPE_2)
	{
		auto type = pPlayer->GetDoAttackType();
		if(playerAttackType != type)
		{
			playerAttackType = type;
			switch (playerAttackType)
			{
			case Player_Attack_1: EffectPlayerRemote1.push_back(ObjectCodex::Acquire<Effect_Player_Remote_1>(effect_Player_Remote_Attack_1, gfx, rg, pPlayer->GetAttackPosition())); break;
			case Player_Attack_2: EffectPlayerRemote2.push_back(ObjectCodex::Acquire<Effect_Player_Remote_2>(effect_Player_Remote_Attack_2, gfx, rg, pPlayer->GetAttackPosition())); break;
			case Player_Attack_3: EffectPlayerRemote3.push_back(ObjectCodex::Acquire<Effect_Player_Remote_3>(effect_Player_Remote_Attack_3, gfx, rg, pPlayer->GetAttackPosition())); break;
			}
		}
	}
	else
	{
		playerAttackType = Attack_Type_None;
	}

	for (int i = 0; i < EffectPlayerRemote1.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EffectPlayerRemote1[i]->IsActive())
			EffectPlayerRemote1[i]->Update(dt);
		else
		{
			EffectPlayerRemote1.erase(EffectPlayerRemote1.begin() + i);
		}
	}
	for (int i = 0; i < EffectPlayerRemote2.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EffectPlayerRemote2[i]->IsActive())
			EffectPlayerRemote2[i]->Update(dt);
		else
		{
			EffectPlayerRemote2.erase(EffectPlayerRemote2.begin() + i);
		}
	}
	for (int i = 0; i < EffectPlayerRemote3.size(); i++)
	{
		// 対象かアクティブでいれば更新する、そうでない場合は容器から削除する
		if (EffectPlayerRemote3[i]->IsActive())
			EffectPlayerRemote3[i]->Update(dt);
		else
		{
			EffectPlayerRemote3.erase(EffectPlayerRemote3.begin() + i);
		}
	}
}

void EffectManager::Submit(void)
{
	for (int i = 0; i < EffectPlayerRemote1.size(); i++)
		if (EffectPlayerRemote1[i]->IsActive())		EffectPlayerRemote1[i]->Submit();
	for (int i = 0; i < EffectPlayerRemote2.size(); i++)
		if (EffectPlayerRemote2[i]->IsActive())		EffectPlayerRemote2[i]->Submit();
	for (int i = 0; i < EffectPlayerRemote3.size(); i++)
		if (EffectPlayerRemote3[i]->IsActive())		EffectPlayerRemote3[i]->Submit();
}

void EffectManager::Reset(void)
{
	for (int i = 0; i < EffectPlayerRemote1.size(); i++)
		if (EffectPlayerRemote1[i]->IsActive())		EffectPlayerRemote1[i]->Deactivate();
	for (int i = 0; i < EffectPlayerRemote2.size(); i++)
		if (EffectPlayerRemote2[i]->IsActive())		EffectPlayerRemote2[i]->Deactivate();
	for (int i = 0; i < EffectPlayerRemote3.size(); i++)
		if (EffectPlayerRemote3[i]->IsActive())		EffectPlayerRemote3[i]->Deactivate();
	EffectPlayerRemote1.clear();
	EffectPlayerRemote2.clear();
	EffectPlayerRemote3.clear();
}