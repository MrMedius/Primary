#include "CubeTiled.h"
#include "BindableCommon.h"
#include "ConstantBuffersEx.h"
#include "DynamicVertex.h"
#include "Stencil.h"
#include "Channels.h"

#include "IndexedTriangleList.h"

namespace dx = DirectX;
using dvL = Dvtx::VertexLayout;

CubeTiled::CubeTiled(Graphics& gfx, DirectX::XMFLOAT3 size, dx::XMFLOAT2 numTiled)
{
	using namespace Bind;

	const auto geometryTag = "$cube." + std::to_string(numTiled.x);
	Dvtx::VertexLayout layout;
	layout.Append(dvL::Position3D).
		   Append(dvL::Normal).
		   Append(dvL::Texture2D);

	Dvtx::VertexBuffer vertices{ std::move(layout) };

	vertices.Resize(24u);
	{
		float side = 0.5f;

		// upper face
		vertices[ 0].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side,  side,  side }; vertices[ 0].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.0f };
		vertices[ 1].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side,  side,  side }; vertices[ 1].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.x,	0.0f };
		vertices[ 2].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side,  side, -side }; vertices[ 2].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		numTiled.y };
		vertices[ 3].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side,  side, -side }; vertices[ 3].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.x, numTiled.y };
		// front face																		 
		vertices[ 4].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side,  side, -side }; vertices[ 4].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.25f };
		vertices[ 5].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side,  side, -side }; vertices[ 5].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.x,	0.25f };
		vertices[ 6].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side, -side, -side }; vertices[ 6].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.5f };
		vertices[ 7].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side, -side, -side }; vertices[ 7].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.x,	0.5f };
		// down face																		 
		vertices[ 8].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side, -side, -side }; vertices[ 8].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.75f };
		vertices[ 9].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side, -side, -side }; vertices[ 9].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.x,	0.75f };
		vertices[10].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side, -side,  side }; vertices[10].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		1.0f };
		vertices[11].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side, -side,  side }; vertices[11].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.x,	1.0f };
		// back face																							    
		vertices[12].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side,  side,  side }; vertices[12].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.25f };
		vertices[13].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side,  side,  side }; vertices[13].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.x,	0.25f };
		vertices[14].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side, -side,  side }; vertices[14].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.5f };
		vertices[15].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side, -side,  side }; vertices[15].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.x,	0.5f };
		// left face																		 
		vertices[16].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side,  side,  side }; vertices[16].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.25f };
		vertices[17].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side,  side, -side }; vertices[17].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.y,	0.25f };
		vertices[18].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side, -side,  side }; vertices[18].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.5f };
		vertices[19].Attr<dvL::Position3D>() = dx::XMFLOAT3{ -side, -side, -side }; vertices[19].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.y,	0.5f };
		// right face																		 
		vertices[20].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side,  side, -side }; vertices[20].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.25f };
		vertices[21].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side,  side,  side }; vertices[21].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.y,	0.25f };
		vertices[22].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side, -side, -side }; vertices[22].Attr<dvL::Texture2D>() = dx::XMFLOAT2{	0.0f,		0.5f };
		vertices[23].Attr<dvL::Position3D>() = dx::XMFLOAT3{  side, -side,  side }; vertices[23].Attr<dvL::Texture2D>() = dx::XMFLOAT2{ numTiled.y,	0.5f };
	}

	std::vector<unsigned short> indices;
	{
		for (int i = 0;i < 6;i++)
		{
			indices.push_back(i * 4 + 0);
			indices.push_back(i * 4 + 1);
			indices.push_back(i * 4 + 2);
			indices.push_back(i * 4 + 1);
			indices.push_back(i * 4 + 3);
			indices.push_back(i * 4 + 2);
		}
	}
	IndexedTriangleList model(vertices, indices);
	model.Transform(dx::XMMatrixScaling(size.x, size.y, size.z));
	model.SetNormalsIndependentFlat();

	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto tcb = std::make_shared<TransformCbuf>(gfx);

	{
		Technique shade("Shade", Chan::main);
		{
			Step top("lambertian");

			top.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "General_VS.cso");
			top.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
			top.AddBindable(std::move(pvs));

			top.AddBindable(PixelShader::Resolve(gfx, "General_PS.cso"));
			Dcb::RawLayout lay;
			lay.Add<Dcb::Float3>("specularColor");
			lay.Add<Dcb::Float>("specularWeight");
			lay.Add<Dcb::Float>("specularGloss");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["specularColor"] = dx::XMFLOAT3{ 1.0f,1.0f,1.0f };
			buf["specularWeight"] = 0.1f;
			buf["specularGloss"] = 20.0f;
			top.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));

			top.AddBindable(Rasterizer::Resolve(gfx, false));

			top.AddBindable(tcb);

			// separate top and body to allow different textures
			Step body = top;

			// top face
			top.AddBindable(Texture::Resolve(gfx, "asset\\Images\\block_field_top.png"));
			top.SetDrawIndexed(6u, 0u);
			shade.AddStep(std::move(top));

			// body faces
			body.AddBindable(Texture::Resolve(gfx, "asset\\Images\\block_field.png"));
			body.SetDrawIndexed(30u, 6u);
			shade.AddStep(std::move(body));
		}
		AddTechnique(std::move(shade));
	}
	{
		Technique outline("Outline", Chan::main);
		{
			Step mask("outlineMask");

			mask.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *VertexShader::Resolve(gfx, "Solid_VS.cso")));

			mask.AddBindable(std::move(tcb));

			outline.AddStep(std::move(mask));
		}
		{
			Step draw("outlineDraw");

			Dcb::RawLayout lay;
			lay.Add<Dcb::Float4>("color");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["color"] = DirectX::XMFLOAT4{ 4.0f,0.4f,0.4f,1.0f };
			draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));

			draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *VertexShader::Resolve(gfx, "Solid_VS.cso")));

			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

			outline.AddStep(std::move(draw));
		}
		AddTechnique(std::move(outline));
		// shadow map technique
		{
			Technique map{ "ShadowMap",Chan::shadow,true };
			{
				Step draw("shadowMap");

				auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
				draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
				draw.AddBindable(std::move(pvs));

				draw.AddBindable(NullPixelShader::Resolve(gfx));

				draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

				map.AddStep(std::move(draw));
			}
			AddTechnique(std::move(map));
		}
	}
}

void CubeTiled::SetPosition(DirectX::XMFLOAT3 pos) noexcept
{
	trans.SetPosition(pos.x, pos.y, pos.z);
}

void CubeTiled::SetRotation(float roll, float pitch, float yaw) noexcept
{
	trans.SetRotationDegreeToRad(roll, pitch, yaw);
}

void CubeTiled::SetScale(DirectX::XMFLOAT3 size) noexcept
{
	trans.SetScale(size.x, size.y, size.z);
}

DirectX::XMMATRIX CubeTiled::GetTransformXM() const noexcept
{
	return trans.GetTransformXM();
}