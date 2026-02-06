#pragma once
#include "Effect.h"
#include "RenderGraph.h"
#include "Sprite3DNoLit.h"
#include "AnimationInfo.h"
#include "Channels.h"

class Effect_Player_Hit : public Effect
{
public:
	Effect_Player_Hit(Graphics& gfx, Rgph::RenderGraph& rg, XMFLOAT3 position, bool flip, Object_Type_Tag tag = effect_Player_Hit)
		:
		Effect(tag)
	{
		// parameters init
		SetPosition(position);
		SetSize({ 5.0f,5.0f,0.0f });
		SetCollisionOnOff(false);

		// graphics init
		visualPre = std::make_unique<Sprite3DNoLit>(gfx, std::vector<std::string>{"asset\\Images\\\Player\\Player_Blue_Effect_Attack.png"});
		visualPre->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 0, pack.FPS, flip, false);
		visualPre->SetPosition(transInfo.position);
		visualPre->SetScale(transInfo.scale);
		visualPre->LinkTechniques(rg);
	}
	void ActiveAt(XMFLOAT3 pos, bool flip) override 
	{	
		// activate
		Activate();

		// reset parameters
		SetPosition(pos);

		// reset animation
		visualPre->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 0, pack.FPS, flip, false);
		visualPre->SetPosition(transInfo.position);
	}
	void OnEnable(void) override {}
	void Update(float dt) override
	{
		// anime update
		visualPre->Update(dt);

		if(visualPre->ClipFinished())
		{
			Deactivate();
		}
	}
	void Submit(void) override
	{
		visualPre->Submit(Chan::main);
	}
	void OnCollide(Character* other) override {}
private:
	std::unique_ptr<Sprite3DNoLit> visualPre;
	SpriteAnimeInfo pack{ 12,13,145,5 };
};