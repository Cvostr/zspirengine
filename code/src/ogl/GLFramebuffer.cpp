#include "../../headers/ogl/GLFramebuffer.hpp"

void Engine::GLframebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glViewport(0, 0, Width, Height);
}

void Engine::GLframebuffer::bindTextures(unsigned int m) {
    for (unsigned int t = 0; t < mTexturesCount; t++) {
        textures[t]->Use(m + t);
    }
}

void Engine::GLframebuffer::AddTexture(TextureFormat Format) {
    AddTexture(Width, Height, Format);
}
void Engine::GLframebuffer::AddDepth(unsigned int Layers, TextureFormat Format) {
    AddDepth(Width, Height, Layers, Format);
}


void Engine::GLframebuffer::AddTexture(uint32_t Width, uint32_t Height, TextureFormat Format) {
    if (mTexturesCount == MAX_RENDERER_ATTACHMENT_COUNT) return;
    bind();
    //Create texture
    textures[mTexturesCount] = allocTexture();
    textures[mTexturesCount]->Create(Width, Height, Format);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + mTexturesCount, GL_TEXTURE_2D, ((glTexture*)textures[mTexturesCount])->TEXTURE_ID, 0);
    mTexturesCount++; //Add texture

    unsigned int attachments[MAX_RENDERER_ATTACHMENT_COUNT] =
    { GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6 };

    glDrawBuffers(mTexturesCount, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Engine::GLframebuffer::~GLframebuffer() {

    glDeleteFramebuffers(1, &this->mFramebuffer);

    for (unsigned int t = 0; t < mTexturesCount; t++) {
        textures[t]->Destroy();
        delete textures[t];
    }

    if (Depth) {
        depthTexture->Destroy();
        delete depthTexture;
    }
}

void Engine::GLframebuffer::AddDepth(uint32_t Width, uint32_t Height, unsigned int Layers, TextureFormat Format) {
    Depth = true;

    bind();
    
    depthTexture = new glTexture;
    depthTexture->Create(Width, Height, Format, Layers);

    GLenum Type = ((glTexture*)depthTexture)->GetGlType();
    GLenum AttachmentType = GL_DEPTH_ATTACHMENT;

    if (Format == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
        AttachmentType = GL_DEPTH_STENCIL_ATTACHMENT;

    glTexParameteri(Type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(Type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(Type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(Type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(Type, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(Type, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

    glFramebufferTexture(GL_FRAMEBUFFER, AttachmentType, ((glTexture*)depthTexture)->TEXTURE_ID, 0);
}

Engine::GLframebuffer::GLframebuffer(unsigned int width, unsigned int height)

{
    textures[0] = 0;
    Width = width;
    Height = height;
    mTexturesCount = 0;

    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default
}

Engine::GLframebuffer::GLframebuffer() {
    textures[0] = 0;
    Width = 0;
    Height = 0;
    mTexturesCount = 0;

    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default
}