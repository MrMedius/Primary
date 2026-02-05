#pragma once
#include "Graphics.h"
#include "RenderGraph.h"
#include "Player.h"

#include "Effect_Player_Remote_1.h"
#include "Effect_Player_Remote_2.h"
#include "Effect_Player_Remote_3.h"

class EffectManager
{
public:
	EffectManager(Graphics& gfx, Rgph::RenderGraph& rg);
	~EffectManager() = default;
	void Update(float dt);
	void Submit(void);
	void Reset(void);
private:
	Graphics& gfx;
	Rgph::RenderGraph& rg;

	std::vector<Effect_Player_Remote_1*> EffectPlayerRemote1;
	std::vector<Effect_Player_Remote_2*> EffectPlayerRemote2;
	std::vector<Effect_Player_Remote_3*> EffectPlayerRemote3;

	Player* pPlayer;
	Attack_Type_Tag playerAttackType{ Attack_Type_None };
};