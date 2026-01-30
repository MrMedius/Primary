#pragma once
#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"
#include "Transformation.h"

class Sprite2D : public Drawable
{
public:
    Sprite2D(Graphics& gfx, std::vector<std::string> paths, std::string tag);
    void Update(float dt);
    void SetPosition(float x, float y) noexcept;
    void SetRotation(float degrees) noexcept;
    void SetScale(float x, float y) noexcept;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;
    DirectX::XMFLOAT2 GetUVOffset() const noexcept;
    DirectX::XMFLOAT2 GetUVScale() const noexcept;

    // auto animation
    void SetAtlas(int numU_, int numV_, int startTex_, int totalTex_) noexcept;
    void SetFPS(float fps) noexcept;
    int GetTextureFrameIndex() const noexcept { return texIndex; }
    void SetTextureFrameIndex(int i) noexcept { texIndex = i; }

    // manual animation
    void SetFrame(int numU_, int numV_, int startTex_, int totalTex_, int countTex_, int indexTex_, bool isFlip_) noexcept;
private:
    Transformation trans;
   
    DirectX::XMFLOAT2 uvOffset = { 0.0f,0.0f };
    DirectX::XMFLOAT2 uvScale = { 1.0f,1.0f };

    int numU = 1;
    int numV = 1;
    int startTex = 1;
    int totalTex = 1;
    int frame = 1;

    float frameTime = 0.1f;
    float accum = 0.0f;

    int texIndex = 0;
    float texAccum = 0.0f;
};