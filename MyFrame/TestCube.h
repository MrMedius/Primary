#pragma once
#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"
#include "Transformation.h"

class TestCube : public Drawable
{
public:
	TestCube(Graphics& gfx, float size);
	void Update(float dt);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx, const char* name) noexcept;
private:
	Transformation trans;
};