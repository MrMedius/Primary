#include "EnvironmentManager.h"
#include "ObjectCodex.h"

#include "Block.h"
#include "BlockTiled.h"
#include "BlockInvisible.h"
#include "BlockFog.h"

EnvironmentManager::EnvironmentManager(Graphics& gfx, Rgph::RenderGraph& rg)
	:
	gfx(gfx),
	rg(rg)
{
	environments.push_back(ObjectCodex::Acquire<Block>(environment_Block, gfx, rg, XMFLOAT3{ 4.0f, 3.0f, 4.0f }, XMFLOAT3{ 4.0f, 4.0f, 4.0f }, true));

	XMFLOAT3 fieldSize = XMFLOAT3(300.0f, 2.0f, 100.0f);
	environments.push_back(ObjectCodex::Acquire<BlockTiled>(environment_BlockTiled, gfx, rg, XMFLOAT3(0.0f, 0.0f, 0.0f), fieldSize, XMFLOAT2(30.0f, 10.0f), true));

	float longSide = 300.0f, shortSide = 50.0f;
	environments.push_back(ObjectCodex::Acquire<BlockInvisible>(environment_BlockInvisible, gfx, rg,
		XMFLOAT3{  fieldSize.x / 2.0f + shortSide / 2.0f, 0.0f, 0.0f }, XMFLOAT3{ shortSide, 100.0f, longSide }, true));
	environments.push_back(ObjectCodex::Acquire<BlockInvisible>(environment_BlockInvisible, gfx, rg,
		XMFLOAT3{ -fieldSize.x / 2.0f - shortSide / 2.0f, 0.0f, 0.0f }, XMFLOAT3{ shortSide, 100.0f, longSide }, true));
	environments.push_back(ObjectCodex::Acquire<BlockInvisible>(environment_BlockInvisible, gfx, rg,
		XMFLOAT3{ 0.0f, 0.0f,  fieldSize.z / 2.0f + shortSide / 2.0f }, XMFLOAT3{ longSide, 100.0f, shortSide }, true));
	environments.push_back(ObjectCodex::Acquire<BlockInvisible>(environment_BlockInvisible, gfx, rg,
		XMFLOAT3{ 0.0f, 0.0f, -fieldSize.z / 2.0f - shortSide / 2.0f }, XMFLOAT3{ longSide, 100.0f, shortSide }, true));

	environments.push_back(ObjectCodex::Acquire<BlockFog>(environment_BlockFog, gfx, rg, XMFLOAT3(0.0f, -300.0f, 0.0f), XMFLOAT3(3000.0f, 500.0f, 3000.0f)));
}

void EnvironmentManager::Update(float dt)
{
	for (int i = 0;i < environments.size();i++)
	{
		environments[i]->Update(dt);
	}
}

void EnvironmentManager::Submit(void)
{
	for (int i = 0;i < environments.size();i++)
	{
		environments[i]->Submit();
	}
}

void EnvironmentManager::Reset(void)
{
	for (int i = 0;i < environments.size();i++)
	{
		environments[i]->OnEnable();
	}
}