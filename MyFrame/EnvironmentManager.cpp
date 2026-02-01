#include "EnvironmentManager.h"
#include "ObjectCodex.h"

EnvironmentManager::EnvironmentManager(Graphics& gfx, Rgph::RenderGraph& rg)
	:
	gfx(gfx),
	rg(rg)
{
	blocks.push_back(ObjectCodex::Acquire<Block>(environment_Block, gfx, rg, environment_Block,
		XMFLOAT3{ 4.0f, 3.0f, 4.0f }, XMFLOAT3{ 4.0f, 4.0f, 4.0f }, true));

	blockTiled = ObjectCodex::Acquire<BlockTiled>(environment_BlockTiled, gfx, rg, environment_BlockTiled,
		XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(200.0f, 2.0f, 200.0f), XMFLOAT2(50.0f, 50.0f), true);
}

void EnvironmentManager::Update(float dt)
{}

void EnvironmentManager::Submit(void)
{
	for (int i = 0;i < blocks.size();i++)
	{
		if (blocks[i]->IsActive()) blocks[i]->Submit();
	}

	if (blockTiled->IsActive()) blockTiled->Submit();
}

void EnvironmentManager::Reset(void)
{
}