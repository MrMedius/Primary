#include "Sprite3D.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "ConstantBuffersEx.h"
#include "Stencil.h"
#include "DynamicConstant.h"
#include "Channels.h"
#include "SpriteUVCbuf.h"
#include "DynamicTexture.h"

Sprite3D::Sprite3D(Graphics& gfx, std::vector<std::string> paths)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$plane." + paths[0] + std::to_string(paths[0].size());
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto tcb = std::make_shared<TransformCbuf>(gfx);
	auto uvcb = std::make_shared<SpriteUVCbuf>(gfx);
	auto dynTex = std::make_shared<DynamicTexture>(gfx, paths);

	{
		Technique shade("Shade", Chan::main);
		{
			Step only("lambertian");

			only.AddBindable(dynTex);
			only.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "Sprite3D_VS.cso");
			only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
			only.AddBindable(std::move(pvs));

			only.AddBindable(PixelShader::Resolve(gfx, "Sprite3D_PS.cso"));

			Dcb::RawLayout lay;
			lay.Add<Dcb::Float3>("specularColor");
			lay.Add<Dcb::Float>("specularWeight");
			lay.Add<Dcb::Float>("specularGloss");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["specularColor"] = dx::XMFLOAT3{ 1.0f,1.0f,1.0f };
			buf["specularWeight"] = 0.1f;
			buf["specularGloss"] = 20.0f;
			only.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));

			only.AddBindable(Rasterizer::Resolve(gfx, true));

			only.AddBindable(tcb);
			only.AddBindable(uvcb);

			shade.AddStep(std::move(only));
		}
		AddTechnique(std::move(shade));
	}
	// shadow map technique
	{
		Technique map{ "ShadowMap", Chan::shadow, true };
		{
			Step draw("shadowMap");
			auto pvs = Bind::VertexShader::Resolve(gfx, "ShadowAlpha_VS.cso");
			draw.AddBindable(Bind::InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
			draw.AddBindable(std::move(pvs));
			draw.AddBindable(Bind::PixelShader::Resolve(gfx, "ShadowAlpha_PS.cso"));

			draw.AddBindable(dynTex);
			draw.AddBindable(Bind::Sampler::Resolve(gfx));

			draw.AddBindable(tcb);
			draw.AddBindable(uvcb);

			draw.AddBindable(Bind::Rasterizer::Resolve(gfx, true));

			map.AddStep(std::move(draw));
		}
		AddTechnique(std::move(map));
	}
}

void Sprite3D::Update(float dt)
{
	accum += dt;
	while (accum >= frameTime)
	{
		accum -= frameTime;
		frame++;
		if (frame > totalTex) frame = 1;
	}

	const int frameNo = (frame - 1) % totalTex + (startTex - 1);
	const int u = frameNo % numU;
	const int v = frameNo / numU;

	if (isFlip)
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

void Sprite3D::SetPosition(DirectX::XMFLOAT3 pos) noexcept
{
	trans.SetPosition(pos.x, pos.y, pos.z);
}

void Sprite3D::SetRotation(float roll, float pitch, float yaw) noexcept
{
	trans.SetRotationDegreeToRad(roll, pitch, yaw);
}

void Sprite3D::SetScale(DirectX::XMFLOAT3 size) noexcept
{
	trans.SetScale(size.x, size.y, size.z);
}

DirectX::XMMATRIX Sprite3D::GetTransformXM() const noexcept
{
	return trans.GetTransformXM();
}



// auto animation
void Sprite3D::SetFrameAuto(int numU_, int numV_, int startTex_, int totalTex_, int indexTex_, float fps, bool isFlip_)
{
	SetAtlas(numU_, numV_, startTex_, totalTex_);
	SetFPS(fps);
	SetTextureFrameIndex(indexTex_);
	SetFlip(isFlip_);
}

void Sprite3D::SetAtlas(int numU_, int numV_, int startTex_, int totalTex_) noexcept
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

void Sprite3D::SetFPS(float fps) noexcept
{
	frameTime = (fps > 0.0f) ? (1.0f / fps) : 0.1f;
}



// manual animation
void Sprite3D::SetFrame(int numU_, int numV_, int startTex_, int totalTex_, int countTex_, int indexTex_, bool isFlip_) noexcept
{
	SetTextureFrameIndex(indexTex_);

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

void Sprite3D::SetRatioOffset(float ratioX, float ratioY) noexcept
{
	ratioX = std::clamp(ratioX, 0.0f, 1.0f);
	ratioY = std::clamp(ratioY, 0.0f, 1.0f);

	auto pos = transBase.position;
	auto size = transBase.scale;
	auto rad = transBase.rotation.z;
	const float c = cosf(rad);
	const float s = sinf(rad);

	float offsetX = size.x * 0.5f * (1.0f - ratioX);
	float offsetY = size.y * 0.5f * (1.0f - ratioY);

	float rotatedOffsetX = c * offsetX - c * offsetY;
	float rotatedOffsetY = s * offsetX + s * offsetY;

	trans.SetPosition(pos.x - rotatedOffsetX, pos.y - rotatedOffsetY, 0.0f);
	trans.SetScale(size.x * ratioX, size.y * ratioY, 1.0f);

	uvScale = { (float)1 / numU * ratioX, (float)1 / numV * ratioY };
}