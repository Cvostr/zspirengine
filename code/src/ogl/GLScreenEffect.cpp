#include "../../headers/ogl/GLScreenEffect.hpp"
#include "../../headers/ogl/GLTexture.hpp"

void Engine::GLScreenEffect::Compute() {
	for (size_t i = 0; i < this->mInputTextures.size(); i++) {
		glTexture* tex = static_cast<glTexture*>(mInputTextures[i]);

        glBindImageTexture(
            static_cast<GLuint>(i),                          //Always bind to slot 0
            tex->TEXTURE_ID,
            0,
            GL_FALSE,
            0,
            GL_READ_ONLY,               //Only read from this texture
            GetInternalFormatGL(tex->GetFormat())
        );
	}

    glTexture* output = static_cast<glTexture*>(OutputTexture);

    glBindImageTexture(
        this->mInputTextures.size(),                          //Always bind to slot 1
        output->TEXTURE_ID,
        0,
        GL_FALSE,
        0,
        GL_WRITE_ONLY,              //Only write to this texture
        GetInternalFormatGL(output->GetFormat())
    );

    //Use shader
    this->mEffectShader->Use();

    glDispatchCompute(mWidth / mLocalSizeX, mHeight / mLocalSizeY, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}