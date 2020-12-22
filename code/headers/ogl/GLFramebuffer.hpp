#pragma once

#include "../render/Renderer.hpp"

namespace Engine {

    class GLframebuffer : public Framebuffer{
    private:
        GLuint fbuffer;
        GLuint depthBuffer;

        unsigned int texture_size;

        unsigned int Width;
        unsigned int Height;

        bool Depth;

    public:

        GLuint textures[MAX_RENDERER_ATTACHMENT_COUNT];

        void bind();

        void bindTextures(unsigned int m);

        void addTexture(GLint intFormat = GL_RGBA8, GLint format = GL_RGBA);

        ~GLframebuffer();
        GLframebuffer(unsigned int width, unsigned int height, bool depth);
    };
}