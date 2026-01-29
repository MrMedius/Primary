#pragma once
#include "Graphics.h"
#include "RenderGraph.h"
#include "Block.h"
#include "BlockTiled.h"

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
};