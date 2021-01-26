#include "../../headers/engine/Resources.hpp"

using namespace Engine;

RenderTextureResource::RenderTextureResource() {

}
RenderTextureResource::~RenderTextureResource() {

}

void RenderTextureResource::load() {
	mFramebuffer = allocFramebuffer(512, 512);

	mFramebuffer->AddTexture(TextureFormat::FORMAT_RGBA);
}