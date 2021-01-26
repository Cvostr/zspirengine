#include "../../headers/vulkan/VKTexture.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

VkFormat Engine::GetFormatVK(TextureFormat format) {
	VkFormat vk_format = VK_FORMAT_UNDEFINED;
	switch (format) {
	case TextureFormat::FORMAT_R:
		vk_format = VK_FORMAT_R8_UNORM;
		break;
	case TextureFormat::FORMAT_RG:
		vk_format = VK_FORMAT_R8G8_UNORM;
		break;
	case TextureFormat::FORMAT_RGB:
		vk_format = VK_FORMAT_R8G8B8_UNORM;
		break;
	case TextureFormat::FORMAT_RGBA:
		vk_format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case TextureFormat::FORMAT_R16:
		vk_format = VK_FORMAT_R16_UNORM;
		break;
	case TextureFormat::FORMAT_RG16:
		vk_format = VK_FORMAT_R16G16_UNORM;
		break;
	case TextureFormat::FORMAT_RGB16:
		vk_format = VK_FORMAT_R16G16B16_UNORM;
		break;
	case TextureFormat::FORMAT_RGBA16:
		vk_format = VK_FORMAT_R16G16B16A16_UNORM;
		break;
	case TextureFormat::FORMAT_R16F:
		vk_format = VK_FORMAT_R16_SFLOAT;
		break;
	case TextureFormat::FORMAT_RG16F:
		vk_format = VK_FORMAT_R16G16_SFLOAT;
		break;
	case TextureFormat::FORMAT_RGB16F:
		vk_format = VK_FORMAT_R16G16B16_SFLOAT;
		break;
	case TextureFormat::FORMAT_RGBA16F:
		vk_format = VK_FORMAT_R16G16B16A16_SFLOAT;
		break;
	case TextureFormat::FORMAT_DEPTH_24_STENCIL_8:
		vk_format = VK_FORMAT_D24_UNORM_S8_UINT;
		break;
	case TextureFormat::FORMAT_DEPTH_32:
		vk_format = VK_FORMAT_D32_SFLOAT;
		break;
	}

	return vk_format;
}

