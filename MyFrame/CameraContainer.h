#pragma once
#include "Window.h"
#include <vector>
#include <memory>

class Camera;
class Graphics;
namespace Rgph
{
	class RenderGraph;
}

class CameraContainer
{
public:
	CameraContainer(Graphics& gfx);
	void Bind( Graphics& gfx );
	void AddCamera( std::shared_ptr<Camera> pCam );
	Camera* operator->();
	~CameraContainer();
	void Update(float dt, DirectX::XMFLOAT3 pos, Window* wnd) noexcept;
	void Submit(size_t channels) const;
	void SpawnWindow(Graphics& gfx);
	void LinkTechniques(Rgph::RenderGraph& rg);
	Camera& GetActiveCamera();
private:
	Camera& GetControlledCamera();
private:
	std::vector<std::shared_ptr<Camera>> cameras;
	int active = 0;
	int controlled = 0;

	// Camera Follow
	float FollowRadius = 40.0f;
	float FollowPitch = DirectX::XM_PI / 6.0f;
	float FollowLerp = 0.1f;
	DirectX::XMFLOAT3 FollowTarget{ 0.0f,0.0f,0.0f };
};