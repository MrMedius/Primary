#include "CameraContainer.h"
#include "imgui/imgui.h"
#include "Camera.h"
#include "Graphics.h"
#include "RenderGraph.h"

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

void CameraContainer::Update(float dt, Window* wnd) noexcept
{
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

	if (!wnd->CursorEnabled())
	{
		if (wnd->kbd.KeyIsPressed('W')) GetControlledCamera().Translate({   0,  0, dt });
		if (wnd->kbd.KeyIsPressed('S')) GetControlledCamera().Translate({   0,  0,-dt });
		if (wnd->kbd.KeyIsPressed('A')) GetControlledCamera().Translate({ -dt,  0,  0 });
		if (wnd->kbd.KeyIsPressed('D')) GetControlledCamera().Translate({  dt,  0,  0 });
		if (wnd->kbd.KeyIsPressed('R')) GetControlledCamera().Translate({   0, dt,  0 });
		if (wnd->kbd.KeyIsPressed('F')) GetControlledCamera().Translate({   0,-dt,  0 });
	}

	while (const auto delta = wnd->mouse.ReadRawDelta())
	{
		if (!wnd->CursorEnabled())
		{
			GetControlledCamera().Rotate(delta->x, delta->y);
		}
	}
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