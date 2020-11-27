#pragma once

#include "../render/Renderer.hpp"

namespace Engine {
	class GLRenderer : public Renderer {
	public:
		void InitShaders();
		void initManager();

		void render2D();
		void render3D(Engine::Camera* cam);

		GLRenderer();
	};
}