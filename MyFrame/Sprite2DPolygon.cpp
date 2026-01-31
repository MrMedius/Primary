#include "Sprite2DPolygon.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "ConstantBuffersEx.h"
#include "Stencil.h"
#include "DynamicConstant.h"
#include "Channels.h"
#include "SpriteUVCbuf.h"
#include "DynamicTexture.h"
#include "RingParamsCbuf.h"

Sprite2DPolygon::Sprite2DPolygon(Graphics& gfx, std::vector<std::string> paths, int sides)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = PlanePolygon::Make2D(sides);
	const auto geometryTag = "$planePoly." + paths[0] + std::to_string(paths[0].size());
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto tcb = std::make_shared<Transform2DCbuf>(gfx);
	auto uvcb = std::make_shared<SpriteUVCbuf>(gfx);
	auto rpcb = std::make_shared<RingParamsCbuf>(gfx);
	auto dynTex = std::make_shared<DynamicTexture>(gfx, paths);

	{
		Technique ui("UI", Chan::ui);
		{
			Step draw("ui");

			draw.AddBindable(dynTex);
			draw.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "Sprite2DPolygon_VS.cso");
			draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
			draw.AddBindable(std::move(pvs));

			draw.AddBindable(PixelShader::Resolve(gfx, "Sprite2DPolygon_PS.cso"));
			draw.AddBindable(rpcb);

			draw.AddBindable(tcb);
			draw.AddBindable(uvcb);

			ui.AddStep(std::move(draw));
		}
		AddTechnique(std::move(ui));
	}
}

void Sprite2DPolygon::Update(float dt)
{
	accum += dt;
	while (accum >= frameTime)
	{
		accum -= frameTime;

		if (isLoop)
		{
			frame++;
			if (frame > totalTex) frame = 1;
		}
		else
		{
			if (frame < totalTex) frame++;
		}
	}
	if (!isLoop && frame > totalTex) frame = totalTex;

	UpdateUV();
}

void Sprite2DPolygon::SetPosition(float x, float y) noexcept
{
	trans.SetPosition(x, y, 0.0f);
	transBase.position = trans.GetPosition();
}

void Sprite2DPolygon::SetRotation(float degrees) noexcept
{
	trans.SetRotationDegreeToRad(0.0f, 0.0f, degrees);
	transBase.rotation = trans.GetRotation();
}

void Sprite2DPolygon::SetScale(float x, float y) noexcept
{
	trans.SetScale(x, y, 1.0f);
	transBase.scale = trans.GetScale();
}

DirectX::XMMATRIX Sprite2DPolygon::GetTransformXM() const noexcept
{
	return trans.GetTransformXM();
}



// auto animation
void Sprite2DPolygon::SetFrameAuto(int numU_, int numV_, int startTex_, int totalTex_, int indexTex_, float fps, bool isFlip_, bool isLoop_) noexcept
{
	SetAtlas(numU_, numV_, startTex_, totalTex_);
	SetFPS(fps);
	SetTextureFrameIndex(indexTex_);
	SetFlip(isFlip_);
	SetLoop(isLoop_);
}

void Sprite2DPolygon::SetAtlas(int numU_, int numV_, int startTex_, int totalTex_) noexcept
{
	numU = (numU_ > 0) ? numU_ : 1;
	numV = (numV_ > 0) ? numV_ : 1;
	startTex = (startTex_ > 0) ? startTex_ : 1;
	totalTex = (totalTex_ > 0) ? totalTex_ : 1;

	frame = 1;
	accum = 0.0f;

	UpdateUV();
}

void Sprite2DPolygon::SetFPS(float fps) noexcept
{
	frameTime = (fps > 0.0f) ? (1.0f / fps) : 0.1f;
}



// manual animation
void Sprite2DPolygon::SetFrame(int numU_, int numV_, int startTex_, int totalTex_, int countTex_, int indexTex_, bool isFlip_) noexcept
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

void Sprite2DPolygon::SetRatioOffset(float ratioX, float ratioY) noexcept
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

	float rotatedOffsetX = c * offsetX - s * offsetY;
	float rotatedOffsetY = s * offsetX + c * offsetY;

	trans.SetPosition(pos.x - rotatedOffsetX, pos.y - rotatedOffsetY, 0.0f);
	trans.SetScale(size.x * ratioX, size.y * ratioY, 1.0f);

	uvScale = { (float)1 / numU * ratioX, (float)1 / numV * ratioY };
}

void Sprite2DPolygon::SetRingRange(float start, float end) noexcept
{
	ringParams.startAngleDeg = start;
	ringParams.endAngleDeg = end;
}

void Sprite2DPolygon::SetRingRatio(float ratio) noexcept
{
	ringParams.ratio = ratio;
}


void Sprite2DPolygon::UpdateUV(void) noexcept
{
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