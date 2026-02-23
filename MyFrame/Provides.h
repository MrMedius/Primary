#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct DynamicTextureTag
{
    using value_type = int;
    static constexpr value_type Default() noexcept { return 0; }
};

struct DynamicCubeTextureTag
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
        int trState = 0;    // 0 - none, 1 - expand, 2 - recover
        int trMode = 0;     // 0 - normal, 1 - burning, 2 - purifying, 3 - digital
        // parameters
        float trRadius{ 0.0f };
        float trSoftness{ 40.0f };
        XMFLOAT3 trCenter{ 0.0f,0.0f,0.0f };
        float opacity{ 1.0f };
        // RGB
        XMFLOAT3 colorFrom{ 1.0f,1.0f,1.0f };
        float pad0{ 0.0f };
        XMFLOAT3 colorTo{ 1.0f,1.0f,1.0f };
        float pad1{ 0.0f };
        // noise
        float noiseScale{ 0.02f };
        float noiseAmp{ 200.0f };
        XMFLOAT2 pad2{ 0.0f,0.0f };
    };

    static constexpr value_type Default() noexcept { return {}; }
};

struct SkyboxTransitionTag
{
    struct value_type
    {
        // core
        int trState{ 0 };
        int trMode{ 0 };
        // parameters
        int fromIdx{ 0 };
        int toIdx{ 0 };
        float t{ 0.0f };
        // radial transition
        XMFLOAT3 trCenter{ 0.0f, 0.0f, 0.0f };
        float trRadius{ 0.0f };
        float trSoftness{ 40.0f };
        // noise
        float noiseScale{ 0.02f };
        float noiseAmp{ 200.0f };
    };
    static constexpr value_type Default() noexcept { return {}; }
};

struct TutorialTransitionTag
{
    struct value_type
    {
        // --- core (同 FieldTransitionTag) ---
        int   trState;          // 0=None,1=Expand,2=Recover,3=Collapse
        int   trMode;           // 0=Normal,1=Burning,2=Purifying,3=Digital
        float trRadius;
        float trSoftness;
        XMFLOAT3 trCenter;
        float opacity;
        // --- RGB ---
        XMFLOAT3 colorFrom;
        float pad0;
        XMFLOAT3 colorTo;
        float pad1;
        // --- noise ---
        float noiseScale;
        float noiseAmp;
        XMFLOAT2 pad2;
        // --- Tutorial ?属 ---
        float collapseTime;      // 0~1 ?一化崩??度
        float vortexStrength;    // ???度
        float rippleCount;       // 波?数量
        float rainbowIntensity;  // 彩虹亮度
        XMFLOAT2 numTiles;       // 格子数量 (X, Z)
        float totalTime;         // 累???（Grid Shader ?用）
        float pad3;
    };
    static constexpr value_type Default() noexcept { return {}; }
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