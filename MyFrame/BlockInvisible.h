#pragma once
#include "ObjectBase.h"

class BlockInvisible : public ObjectBase
{
public:
	BlockInvisible(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag, XMFLOAT3 position, XMFLOAT3 size, bool onCollision)
		:
		ObjectBase(tag)
	{
		// parameters init
		SetPosition(position);
		SetSize(size);
		SetCollisionSize(size);
		SetCollisionOnOff(onCollision);

		// collider init
		DirectX::XMFLOAT3 localHalf{ 0.5f, 0.5f, 0.5f };
		boxCollider = BoxCollider::BuildFromWorldMatrix(transInfo.GetWorldMatrix(), localHalf);
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
};