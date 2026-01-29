#pragma once
#include "Drawable.h"
#include "Bindable.h"
#include "Transformation.h"

class CubeTiled : public Drawable
{
public:
	CubeTiled(Graphics& gfx, DirectX::XMFLOAT3 size, DirectX::XMFLOAT2 numTiled);
	void SetPosition(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	void SetScale(DirectX::XMFLOAT3 size) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	Transformation trans;
};