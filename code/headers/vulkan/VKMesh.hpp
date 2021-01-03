#pragma once

#include "../render/Mesh.hpp"
#include <vulkan/vulkan.hpp>
#include "ZSVMA.hpp"

namespace Engine {
    class vkMesh : public Engine::Mesh {
    private:
        VmaVkBuffer vertexBuffer;
        VmaVkBuffer indexBuffer;
    public:
        void Init();
        void setMeshData(ZSVERTEX* vertices, unsigned int* indices, unsigned int vertices_num, unsigned int indices_num);
        void setMeshData(ZSVERTEX* vertices, unsigned int vertices_num);
        void Draw(VkCommandBuffer CmdBuf);
        void Draw();
        void DrawInstanced(VkCommandBuffer CmdBuf, unsigned int instances);
        void DrawInstanced(unsigned int instances);
        void Destroy();

        vkMesh();
        ~vkMesh();
    };
}