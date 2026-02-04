#pragma once
#include "Environment.h"
#include "Character.h"

class BlockInvisible : public Environment
{
public:
	BlockInvisible(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag, XMFLOAT3 position, XMFLOAT3 size, bool onCollision)
		:
		Environment(tag),
		renderG(rg)
	{
		// parameters init
		SetPosition(position);
		SetSize(size);
		SetCollisionSize(size);
		SetCollisionOnOff(onCollision);

		// collider init
		DirectX::XMFLOAT3 localHalf{ 0.5f, 0.5f, 0.5f };
		boxCollider = BoxCollider::BuildFromWorldMatrix(transInfo.GetWorldMatrix(), localHalf);
		BlockXorZ = size.x < size.z;
		BlockPorM = BlockXorZ ? position.x < 0.0f : position.z < 0.0f;
#ifdef _DEBUG
		boxColliderWire = std::make_unique<CubeWireframe>(gfx, XMFLOAT3(1.0f, 0.0f, 0.0f));
		boxColliderWire->LinkTechniques(rg);
#endif
	}
	void OnEnable(void) override {}
	void Update(float dt) override {}
	void Submit(void) override 
	{
#ifdef _DEBUG
		boxColliderWire->DoSubmit(GetPosition(), GetSize());
#endif
	}
	void OnCollide(Character* character) override 
	{
		float pushPower = (BlockPorM ? 1.0f : -1.0f) * 0.5f;

		XMFLOAT3 velocity = BlockXorZ ?
			XMFLOAT3{ pushPower, 0.0f, 0.0f } :
			XMFLOAT3{ 0.0f, 0.0f, pushPower };

		character->CalculateMoveVelocity(velocity);

		renderG.Interaction();
	}
private:
	bool BlockXorZ{ false };
	bool BlockPorM{ false };
	Rgph::RenderGraph& renderG;
};