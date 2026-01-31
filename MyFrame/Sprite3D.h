#pragma once
#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"
#include "Transformation.h"
#include "Provides.h"

class Sprite3D : public Drawable, public IProvides<DynamicTextureTag>, public IProvides<SpriteUVTag>
{
public:
	Sprite3D(Graphics& gfx, std::vector<std::string> paths);
	void Update(float dt);
	void SetPosition(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	void SetScale(DirectX::XMFLOAT3 size) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	// auto animation
	void SetFrameAuto(int numU_, int numV_, int startTex_, int totalTex_, int indexTex_, float fps, bool isFlip_);
	void SetAtlas(int numU_, int numV_, int startTex_, int totalTex_) noexcept;
	void SetFPS(float fps) noexcept;
	// manual animation
	void SetFrame(int numU_, int numV_, int startTex_, int totalTex_, int countTex_, int indexTex_, bool isFlip_) noexcept;
	void SetRatioOffset(float ratioX, float ratioY) noexcept;
	// setters
	void SetFlip(bool isFlip_) noexcept { isFlip = isFlip_; }
	void SetTextureFrameIndex(int i) noexcept { texIndex = i; }

	DynamicTextureTag::value_type Provide(DynamicTextureTag) const noexcept override { return texIndex; }
	SpriteUVTag::value_type Provide(SpriteUVTag) const noexcept override { return { uvOffset, uvScale }; }
private:
	Transformation trans;
	TransInfo transBase;
	DirectX::XMFLOAT2 uvOffset = { 0.0f,0.0f };
	DirectX::XMFLOAT2 uvScale = { 1.0f,1.0f };

	int numU = 1;
	int numV = 1;
	int startTex = 1;
	int totalTex = 1;

	int frame = 1;
	float frameTime = 0.1f;
	float accum = 0.0f;

	bool isFlip = false;

	int texIndex = 0;
};