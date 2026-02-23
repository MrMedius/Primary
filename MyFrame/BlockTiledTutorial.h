#pragma once
#include "Environment.h"
#include "RenderGraph.h"
#include "CubeTiledTutorial.h"
#include "Channels.h"
#include "Player.h"

class BlockTiledTutorial : public Environment
{
public:
    BlockTiledTutorial(Graphics& gfx, Rgph::RenderGraph& rg,
        XMFLOAT3 position, XMFLOAT3 size, XMFLOAT2 numTiled,
        bool onCollision,
        Object_Type_Tag tag = environment_BlockTiledTutorial)
        : Environment(tag)
    {
        pPlayer = ObjectCodex::FindFirstActiveObjectByTag<Player>(character_Player);
        SetPosition(position);
        SetSize(size);
        SetCollisionSize(size);
        SetCollisionOnOff(onCollision);
        m_originalCollisionSize = size;

        visual = std::make_unique<CubeTiledTutorial>(gfx, size, numTiled);
        visual->LinkTechniques(rg);
        visual->SetPosition(transInfo.position);
        visual->GameStartExpend();

        DirectX::XMFLOAT3 localHalf{ 0.5f,0.5f,0.5f };
        boxCollider = BoxCollider::BuildFromWorldMatrix(transInfo.GetWorldMatrix(), localHalf);
    }

    void OnEnable() override { visual->GameStartExpend(); }

    void Update(float dt) override
    {
        auto playerPos = pPlayer->GetPosition();
        visual->SetGridCenter(playerPos);  // © XV Grid Wave ’†S

        if (!fieldPlay)
        {
            if (pPlayer->GetIsSkill())
            {
                fieldPlay = true;
                int mode = 0;
                switch (pPlayer->GetWeaponSlots()->GetCurrentSlot().GetType())
                {
                case WEAPON_TYPE_1: mode = 1; break;
                case WEAPON_TYPE_2: mode = 2; break;
                case WEAPON_TYPE_3: mode = 3; break;
                }
                if (visual->GetPlayMode() != mode)
                    visual->StartExpand(playerPos, mode);
            }
        }
        if (!pPlayer->GetIsSkill() && fieldPlay)
        {
            fieldPlay = false;
        }


        if (!GameStatsCodex::GetIsTutorial())
        {
            visual->StartCollapse({ 0.0f,0.0f,0.0f });
        }

        visual->Update(dt, playerPos);
    }

    void Submit() override
    {
        visual->Submit(Chan::main);
    }

    // Tutorial
    void StartCollapse() { visual->StartCollapse(transInfo.position); }
    bool IsCollapseFinished() const { return visual->IsCollapseFinished(); }
    bool IsCollapsing()       const { return visual->IsCollapsing(); }
    bool IsInsideCollapseHole(XMFLOAT3 p) const { return visual->IsInsideCollapseHole(p); }

    void ResetState()
    {
        visual->ResetState();
        SetCollisionSize(m_originalCollisionSize);
        SetCollisionOnOff(true);
        fieldPlay = false;
    }

    void SetGridOpacity(float v) { visual->SetGridOpacity(v); }
    float GetGridOpacity() const { return visual->GetGridOpacity(); }

    void OnCollide(Character*) override {}

private:
    std::unique_ptr<CubeTiledTutorial> visual;
    Player* pPlayer = nullptr;
    bool       fieldPlay = false;
    XMFLOAT3   m_originalCollisionSize{};
};