#pragma once
#include "ObjectBase.h"
#include "RenderGraph.h"
#include "CubeBase.h"
#include "Channels.h"

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
	}
	void OnEnable(void) override {}
	void Update(float dt) override {}
	void Submit(void) override
	{
		visualPre.Submit(Chan::main);
		visualPre.Submit(Chan::shadow);

#ifdef _DEBUG
		//Collision::DrawBoxWireFrame(Position, CollisionSize, pGfx, pCamera, Colors::Red);
#endif
	}
	CubeBase visualPre;
};

