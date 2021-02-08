#include "../../headers/ogl/GLTextureSampler.hpp"
#include <GL/glew.h>

GLint GetSamplerFilterGL(Engine::TextureFilteringMode TFM) {
	GLint result = 0;

	if (TFM == Engine::SAMPLER_FILTERING_LINEAR)
		result = GL_LINEAR;
	if (TFM == Engine::SAMPLER_FILTERING_NEAREST)
		result = GL_NEAREST;

	return result;
}

GLint GetSamplerWrapMode(Engine::TextureWrapMode TWM) {
	GLint result = 0;

	if (TWM == Engine::SAMPLER_WRAP_REPEAT)
		result = GL_REPEAT;
	if (TWM == Engine::SAMPLER_WRAP_MIRRORED_REPEAT)
		result = GL_MIRRORED_REPEAT;
	if (TWM == Engine::SAMPLER_WRAP_CLAMP_TO_EDGE)
		result = GL_CLAMP_TO_EDGE;

	return result;
}

void Engine::glTextureSampler::CreateSampler() {
	glCreateSamplers(1, &this->mGLSampler);

	glSamplerParameteri(mGLSampler, GL_TEXTURE_MIN_FILTER, GetSamplerFilterGL(mMinFiltering));
	glSamplerParameteri(mGLSampler, GL_TEXTURE_MAG_FILTER, GetSamplerFilterGL(mMagFiltering));

	glSamplerParameteri(mGLSampler, GL_TEXTURE_WRAP_S, GetSamplerWrapMode(mWrapU));
	glSamplerParameteri(mGLSampler, GL_TEXTURE_WRAP_T, GetSamplerWrapMode(mWrapV));
	glSamplerParameteri(mGLSampler, GL_TEXTURE_WRAP_R, GetSamplerWrapMode(mWrapW));

	glSamplerParameterf(mGLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, mMaxAnisotropy);

	mCreated = true;
}

void Engine::glTextureSampler::Destroy() {
	if (mCreated) {
		glDeleteSamplers(1, &mGLSampler);
		mCreated = false;
	}
}