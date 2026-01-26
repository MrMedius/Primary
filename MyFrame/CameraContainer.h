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
	void Bind( Graphics& gfx );
	void AddCamera( std::shared_ptr<Camera> pCam );
	Camera* operator->();
	~CameraContainer();
	void Update(float dt, Window* wnd) noexcept;
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
};