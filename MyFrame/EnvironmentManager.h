#pragma once
#include "Graphics.h"
#include "RenderGraph.h"
#include "Block.h"
#include "BlockTiled.h"
#include "BlockInvisible.h"

class EnvironmentManager
{
public:
	EnvironmentManager(Graphics& gfx, Rgph::RenderGraph& rg);
	~EnvironmentManager() = default;
	void Update(float dt);
	void Submit(void);
	void Reset(void);
private:
	Graphics& gfx;
	Rgph::RenderGraph& rg;

	std::vector<Block*> blocks;
	BlockTiled* blockTiled;
	std::vector<BlockInvisible*> blocksInvisible;
};