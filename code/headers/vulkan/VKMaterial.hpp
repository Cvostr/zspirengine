#include "../render/Material.hpp"


class VKMaterialTemplate : public MaterialTemplate {
public:
	Engine::ZSVulkanPipeline* Pipeline;
	bool mPipelineCreated;

	void CreatePipeline();
	void MakeDescrSetUniform(Engine::ZSVulkanDescriptorSet* DescrSet);
	void MakeDescrSetTextures(Engine::ZSVulkanDescriptorSet* DescrSet);
	void MakeDescrSetStorage(Engine::ZSVulkanDescriptorSet* DescrSet);

	VKMaterialTemplate(Engine::Shader* shader, unsigned int UB_ConnectID, unsigned int UB_SIZE) :
		MaterialTemplate(shader, UB_ConnectID, UB_SIZE),
		Pipeline(nullptr),
		mPipelineCreated(false)
	{}
};

class VKMaterial : public Material {
public:
	Engine::ZSVulkanDescriptorSet* DescrSetTextures;

	void CreateDescriptors();
	void SetTexture(unsigned int slot, VkImageView view, Engine::ZSVulkanSampler* sampler);

	VKMaterial(MaterialTemplate* Template):
		Material(Template)
	{}
	VKMaterial() :
		Material()
	{}

};