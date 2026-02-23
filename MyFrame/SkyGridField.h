#pragma once
#include "Environment.h"
#include "RenderGraph.h"
#include "SkyGridCylinder.h"
#include "Channels.h"

class SkyGridField : public Environment
{
public:
    SkyGridField(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag = environment_SkyGridField)
        :
        Environment(tag)
    {
        SetCollisionOnOff(false);

        m_cylinder = std::make_unique<SkyGridCylinder>(gfx);
        m_cylinder->LinkTechniques(rg);

        SetPosition(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
    }

    void OnEnable(void) override {}

    void Update(float dt) override
    {
        m_cylinder->Update(dt);

        //a += dt;
        //if (a >= 10.0f && !b)
        //{
        //    StartFadeOut();
        //    b = true;
        //}
    }

    void Submit(void) override
    {
        if (!m_cylinder->IsFadeOutFinished())
            m_cylinder->Submit(Chan::main);
    }

    void OnCollide(Character* other) override {}

    void StartFadeOut(float duration = 2.0f) { m_cylinder->StartFadeOut(duration); }
    bool IsFadeOutFinished() const { return m_cylinder->IsFadeOutFinished(); }

private:
    std::unique_ptr<SkyGridCylinder> m_cylinder;

    float a = 0.0f;
    bool b = false;
};