void Engine::vkTexture::Create(unsigned int Width, unsigned int Height, TextureFormat format, unsigned int Layers) {
	maxWidth = Width;
	maxHeight = Height;
	mFormat = format;
	mLayers = Layers;
	mMipsCount = 1;
	//Convert format from universal to vulkan
	VkFormat TexFormat = GetFormatVK(format);

	VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
	{
		imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
	{
		imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = 0; // Optional
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(maxWidth);
	imageInfo.extent.height = static_cast<uint32_t>(maxHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mMipsCount;
	imageInfo.arrayLayers = Layers;
	imageInfo.format = TexFormat;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = imageLayout;
	imageInfo.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	if (vkCreateImage(game_data->vk_main->mDevice->getVkDevice(), &imageInfo, nullptr, &mImage) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}


	//Get Memory Requirements
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(game_data->vk_main->mDevice->getVkDevice(), mImage, &memRequirements);
	//Get Memory Properties
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(game_data->vk_main->mDevice->getPhysicalDevice(), &memProperties);

	unsigned int memoryType = 0;
	VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
			memoryType = i;
		}
	}

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryType;
	//Allocate Memory
	if (vkAllocateMemory(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, nullptr, &mImageMem) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(game_data->vk_main->mDevice->getVkDevice(), mImage, mImageMem, 0);

	VkImageViewType ImageViewType = VK_IMAGE_VIEW_TYPE_2D;
	if (mLayers > 1)
		ImageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

	VkImageViewCreateInfo textureImageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	textureImageViewInfo.image = mImage;
	textureImageViewInfo.viewType = ImageViewType;
	textureImageViewInfo.format = TexFormat;
	textureImageViewInfo.subresourceRange.aspectMask = aspect;
	textureImageViewInfo.subresourceRange.baseMipLevel = 0;
	textureImageViewInfo.subresourceRange.levelCount = 1;
	textureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	textureImageViewInfo.subresourceRange.layerCount = mLayers;
	vkCreateImageView(game_data->vk_main->mDevice->getVkDevice(), &textureImageViewInfo, nullptr, &mImageView);

}

//Loads texture from buffer
bool Engine::vkTexture::LoadDDSTextureFromBuffer(unsigned char* data){

	maxHeight = *(reinterpret_cast<int*>(&(data[12]))); //Getting height of texture in px info
	maxWidth = *(reinterpret_cast<int*>(&(data[16]))); //Getting width of texture in px info
	unsigned int linearSize = *(reinterpret_cast<unsigned int*>(&(data[20])));
	mMipsCount = *(reinterpret_cast<unsigned int*>(&(data[28])));
	unsigned int fourCC = *(reinterpret_cast<unsigned int*>(&(data[84])));


	unsigned int bufsize;
	bufsize = mMipsCount > 1 ? linearSize * 2 : linearSize;//Getting buffer size
	unsigned char* bufferT = data + 128; //jumping over header

	VkFormat format; //Getting texture format
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		mFormat = FORMAT_BC1_UNORM;
		break;
	case FOURCC_DXT3:
		format = VK_FORMAT_BC2_UNORM_BLOCK;
		mFormat = FORMAT_BC2_UNORM;
		break;
	case FOURCC_DXT5:
		format = VK_FORMAT_BC3_UNORM_BLOCK;
		mFormat = FORMAT_BC3_UNORM;
		break;
	default:

		return 0;
	}
	//Getting block size
	unsigned int blockSize = (format == VK_FORMAT_BC1_RGBA_UNORM_BLOCK) ? 8 : 16;

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = 0; // Optional
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(maxWidth);
	imageInfo.extent.height = static_cast<uint32_t>(maxHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mMipsCount;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	if (vkCreateImage(game_data->vk_main->mDevice->getVkDevice(), &imageInfo, nullptr, &mImage) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}


	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(game_data->vk_main->mDevice->getVkDevice(), mImage, &memRequirements);

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(game_data->vk_main->mDevice->getPhysicalDevice(), &memProperties);

	unsigned int memoryType = 0;
	VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
			memoryType = i;
		}
	}

	

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryType;

	if (vkAllocateMemory(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, nullptr, &mImageMem) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(game_data->vk_main->mDevice->getVkDevice(), mImage, mImageMem, 0);

	unsigned int size = ((maxWidth + 3) / 4) * ((maxHeight + 3) / 4) * blockSize;

	VmaVkBuffer temp_buf;
	void* temp_map;
	game_data->vk_main->mVMA->allocateCpu(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &temp_buf, size, &temp_map);
	

	unsigned int offset = 0;
	int nwidth = maxWidth;
	int nheight = maxHeight;
	

	for (unsigned int level = 0; level < mMipsCount && (nwidth || nheight); ++level) //Iterating over mipmaps
	{
		size = ((nwidth + 3) / 4) * ((nheight + 3) / 4) * blockSize; //Calculating mip texture size
		
		game_data->vk_main->mVMA->map(&temp_buf, &temp_map);
		memcpy(temp_map, bufferT + offset, size);
		game_data->vk_main->mVMA->unmap(&temp_buf);

		Transition(temp_buf, level, nwidth, nheight);

		offset += size;
		nwidth /= 2;
		nheight /= 2;
	}

	game_data->vk_main->mVMA->unmap(&temp_buf);
	//Free temporary buffer
	game_data->vk_main->mVMA->destroyBuffer(&temp_buf);
	//Create image view
	CreateImageView(format);
    
	mCreated = true;

    return true;
}
//Use in rendering pipeline
void Engine::vkTexture::Use(int slot){
	
}
void Engine::vkTexture::Destroy(){
	if (mCreated) {
		VkDevice device = game_data->vk_main->mDevice->getVkDevice();
		vkDestroyImageView(device, mImageView, nullptr);
		vkDestroyImage(device, mImage, nullptr);
		vkFreeMemory(device, mImageMem, nullptr);
		mCreated = false;
	}
}

void Engine::vkTexture::Transition(VmaVkBuffer temp, unsigned int MipLevel, uint32_t Width, uint32_t Height) {
	VkCommandBuffer cmdbuf = game_data->vk_main->mVMA->GetSingleTimeCmdBuf();
	VkCommandPool cmdpool = game_data->vk_main->mVMA->GetSingleTimeCmdPool();

	VkImageMemoryBarrier imgMemBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	imgMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgMemBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imgMemBarrier.subresourceRange.baseMipLevel = 0;
	imgMemBarrier.subresourceRange.levelCount = 1;
	imgMemBarrier.subresourceRange.baseArrayLayer = 0;
	imgMemBarrier.subresourceRange.layerCount = 1;
	imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imgMemBarrier.image = mImage;
	imgMemBarrier.srcAccessMask = 0;
	imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdbuf, &beginInfo);

	vkCmdPipelineBarrier(
		cmdbuf,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imgMemBarrier);

	VkBufferImageCopy region = {};
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.mipLevel = MipLevel;
	region.imageExtent.width = Width;
	region.imageExtent.height = Height;
	region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(cmdbuf, temp.Buffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imgMemBarrier.image = mImage;
	imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		cmdbuf,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imgMemBarrier);


	//Run command
	vkEndCommandBuffer(cmdbuf);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdbuf;

	vkQueueSubmit(game_data->vk_main->mDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(game_data->vk_main->mDevice->GetGraphicsQueue());
}


void Engine::vkTexture::CreateImageView(VkFormat format) {

	VkImageViewCreateInfo textureImageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	textureImageViewInfo.image = mImage;
	textureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	textureImageViewInfo.format = format;
	textureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	textureImageViewInfo.subresourceRange.baseMipLevel = 0;
	textureImageViewInfo.subresourceRange.levelCount = 1;
	textureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	textureImageViewInfo.subresourceRange.layerCount = 1;
	vkCreateImageView(game_data->vk_main->mDevice->getVkDevice(), &textureImageViewInfo, nullptr, &mImageView);
}


Engine::vkTexture::vkTexture(){

}
Engine::vkTexture::~vkTexture(){

}


void Engine::vkTexture3D::Init(){

}
bool Engine::vkTexture3D::pushTextureBuffer(int index, unsigned char* data){
    return true;
}
//Use in rendering pipeline
void Engine::vkTexture3D::Use(int slot){

}
void Engine::vkTexture3D::Destroy(){

}

Engine::vkTexture3D::vkTexture3D(){

}
Engine::vkTexture3D::~vkTexture3D(){

}
