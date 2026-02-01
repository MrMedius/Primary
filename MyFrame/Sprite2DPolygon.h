#pragma once
#include "Sprite.h"
#include "Provides.h"

class Sprite2DPolygon : public Sprite, public IProvides<RingParamsTag>
{
public:
    Sprite2DPolygon(Graphics& gfx, std::vector<std::string> paths, int sides = 3);
    ~Sprite2DPolygon() = default;
	// setters
    void SetRingRange(float start, float end) noexcept;
	void SetRingRatio(float ratio) noexcept;
	// getters
    RingParamsTag::value_type Provide(RingParamsTag) const noexcept override { return ringParams; }
private:
	RingParamsTag::value_type ringParams{ 0.0f,360.0f,1.0f,0.0f };
};