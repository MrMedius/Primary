#pragma once
#include "Effect.h"
#include "ObjectCodex.h"
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "RenderGraph.h"
#include "Sprite3DNoLit.h"
#include "AnimationInfo.h"
#include "Channels.h"

class Effect_Player_Remote_1 : public Effect
{
public:
	Effect_Player_Remote_1(Graphics& gfx, Rgph::RenderGraph& rg, XMFLOAT3 position, Object_Type_Tag tag = effect_Player_Remote_Attack_1)
		:
		Effect(tag)
	{
		pPlayer = ObjectCodex::FindActiveObjectsByTag<Character>(character_Player)[0];

		// parameters init
		SetPosition(position);
		SetSize({ 3.0f,3.0f,0.0f });
		SetCollisionSize({ 3.0f,1.0f,3.0f });
		SetCollisionOnOff(true);
		SetMoveAccel(0.03f);
		SetMoveAccel((pPlayer->GetIsFlip() ? -1.0f : 1.0f) * speedNoPlay);
		SetDoAttackType(Player_Attack_1);
		SetEffectState(Appear);
		lastTime = 0.0f;
		lifeTime = 0.5f;

		// graphics init
		pack.push_back(SpriteAnimeInfo{ 12,14,25,18 });
		pack.push_back(SpriteAnimeInfo{ 12,14,40,3 });
		pack.push_back(SpriteAnimeInfo{ 12,14,43,6 });
		visualPre = std::make_unique<Sprite3DNoLit>(gfx, std::vector<std::string>{"asset\\Images\\\Player\\Player_Green_Effect_Attack.png"});
		visualPre->SetFrameAuto(pack[0].numU, pack[0].numV, pack[0].FrameStart, pack[0].FrameTotalCount, 0, pack[0].FPS, !pPlayer->GetIsFlip(), false);
		visualPre->SetPosition(transInfo.position);
		visualPre->SetRotation(90.0f, 0.0f, 0.0f);
		visualPre->SetScale(transInfo.scale);
		visualPre->LinkTechniques(rg);

		// collider init
#ifdef _DEBUG
		boxColliderWire = std::make_unique<CubeWireframe>(gfx, XMFLOAT3(0.0f, 0.0f, 1.0f));
		boxColliderWire->LinkTechniques(rg);
#endif
	}
	void OnEnable(void) override
	{
		// reset parameters
		SetPosition(pPlayer->GetAttackPosition());
		SetCollisionOnOff(true);
		boxCollider.center = transInfo.position;
		SetMoveAccel((pPlayer->GetIsFlip() ? -1.0f : 1.0f) * speedNoPlay);
		SetEffectState(Appear);
		ResetMoveVelocity();
		lastTime = 0.0f;

		// reset animation
		visualPre->SetFrameAuto(pack[0].numU, pack[0].numV, pack[0].FrameStart, pack[0].FrameTotalCount, 0, pack[0].FPS, !pPlayer->GetIsFlip(), false);
		visualPre->SetPosition(transInfo.position);
	}
	void Update(float dt) override
	{
		// anime update
		visualPre->Update(dt);
		switch (effectState)
		{
		case Appear:
			if(visualPre->ClipFinished())
			{
				SetEffectState(Play);
				SetMoveAccel((pPlayer->GetIsFlip() ? -1.0f : 1.0f) * speedPlay);
				visualPre->SetFrameAuto(pack[1].numU, pack[1].numV, pack[1].FrameStart, pack[1].FrameTotalCount, 0, pack[1].FPS, !pPlayer->GetIsFlip(), true);
			}
			break;
		case Play:
			lastTime +=dt;
			if (lastTime >= lifeTime && visualPre->GetCurrentFrame() == 3)
			{
				SetEffectState(Disappear);
				SetMoveAccel(GetMoveAccel() / 10.0f);
				SetCollisionOnOff(false);
			}
			break;
		case Disappear:
			if (visualPre->ClipFinished())
			{
				Deactivate();
			}
			break;
		}

		// move update
		CalculateMoveVelocity(GetMoveAccel(), 0.0f, 0.0f);
		Transform(MoveVelocity.x, MoveVelocity.y, MoveVelocity.z);
		visualPre->SetPosition(transInfo.position);

		// collider update
		boxCollider.center = transInfo.position;
	}
	void Submit(void) override
	{
		visualPre->Submit(Chan::main);
#ifdef _DEBUG
		boxColliderWire->DoSubmit(transInfo.position, boxCollider.GetSize());
#endif
	}
	void OnCollide(Character* other) override
	{
		SetEffectState(Disappear);

		if (auto* e = dynamic_cast<Enemy*>(other))
		{
			if(e->GetBeAttackedType() != GetDoAttackType())
			{
				e->SetIsHurt(true);	// çUåÇÇ≥ÇÍÇΩèÛë‘Ç…ëJà⁄
				e->CalculateHpCurrent(-1.0f); // ëÃóÕåvéZ
				e->SetBeAttackedType(GetDoAttackType());

				float dx = e->GetPosition().x - transInfo.position.x;
				float dz = e->GetPosition().z - transInfo.position.z;
				float angle = atan2f(dx, dz);
				XMFLOAT3 repel{ 0.3f,0.1f,0.1f };
				other->CalculateMoveVelocity(sinf(angle) * repel.x, repel.y, cosf(angle) * repel.z); // åÇëﬁÇ∑ÇÈ

				// ÉQÅ[ÉÄÉäÉ\Å[ÉXîªíf(FeverÇ∂Ç·Ç»Ç¢éûÇæÇØÉGÉlÉãÉMÅ[Ç™Ç‡ÇÁÇ¶ÇÈ)
				auto p = ObjectCodex::FindFirstActiveObjectByTag<Player>(character_Player);
				p->GetResourceBars()->AttackSettlement(e->GetEnemyType(), p->GetIsFever());
				p->AttackCameraShake(4, 0.3f, 0.5f);
			}
		}
	}
private:
	std::unique_ptr<Sprite3DNoLit> visualPre;
	Character* pPlayer;
	std::vector<SpriteAnimeInfo> pack;
	static constexpr float speedNoPlay = 0.005f;
	static constexpr float speedPlay = speedNoPlay * 10.0f;
#ifdef _DEBUG
	std::unique_ptr<CubeWireframe> searchColliderWire;
#endif
};