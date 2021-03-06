#include "../render/Material.hpp"


class VKMaterialTemplate : public MaterialTemplate {
public:
	Engine::ZSVulkanPipeline* Pipeline;
	bool mPipelineCreated;
	uint32_t BufferSize;

	void CreatePipeline();
	void MakeDescrSetUniform(Engine::ZSVulkanDescriptorSet* DescrSet);
	void MakeDescrSetTextures(Engine::ZSVulkanDescriptorSet* DescrSet);
	void MakeDescrSetStorage(Engine::ZSVulkanDescriptorSet* DescrSet);

	VKMaterialTemplate(Engine::Shader* shader, unsigned int UB_SIZE) :
		MaterialTemplate(shader, UB_SIZE),
		Pipeline(nullptr),
		mPipelineCreated(false),
		BufferSize(UB_SIZE)
	{}
};

class VKMaterial : public Material {
public:
	Engine::ZSVulkanDescriptorSet* DescrSetTextures;
	char* MaterialBuffer;

	void CreateDescriptors();
	void SetTexture(unsigned int slot, VkImageView view, Engine::ZSVulkanSampler* sampler);

	void CreateBuffer(uint32_t Size);

	void WriteToBuffer(unsigned int offset, unsigned int size, char* data);

	VKMaterial(MaterialTemplate* Template) :
		Material(Template)
		, DescrSetTextures(nullptr)
	{
		VKMaterialTemplate* VKTemplate = static_cast<VKMaterialTemplate*>(Template);
		CreateBuffer(VKTemplate->BufferSize);
	}
	VKMaterial() :
		Material()
		, DescrSetTextures(nullptr),
		MaterialBuffer(nullptr)
	{}

	~VKMaterial() {
		if (MaterialBuffer != nullptr)
			 delete[] MaterialBuffer;
	}

};