#include "Sprite2D.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "ConstantBuffersEx.h"
#include "Stencil.h"
#include "DynamicConstant.h"
#include "Channels.h"
#include "SpriteUVCbuf.h"
#include "DynamicTexture.h"

Sprite2D::Sprite2D(Graphics& gfx, std::vector<std::string> paths, std::string tag)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make2D(1, 1);
	const auto geometryTag = "$plane." + tag;
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto tcb = std::make_shared<Transform2DCbuf>(gfx);
	auto uvcb = std::make_shared<SpriteUVCbuf>(gfx);
	auto dynTex = std::make_shared<DynamicTexture>(gfx, paths);

	{
		Technique ui("UI", Chan::ui);
		{
			Step draw("ui");

			draw.AddBindable(dynTex);
			draw.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "GeneralSprite2D_VS.cso");
			draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
			draw.AddBindable(std::move(pvs));

			draw.AddBindable(PixelShader::Resolve(gfx, "GeneralSprite2D_PS.cso"));

			draw.AddBindable(tcb);
			draw.AddBindable(uvcb);

			ui.AddStep(std::move(draw));
		}
		AddTechnique(std::move(ui));
	}
}

void Sprite2D::Update(float dt)
{
	accum += 2 * dt;
	while (accum >= frameTime)
	{
		accum -= frameTime;
		frame++;
		if (frame > totalTex) frame = 1;
	}

	const int frameNo = (frame - 1) % totalTex + (startTex - 1);
	const int u = frameNo % numU;
	const int v = frameNo / numU;

	uvScale = { 1.0f / float(numU), 1.0f / float(numV) };
	uvOffset = { float(u) / float(numU), float(v) / float(numV) };


	texAccum += dt;
	if (texAccum >= 5.0f)
	{
		texAccum = 0.0f;
		texIndex++;
	}
}

void Sprite2D::SetPosition(float x, float y) noexcept
{
	trans.SetPosition(x, y, 0.0f);
}

void Sprite2D::SetRotation(float degrees) noexcept
{
	trans.SetRotationDegreeToRad(0.0f, 0.0f, degrees);
}

void Sprite2D::SetScale(float x, float y) noexcept
{
	trans.SetScale(x, y, 1.0f);
}

DirectX::XMMATRIX Sprite2D::GetTransformXM() const noexcept
{
	return trans.GetTransformXM();
}

DirectX::XMFLOAT2 Sprite2D::GetUVOffset() const noexcept
{
	return uvOffset;
}

DirectX::XMFLOAT2 Sprite2D::GetUVScale() const noexcept
{
	return uvScale;
}



// auto animation
void Sprite2D::SetAtlas(int numU_, int numV_, int startTex_, int totalTex_) noexcept
{
	numU = (numU_ > 0) ? numU_ : 1;
	numV = (numV_ > 0) ? numV_ : 1;
	startTex = (startTex_ > 0) ? startTex_ : 1;
	totalTex = (totalTex_ > 0) ? totalTex_ : 1;

	frame = 1;
	accum = 0.0f;

	uvScale = { 1.0f / float(numU), 1.0f / float(numV) };
	uvOffset = { 0.0f, 0.0f };
}

void Sprite2D::SetFPS(float fps) noexcept
{
	frameTime = (fps > 0.0f) ? (1.0f / fps) : 0.1f;
}



// manual animation
void Sprite2D::SetFrame(int numU_, int numV_, int startTex_, int totalTex_, int countTex_, int indexTex_, bool isFlip_) noexcept
{
	texIndex = indexTex_;

	numU = (numU_ > 0) ? numU_ : 1;
	numV = (numV_ > 0) ? numV_ : 1;
	startTex = (startTex_ > 0) ? startTex_ : 1;
	totalTex = (totalTex_ > 0) ? totalTex_ : 1;

	frame = countTex_;

	const int frameNo = (frame - 1) % totalTex + (startTex - 1);
	const int u = frameNo % numU;
	const int v = frameNo / numU;

	if (isFlip_)
	{
		uvScale = { -1.0f / float(numU), 1.0f / float(numV) };
		uvOffset = { float(u + 1) / float(numU), float(v) / float(numV) };
	}
	else
	{
		uvScale = { 1.0f / float(numU), 1.0f / float(numV) };
		uvOffset = { float(u) / float(numU), float(v) / float(numV) };
	}
}
