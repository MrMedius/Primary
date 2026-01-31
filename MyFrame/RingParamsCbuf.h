#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"

namespace Bind
{
    class RingParamsCbuf : public CloningBindable
    {
    protected:
        struct RingData
        {
            float startAngleDeg;
            float endAngleDeg;
            float ratio;
            float padding;
        };
    public:
        RingParamsCbuf(Graphics& gfx, UINT slot = 0u);
        void Bind(Graphics& gfx) noxnd override;
        void InitializeParentReference(const Drawable& parent) noexcept override;
        std::unique_ptr<CloningBindable> Clone() const noexcept override;
    protected:
        void UpdateBindImpl(Graphics& gfx, const RingData& data) noxnd;
        RingData GetRingData() const noexcept;
    private:
        static std::unique_ptr<PixelConstantBuffer<RingData>> pPcbuf;
        const Drawable* pParent = nullptr;
    };
}