#include "Sprite3D.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "ConstantBuffersEx.h"
#include "imgui/imgui.h"
#include "Stencil.h"
#include "DynamicConstant.h"
#include "Channels.h"
#include "SpriteUVCbuf.h"

Sprite3D::Sprite3D(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$plane." + std::to_string(size);
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto tcb = std::make_shared<TransformCbuf>(gfx);
	auto uvcb = std::make_shared<Bind::SpriteUVCbuf>(gfx);

	{
		Technique shade("Shade", Chan::main);
		{
			Step only("lambertian");

			only.AddBindable(Texture::Resolve(gfx, "asset\\Images\\Player\\Player_Usual.png"));
			only.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "GeneralSprite3D_VS.cso");
			only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
			only.AddBindable(std::move(pvs));

			only.AddBindable(PixelShader::Resolve(gfx, "GeneralSprite3D_PS.cso"));

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

			draw.AddBindable(Bind::Texture::Resolve(gfx, "asset\\Images\\Player\\Player_Usual.png"));
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

	uvScale = { 1.0f / float(numU), 1.0f / float(numV) };
	uvOffset = { float(u) / float(numU), float(v) / float(numV) };
}

void Sprite3D::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	trans.SetPosition(pos.x, pos.y, pos.z);
}

void Sprite3D::SetRotation(float roll, float pitch, float yaw) noexcept
{
	trans.SetRotationDegreeToRad(roll, pitch, yaw);
}

DirectX::XMMATRIX Sprite3D::GetTransformXM() const noexcept
{
	return trans.GetTransformXM();
}

DirectX::XMFLOAT2 Sprite3D::GetUVOffset() const noexcept 
{ 
	return uvOffset; 
}

DirectX::XMFLOAT2 Sprite3D::GetUVScale() const noexcept
{ 
	return uvScale; 
}

void Sprite3D::SpawnControlWindow(Graphics& gfx, const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		auto translation = trans.GetPosition();
		auto angles = trans.GetRotation();
		auto scales = trans.GetScale();

		bool dirty = false;
		ImGui::Text("Position");
		dirty |= ImGui::SliderFloat("X", &translation.x, -80.0f, 80.0f, "%.1f");
		dirty |= ImGui::SliderFloat("Y", &translation.y, -80.0f, 80.0f, "%.1f");
		dirty |= ImGui::SliderFloat("Z", &translation.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		dirty |= ImGui::SliderAngle("Roll", &angles.x, -180.0f, 180.0f);
		dirty |= ImGui::SliderAngle("Pitch", &angles.y, -180.0f, 180.0f);
		dirty |= ImGui::SliderAngle("Yaw", &angles.z, -180.0f, 180.0f);
		ImGui::Text("Scale");
		dirty |= ImGui::SliderFloat("ScaleX", &scales.x, 1.0f, 100.0f);
		dirty |= ImGui::SliderFloat("ScaleY", &scales.y, 1.0f, 100.0f);
		dirty |= ImGui::SliderFloat("ScaleZ", &scales.z, 1.0f, 100.0f);
		if (dirty)
		{
			trans.SetPosition(translation.x, translation.y, translation.z);
			trans.SetRotationRad(angles.x, angles.y, angles.z);
			trans.SetScale(scales.x, scales.y, scales.z);
		}

		class Probe : public TechniqueProbe
		{
		public:
			void OnSetTechnique() override
			{
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, pTech->GetName().c_str());
				bool active = pTech->IsActive();
				ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
				if (pTech->IsActive() != active)
					pTech->SetActiveState(active);
			}
			bool OnVisitBuffer(Dcb::Buffer& buf) override
			{
				namespace dx = DirectX;
				float dirty = false;
				const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed;};
				auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
				(const char* label) mutable
					{
						tagScratch = label + tagString;
						return tagScratch.c_str();
					};

				if (auto v = buf["scale"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f", 3.5f));
				}
				if (auto v = buf["color"]; v.Exists())
				{
					dcheck(ImGui::ColorPicker4(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT4&>(v))));
				}
				if (auto v = buf["specularIntensity"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Spec. Intens."), &v, 0.0f, 1.0f));
				}
				if (auto v = buf["specularPower"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
				}
				return dirty;
			}
		} probe;

		Accept(probe);
	}
	ImGui::End();
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