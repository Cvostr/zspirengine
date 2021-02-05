#pragma once

#define GLEW_STATIC

#include "GLFramebuffer.hpp"
#include "GLScreenEffect.hpp"
#include "../render/Renderer.hpp"
#include <GL/glew.h>

namespace Engine {
	class GLRenderer : public Renderer {
	public:

		GLScreenEffect* effect;

		GLScreenEffect* CopyEffect;

		void InitShaders();
		void initManager();

		void DrawObject(Engine::GameObject* obj);
		void render2D();
		void render3D(Engine::Camera* cam);

		void Render3DCamera(void* cam_prop);


		void ClearFBufferGL(bool clearColor, bool clearDepth);
		void setClearColor(float r, float g, float b, float a);

		void setBlendingState(bool blend);
		void setDepthState(bool depth);
		void setFaceCullState(bool face_cull);
		void setFullscreenViewport(unsigned int Width, unsigned int Height);
		void setFrontFace(CameraCullFaceDirection CCFD);

		void create_G_Buffer_GL(unsigned int width, unsigned int height);

		void OnUpdateWindowSize(int W, int H);

		GLRenderer();
	};
}