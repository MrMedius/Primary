#include "DynamicTexture.h"
#include "Texture.h"
#include <cassert>
#include "Sprite3D.h"

namespace Bind
{
	DynamicTexture::DynamicTexture(Graphics& gfx, std::vector<std::string> paths, UINT slot)
	{
		frames.reserve(paths.size());
		for (auto& p : paths)
		{
			frames.push_back(Bind::Texture::Resolve(gfx, p, slot));
		}
		assert(!frames.empty());
	}

	void DynamicTexture::InitializeParentReference(const Drawable& parent) noexcept
	{
		pParent = &parent;
	}

	std::unique_ptr<CloningBindable> DynamicTexture::Clone() const noexcept
	{
		return std::make_unique<DynamicTexture>(*this);
	}

	void DynamicTexture::Bind(Graphics& gfx) noxnd
	{
		int idx = 0;

		assert(pParent != nullptr);
		const auto& spr = dynamic_cast<const Sprite3D&>(*pParent);
		idx = spr.GetTextureFrameIndex();

		const int n = (int)frames.size();
		if (n <= 0) return;
		idx %= n;
		if (idx < 0) idx += n;

		frames[(size_t)idx]->Bind(gfx);
	}
}