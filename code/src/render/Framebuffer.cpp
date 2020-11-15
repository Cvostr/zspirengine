#include "../../headers/render/Framebuffer.hpp"

void Engine::GLframebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbuffer);
    glViewport(0, 0, Width, Height);
}

void Engine::GLframebuffer::bindTextures(unsigned int m) {
    for (unsigned int t = 0; t < texture_size; t++) {
        glActiveTexture(GL_TEXTURE0 + m + t);
        glBindTexture(GL_TEXTURE_2D, textures[t]);
    }
}

void Engine::GLframebuffer::addTexture(GLint intFormat, GLint format) {
    if (texture_size == MAX_RENDERER_ATTACHMENT_COUNT) return;
    bind();
    //Create texture
    glGenTextures(1, &textures[texture_size]);
    glBindTexture(GL_TEXTURE_2D, textures[texture_size]);
    glTexImage2D(GL_TEXTURE_2D, 0, intFormat, Width, Height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texture_size, GL_TEXTURE_2D, textures[texture_size], 0);
    texture_size++; //Add texture

    unsigned int attachments[MAX_RENDERER_ATTACHMENT_COUNT] =
    { GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6 };

    glDrawBuffers(texture_size, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Engine::GLframebuffer::~GLframebuffer() {
    if (Depth)
        glDeleteRenderbuffers(1, &this->depthBuffer);
    glDeleteFramebuffers(1, &this->fbuffer);

    for (unsigned int t = 0; t < texture_size; t++) {
        glDeleteTextures(1, &textures[t]);
    }
}
Engine::GLframebuffer::GLframebuffer(unsigned int width, unsigned int height, bool depth) {
    textures[0] = 0;
    this->Width = width;
    this->Height = height;

    this->Depth = depth;
    this->depthBuffer = 0;
    texture_size = 0;

    glGenFramebuffers(1, &fbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, fbuffer);
    if (depth) {
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default
}
