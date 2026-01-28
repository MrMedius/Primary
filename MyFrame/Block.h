#pragma once
#include "ObjectBase.h"
#include "RenderGraph.h"
#include "CubeBlock.h"
#include "Channels.h"
#include "Surface.h"

class Block : public ObjectBase
{
public:
	Block(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag, XMFLOAT3 position, XMFLOAT3 size, XMFLOAT3 collisionSize, bool onCollision)
		:
		ObjectBase(tag),
		visualPre(gfx, size)
	{
		// parameters init
		SetPosition(position);
		SetSize(size);
		SetCollisionSize(collisionSize);
		SetCollisionOnOff(onCollision);

		// graphics init
		visualPre.LinkTechniques(rg);
		visualPre.SetPosition(transInfo.position);

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
		visualPre.Submit(Chan::main);
		visualPre.Submit(Chan::shadow);
#ifdef _DEBUG
		//boxColliderWire->DoSubmit(transInfo.position, boxCollider.GetSize());
#endif
	}
	CubeBase visualPre;
};

