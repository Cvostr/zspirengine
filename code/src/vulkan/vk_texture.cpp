#include "../../headers/vulkan/vk_data.h"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

void Engine::_vk_Texture::Init(){

}
//Loads texture from buffer
bool Engine::_vk_Texture::LoadDDSTextureFromBuffer(unsigned char* data){

	maxHeight = *(reinterpret_cast<int*>(&(data[12]))); //Getting height of texture in px info
	maxWidth = *(reinterpret_cast<int*>(&(data[16]))); //Getting width of texture in px info
	unsigned int linearSize = *(reinterpret_cast<unsigned int*>(&(data[20])));
	unsigned int mipMapCount = *(reinterpret_cast<unsigned int*>(&(data[28])));
	unsigned int fourCC = *(reinterpret_cast<unsigned int*>(&(data[84])));


	unsigned int bufsize;
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;//Getting buffer size
	unsigned char* bufferT = data + 128; //jumping over header

	VkFormat format; //Getting texture format
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = VK_FORMAT_BC1_RGB_SRGB_BLOCK;
		break;
	case FOURCC_DXT3:
		format = VK_FORMAT_BC2_SRGB_BLOCK;
		break;
	case FOURCC_DXT5:
		format = VK_FORMAT_BC3_SRGB_BLOCK;
		break;
	default:

		return 0;
	}
	//Getting block size
	unsigned int blockSize = (format == VK_FORMAT_BC1_RGB_SRGB_BLOCK) ? 8 : 16;
	unsigned int offset = 0;

	unsigned int size = ((maxWidth + 3) / 4) * ((maxHeight + 3) / 4) * blockSize;

	VmaVkBuffer temp_buf;
	void* temp_map;
	game_data->vk_main->mVMA->allocateCpu(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &temp_buf, size, &temp_map);
	memcpy(temp_map, bufferT, size);
	game_data->vk_main->mVMA->unmap(&temp_buf);

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = 0; // Optional
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(maxWidth);
	imageInfo.extent.height = static_cast<uint32_t>(maxHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
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

	VkDeviceMemory tmem;

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryType;

	if (vkAllocateMemory(game_data->vk_main->mDevice->getVkDevice(), &allocInfo, nullptr, &tmem) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(game_data->vk_main->mDevice->getVkDevice(), mImage, tmem, 0);



	Transition(temp_buf);
	//Free temporary buffer
	game_data->vk_main->mVMA->destroyBuffer(&temp_buf);
	//Create image view
	CreateImageView(format);
    
    return true;
}
//Use in rendering pipeline
void Engine::_vk_Texture::Use(int slot){
	
}
void Engine::_vk_Texture::Destroy(){

}

void Engine::_vk_Texture::Transition(VmaVkBuffer temp) {
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
	region.imageExtent.width = maxWidth;
	region.imageExtent.height = maxHeight;
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


void Engine::_vk_Texture::CreateImageView(VkFormat format) {

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


Engine::_vk_Texture::_vk_Texture(){

}
Engine::_vk_Texture::~_vk_Texture(){

}


void Engine::_vk_Texture3D::Init(){

}
bool Engine::_vk_Texture3D::pushTextureBuffer(int index, unsigned char* data){
    return true;
}
//Use in rendering pipeline
void Engine::_vk_Texture3D::Use(int slot){

}
void Engine::_vk_Texture3D::Destroy(){

}

Engine::_vk_Texture3D::_vk_Texture3D(){

}
Engine::_vk_Texture3D::~_vk_Texture3D(){

}
