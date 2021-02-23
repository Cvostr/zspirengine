#include <ogl/GLFramebuffer.hpp>

unsigned int attachments[MAX_RENDERER_ATTACHMENT_COUNT] =
{ GL_COLOR_ATTACHMENT0,
GL_COLOR_ATTACHMENT1,
GL_COLOR_ATTACHMENT2,
GL_COLOR_ATTACHMENT3,
GL_COLOR_ATTACHMENT4,
GL_COLOR_ATTACHMENT5,
GL_COLOR_ATTACHMENT6,
GL_COLOR_ATTACHMENT7 };

void Engine::GLframebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

void Engine::GLframebuffer::bindTextures(unsigned int m) {
    for (unsigned int t = 0; t < mTexturesCount; t++) {
        textures[t]->Use(t);
    }

    if (Depth)
        depthTexture->Use(mTexturesCount);
}

void Engine::GLframebuffer::AddTexture(TextureFormat Format) {
    AddTexture(Width, Height, Format);
}
void Engine::GLframebuffer::AddDepth(unsigned int Layers, TextureFormat Format) {
    AddDepth(Width, Height, Layers, Format);
}


void Engine::GLframebuffer::AddTexture(uint32_t Width, uint32_t Height, TextureFormat Format) {
    if (mTexturesCount == MAX_RENDERER_ATTACHMENT_COUNT) return;
    //Create texture
    Texture* Attachment = allocTexture();
    Attachment->SetRenderTargetFlag(true);
    Attachment->Create(Width, Height, Format);
    
    AddTexture(Attachment);
}

void Engine::GLframebuffer::AddDepth(uint32_t Width, uint32_t Height, unsigned int Layers, TextureFormat Format) {
    if (!Depth) {
        Depth = true;

        depthTexture = new glTexture;
        depthTexture->SetRenderTargetFlag(true);
        depthTexture->Create(Width, Height, Format, Layers);
    }
    /*
    glTexParameteri(Type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(Type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(Type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(Type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(Type, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(Type, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
    */

}

void Engine::GLframebuffer::SetSize(uint32_t Width, uint32_t Height) {
    Framebuffer::SetSize(Width, Height);

    //Resize all textures
    for (unsigned int t = 0; t < mTexturesCount; t++) {
        textures[t]->Resize(Width, Height);
    }
    //Resize depth, if exists
    if (Depth) {
        depthTexture->Resize(Width, Height);
    }

    //if framebuffer already created
    if (mCreated) {
        //Destroy current framebuffer
        glDeleteFramebuffers(1, &this->mFramebuffer);

        mCreated = false;
        Create();
    }
}

void Engine::GLframebuffer::AddTexture(Texture* Texture) {
    if (mTexturesCount == MAX_RENDERER_ATTACHMENT_COUNT || !Texture->IsRenderTarget()) return;
    //Assign texture to array
    textures[mTexturesCount] = Texture;
    //Increase texture count
    mTexturesCount++; 
}

void Engine::GLframebuffer::Create() {
    if (!mCreated) {
        glGenFramebuffers(1, &mFramebuffer);
        bind();
        //Push all color attachments to opengl framebuffer
        for (unsigned int texture_i = 0; texture_i < mTexturesCount; texture_i++) {
            glTexture* Attachment = static_cast<glTexture*>(textures[texture_i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texture_i,
                GL_TEXTURE_2D, Attachment->TEXTURE_ID, 0);

        }
        //Submit color attachments
        glDrawBuffers(mTexturesCount, attachments);
        //Push depth attachment, if exists
        if (Depth) {
            glTexture* DepthAttachment = static_cast<glTexture*>(depthTexture);

            GLenum Type = ((glTexture*)depthTexture)->GetGlType();
            GLenum AttachmentType = GL_DEPTH_ATTACHMENT;

            if (depthTexture->GetFormat() == TextureFormat::FORMAT_DEPTH_24_STENCIL_8)
                AttachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
            //Attach depth texture to framebuffer
            glFramebufferTexture(GL_FRAMEBUFFER, AttachmentType, DepthAttachment->TEXTURE_ID, 0);
        }
        //Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default

        mCreated = true;
    }
}

void Engine::GLframebuffer::Destroy() {
    if (mCreated) {
        //Destroy opengl framebuffer
        glDeleteFramebuffers(1, &this->mFramebuffer);

        for (unsigned int t = 0; t < mTexturesCount; t++) {
            if(!textures[t]->IsRenderTargetResource())
                textures[t]->Destroy();
        }

        if (Depth) {
            depthTexture->Destroy();
        }
        //unset flag
        mCreated = false;
    }
}

Engine::GLframebuffer::GLframebuffer(unsigned int width, unsigned int height)
{
    textures[0] = 0;
    Width = width;
    Height = height;
}

Engine::GLframebuffer::~GLframebuffer() {
    Destroy();

    for (unsigned int t = 0; t < mTexturesCount; t++) {
        if (!textures[t]->IsRenderTargetResource())
            delete textures[t];
    }

    if (Depth) {
        delete depthTexture;
    }
}

Engine::GLframebuffer::GLframebuffer() {
     
}