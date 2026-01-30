#include "DynamicTexture.h"
#include "Texture.h"
#include <cassert>
#include "Sprite3D.h"
#include "Sprite2D.h"

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
		assert(pParent != nullptr);

		int idx = 0;
		if (auto* p3 = dynamic_cast<const Sprite3D*>(pParent))
			idx = p3->GetTextureFrameIndex();
		else if (auto* p2 = dynamic_cast<const Sprite2D*>(pParent))
			idx = p2->GetTextureFrameIndex();

		const int n = (int)frames.size();
		if (n <= 0) return;
		idx %= n;
		if (idx < 0) idx += n;

		frames[(size_t)idx]->Bind(gfx);
	}
}