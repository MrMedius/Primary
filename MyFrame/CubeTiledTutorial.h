#pragma once
#include "Drawable.h"
#include "Transformation.h"
#include "Provides.h"
using namespace DirectX;

class CubeTiledTutorial : public Drawable, public IProvides<TutorialTransitionTag>
{
public:
    CubeTiledTutorial(Graphics& gfx, XMFLOAT3 size, XMFLOAT2 numTiled);
    void SetPosition(XMFLOAT3 pos) noexcept;
    void SetRotation(float roll, float pitch, float yaw) noexcept;
    void SetScale(XMFLOAT3 size) noexcept;
    XMMATRIX GetTransformXM() const noexcept override;

    // State Control
    void Update(float dt, const XMFLOAT3& centerWorld);
    void StartExpand(const XMFLOAT3& center, int mode);
    void StartRecover(int mode);
    void FinishRecover();
    void GameStartExpend();
    int  GetPlayMode() { return FT.trMode; }

    // Collapse
    void StartCollapse(XMFLOAT3 center);
    bool IsCollapseFinished() const { return m_collapseFinished; }
    bool IsCollapsing()       const { return m_isCollapsing; }
    bool IsInsideCollapseHole(XMFLOAT3 worldPos) const;
    float GetCurrentCollapseRadius() const { return m_currentCollapseRadiusWorld; }
    XMFLOAT3 GetCollapseCenter() const { return FT.trCenter; }

    // Grid Opacity
    void  SetGridOpacity(float v) { m_gridOpacity = v; }
    float GetGridOpacity()  const { return m_gridOpacity; }

    void SetGridCenter(XMFLOAT3 playerPos);

    void ResetState();

    TutorialTransitionTag::value_type Provide(TutorialTransitionTag) const noexcept override;

private:
    Transformation trans;

    TutorialTransitionTag::value_type FT{};

    // State Machine
    float m_gridTimeAccumulator = 0.0f;
    float m_gridOpacity = 1.0f;

    // collapse
    bool  m_isCollapsing = false;
    bool  m_collapseFinished = false;
    float m_collapseSpeed = 0.5f;
    float m_collapseElapsedTime = 0.0f;
    float m_collapseDuration = 180.0f;
    float m_totalTimeAccumulator = 0.0f;
    float m_maxVortexStrength = 0.3f;
    float m_maxRainbowIntensity = 4.0f;
    float m_currentCollapseRadiusWorld = 0.0f;
    XMFLOAT3 m_size{};
    XMFLOAT2 m_numTiled{};

    // normal
    float m_t = 0.0f;
    float m_duration = 2.0f;
    float m_maxRadius = 200.0f;
};