#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct DynamicTextureTag
{
    using value_type = int;
    static constexpr value_type Default() noexcept { return 0; }
};

struct SpriteUVTag
{
    struct value_type
    {
        XMFLOAT2 offset{ 0.0f, 0.0f };
        XMFLOAT2 scale{ 1.0f, 1.0f };
    };
    static constexpr value_type Default() noexcept { return {}; }
};

struct RingParamsTag
{
    struct value_type
    {
        float startAngleDeg{ 0.0f };
        float endAngleDeg{ 359.0f };
        float ratio{ 1.0f };
        float padding;
    };
    static constexpr value_type Default() noexcept { return {}; }
};

struct FieldTransitionTag
{
    struct value_type
    {
        // core
        int trState;    // 0 - none, 1 - expand, 2 - recover
        int trMode;     // 0 - normal, 1 - burning, 2 - purifying, 3 - digital
        // parameters
        float trRadius;
        float trSoftness;
        XMFLOAT3 trCenter;
        float opacity;
        // RGB
        XMFLOAT3 colorFrom;
        float pad0;
        XMFLOAT3 colorTo;
        float pad1;
        // noise
        float noiseScale;
        float noiseAmp;
        XMFLOAT2 pad2;
    };

    static constexpr value_type Default() noexcept
    {
        return {
            0, 0,
            0, 40.0f, {0.0f,0.0f,0.0f}, 1.0f,
            {1.0f,1.0f,1.0f}, 0.0f,
            {1.0f,1.0f,1.0f}, 0.0f,
            0.02f, 200.0f, {0.0f,0.0f}
        };
    }
};



template<class Tag>
struct IProvides
{
    using value_type = typename Tag::value_type;
    virtual ~IProvides() = default;

    virtual value_type Provide(Tag) const noexcept = 0;
};

template<class Tag, class BaseT>
inline typename Tag::value_type TryProvide(const BaseT* pBase) noexcept
{
    if (auto* p = dynamic_cast<const IProvides<Tag>*>(pBase))
    {
        return p->Provide(Tag{});
    }

    return Tag::Default();
}
