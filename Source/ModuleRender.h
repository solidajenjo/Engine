#ifndef __ModuleRender_h__
#define __ModuleRender_h__

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	bool Start() override;
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void OnResize();
	void DrawGUI();

private:

	void DrawGizmos() const;
	void ViewMatrix() const;
	void ProjectionMatrix() const;
	float4x4 LookAt(math::float3 OBS, math::float3 VRP, math::float3 up) const;
	void CreateFrameBuffer();
	void CreateBlockUniforms();
	void DrawLines() const;
	void DrawAxis() const;
	void InitFrustum();
	void InitSDL();
	void InitOpenGL() const;

public:
	void* context = nullptr;
	Frustum frustum;
	unsigned int renderTexture = 0;

private:
	unsigned int FBO = 0;
	unsigned int RBO = 0;
	unsigned int UBO = 0;
	bool useCheckersTexture = false;
	bool depthTest = true;
	bool wireframe = false;
	bool boundingBox = false;
	bool vsync = false;
};

#endif /* __ModuleRender_h__ */
