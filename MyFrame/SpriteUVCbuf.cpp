#include "SpriteUVCbuf.h"
#include <cassert>
#include "Sprite3D.h"
#include "Sprite2D.h"

namespace Bind
{
    SpriteUVCbuf::SpriteUVCbuf(Graphics& gfx, UINT slot)
    {
        if (!pVcbuf)
        {
            pVcbuf = std::make_unique<VertexConstantBuffer<UVData>>(gfx, slot);
        }
    }

    void SpriteUVCbuf::Bind(Graphics& gfx) noxnd
    {
        assert(pParent != nullptr);
        UpdateBindImpl(gfx, GetUVData());
    }

    void SpriteUVCbuf::InitializeParentReference(const Drawable& parent) noexcept
    {
        pParent = &parent;
    }

    std::unique_ptr<CloningBindable> SpriteUVCbuf::Clone() const noexcept
    {
        return std::make_unique<SpriteUVCbuf>(*this);
    }

    void SpriteUVCbuf::UpdateBindImpl(Graphics& gfx, const UVData& data) noxnd
    {
        pVcbuf->Update(gfx, data);
        pVcbuf->Bind(gfx);
    }

    SpriteUVCbuf::UVData SpriteUVCbuf::GetUVData() const noexcept
    {
        assert(pParent != nullptr);

		DirectX::XMFLOAT2 offset{};
		DirectX::XMFLOAT2 scale{};
        if (auto* p3 = dynamic_cast<const Sprite3D*>(pParent))
        {
            offset = p3->GetUVOffset();
            scale = p3->GetUVScale();
        }
        else if (auto* p2 = dynamic_cast<const Sprite2D*>(pParent))
        {
            offset = p2->GetUVOffset();
            scale = p2->GetUVScale();
        }

        return { offset, scale };
    }

    std::unique_ptr<VertexConstantBuffer<SpriteUVCbuf::UVData>> SpriteUVCbuf::pVcbuf;
}