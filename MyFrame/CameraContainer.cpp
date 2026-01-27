#include "CameraContainer.h"
#include "imgui/imgui.h"
#include "Camera.h"
#include "Graphics.h"
#include "RenderGraph.h"

namespace dx = DirectX;

CameraContainer::CameraContainer(Graphics& gfx)
{
	AddCamera(std::make_unique<Camera>(gfx, "Main Camera", dx::XMFLOAT3{ 0.0f,25.0f,-30.0f }, PI / 6.0f, 0.0f));
	AddCamera(std::make_unique<Camera>(gfx, "Observer", dx::XMFLOAT3{ -13.5f,28.8f,-6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f));
}

void CameraContainer::Bind( Graphics& gfx )
{
	gfx.SetCamera((*this)->GetMatrix());
}

void CameraContainer::AddCamera( std::shared_ptr<Camera> pCam )
{
	cameras.push_back( std::move( pCam ) );
}

Camera* CameraContainer::operator->() 
{
	return &GetActiveCamera();
}

CameraContainer::~CameraContainer()
{}

void CameraContainer::Update(float dt, DirectX::XMFLOAT3 pos,Window* wnd) noexcept
{
	// Main Camera Follow
	FollowTarget.x += (pos.x - FollowTarget.x) * FollowLerp;
	FollowTarget.y += (pos.y - FollowTarget.y) * FollowLerp;
	FollowTarget.z += (pos.z - FollowTarget.z) * FollowLerp;
	constexpr float yaw = 0.0f;
	const float sx = sinf(FollowPitch);
	const float cx = cosf(FollowPitch);
	const float sy = sinf(yaw);
	const float cy = cosf(yaw);

	dx::XMFLOAT3 camPos;
	camPos.x = FollowTarget.x + sy * cx * FollowRadius;
	camPos.y = FollowTarget.y + sx * FollowRadius;
	camPos.z = FollowTarget.z - cy * cx * FollowRadius;
	const dx::XMFLOAT3 camRot = { FollowPitch, yaw, 0.0f };
	cameras[0]->SetPos(camPos);
	cameras[0]->SetRot(camRot);

#ifdef _DEBUG
	while (const auto e = wnd->kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (wnd->CursorEnabled())
			{
				wnd->DisableCursor();
				wnd->mouse.EnableRaw();
			}
			else
			{
				wnd->EnableCursor();
				wnd->mouse.DisableRaw();
			}
			break;
		}
	}

	if (!wnd->CursorEnabled() && controlled != 0)
	{
		if (wnd->kbd.KeyIsPressed(KK_UP))	 GetControlledCamera().Translate({   0,  0, dt });
		if (wnd->kbd.KeyIsPressed(KK_DOWN))  GetControlledCamera().Translate({   0,  0,-dt });
		if (wnd->kbd.KeyIsPressed(KK_LEFT))  GetControlledCamera().Translate({ -dt,  0,  0 });
		if (wnd->kbd.KeyIsPressed(KK_RIGHT)) GetControlledCamera().Translate({  dt,  0,  0 });
	}

	while (const auto delta = wnd->mouse.ReadRawDelta())
	{
		if (!wnd->CursorEnabled() && controlled != 0)
		{
			GetControlledCamera().Rotate(delta->x, delta->y);
		}
	}
#endif
}

void CameraContainer::Submit(size_t channels) const
{
	for (size_t i = 0; i < cameras.size(); i++)
	{
		if (i != active)
		{
			cameras[i]->Submit(channels);
		}
	}
}

void CameraContainer::SpawnWindow(Graphics& gfx)
{
	if (ImGui::Begin("Cameras"))
	{
		if (ImGui::BeginCombo("Active Camera", (*this)->GetName().c_str()))
		{
			for (int i = 0; i < std::size(cameras); i++)
			{
				const bool isSelected = i == active;
				if (ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected))
				{
					active = i;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Controlled Camera", GetControlledCamera().GetName().c_str()))
		{
			for (int i = 0; i < std::size(cameras); i++)
			{
				const bool isSelected = i == controlled;
				if (ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected))
				{
					controlled = i;
				}
			}
			ImGui::EndCombo();
		}

		GetControlledCamera().SpawnControlWidgets(gfx);
	}
	ImGui::End();
}

void CameraContainer::LinkTechniques(Rgph::RenderGraph& rg)
{
	for (auto& pcam : cameras)
	{
		pcam->LinkTechniques(rg);
	}
}

Camera& CameraContainer::GetActiveCamera()
{
	return *cameras[active];
}

Camera& CameraContainer::GetControlledCamera()
{
	return *cameras[controlled];
}