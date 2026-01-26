#include "SpriteUVCbuf.h"
#include <cassert>
#include "Sprite3D.h"

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
        const auto& spr = dynamic_cast<const Sprite3D&>(*pParent);
        return { spr.GetUVOffset(), spr.GetUVScale() };
    }

    std::unique_ptr<VertexConstantBuffer<SpriteUVCbuf::UVData>> SpriteUVCbuf::pVcbuf;
}