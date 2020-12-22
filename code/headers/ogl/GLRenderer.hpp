#pragma once

#include "GLFramebuffer.hpp"
#include "../render/Renderer.hpp"

namespace Engine {
	class GLRenderer : public Renderer {
	public:
		void InitShaders();
		void initManager();

		void DrawObject(Engine::GameObject* obj);
		void render2D();
		void render3D(Engine::Camera* cam);

		void ClearFBufferGL(bool clearColor, bool clearDepth);
		void setClearColor(float r, float g, float b, float a);

		void setBlendingState(bool blend);
		void setDepthState(bool depth);
		void setFaceCullState(bool face_cull);
		void setFullscreenViewport(unsigned int Width, unsigned int Height);

		void create_G_Buffer_GL(unsigned int width, unsigned int height);

		void OnUpdateWindowSize(int W, int H);

		GLRenderer();
	};
}