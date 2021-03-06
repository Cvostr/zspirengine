#include "../../headers/world/Terrain.hpp"
#include "../../headers/render/Renderer.hpp"
#include "../../headers/world/ObjectsComponents/TerrainComponent.hpp"
#include "../../headers/world/ObjectsComponents/MaterialComponent.hpp"
#include <GL/glew.h>

void TerrainData::initGL() {
    meshGL = Engine::allocateMesh();
    meshGL->Init();
}

void TerrainData::destroyGL() {
    meshGL->Destroy();

    this->painting.texture_mask1->Destroy();
    this->painting.texture_mask2->Destroy();
    this->painting.texture_mask3->Destroy();
}

void TerrainData::Draw(bool picking, unsigned int instances) {
    //if opengl data not generated, exit function
    if (!created) return;
    //small optimization in terrain painting
    if (!picking) {
        painting.texture_mask1->Use(24);
        painting.texture_mask2->Use(25);
        painting.texture_mask3->Use(26);
    }
    
    meshGL->DrawInstanced(instances);
}

void TerrainData::updateTextureBuffersGL() {
    //Create texture masks texture
    painting.texture_mask1->LoadTextureFromBufferUByte(painting._texture, static_cast<int>(W), static_cast<int>(H), Engine::TextureFormat::FORMAT_RGBA);
    painting.texture_mask2->LoadTextureFromBufferUByte(painting._texture1, static_cast<int>(W), static_cast<int>(H), Engine::TextureFormat::FORMAT_RGBA);
    painting.texture_mask3->LoadTextureFromBufferUByte(painting._texture2, static_cast<int>(W), static_cast<int>(H), Engine::TextureFormat::FORMAT_RGBA);
}

void TerrainData::updateGeometryBuffersGL() {
    
    meshGL->setMeshData((void*)vertices, (int)sizeof(HeightmapVertex), indices, (unsigned int)(W * H),  (W - 1) * (H - 1) * 2 * 3);
    //Vertex pos 3 floats
    meshGL->_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), nullptr);
    //Vertex UV 2 floats
    meshGL->_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 3));
    //Vertex Normals 3 floats
    meshGL->_glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 5));
    //Vertex Tangent 3 floats
    meshGL->_glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 8));
    //Vertex BiTangent 3 floats
    meshGL->_glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 11));
}

void TerrainData::generateGLMesh() {
    if (W < 1 || H < 1)
        return;
    if (!created)
        initGL();

    updateTextureBuffers();
    updateTextureBuffersGL();

    updateGeometryBuffers();
    updateGeometryBuffersGL();

    created = true;
}

void Engine::TerrainProperty::DrawMesh(Renderer* pipeline, unsigned int instances) {
    //Draw terrain mesh
    data.Draw(false, instances);
    //Draw Grass
    DrawGrass(pipeline);
}

void Engine::TerrainProperty::DrawGrass(Renderer* pipeline) {
    if (pipeline->current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT) {
        //Use grass shader
        pipeline->grass_shader->Use();
        //Bind instanced buffer
        pipeline->instancedTransformBuffer->bind();

        Engine::TransformProperty* t_ptr = (this->go_link.updLinkPtr())->getPropertyPtr<Engine::TransformProperty>();
        //Iterate over all grass types
        for (unsigned int grass_i = 0; grass_i < data.grass.size(); grass_i++) {
            //Get a pointer to grass type
            HeightmapGrass* grass = &data.grass[grass_i];

            //if texture is specified
            if (grass->diffuse)
                //then bind it to slot 0
                grass->diffuse->Use(0);
            //Calc amount of steps 
            unsigned int steps = static_cast<unsigned int>(grass->inst_transform.size()) / INSTANCED_RENDER_BUFFER_SIZE;
            //Amount of steps, that hadn't processed
            unsigned int left_steps = static_cast<unsigned int>(grass->inst_transform.size());
            //if no steps left, then exit function
            if (left_steps < 1)
                return;
            //Iterate over all steps
            for (unsigned int step_i = 0; step_i < steps + 1; step_i++) {
                //Amount of steps, that will processed by this operation
                unsigned int entries = (left_steps >= INSTANCED_RENDER_BUFFER_SIZE) ? INSTANCED_RENDER_BUFFER_SIZE : left_steps;
                //offset of array grass->inst_transform[] to be processed by this step
                unsigned int offset = step_i * INSTANCED_RENDER_BUFFER_SIZE;
                //Reduce amount of steps
                left_steps -= entries;
                //if grass has changed, but arrays weren't prepared for rendering
                if (data.hasGrassChanged == true)
                    //Avoid grass rendering
                    return;
                //Send matrices to instanced buffer
                pipeline->instancedTransformBuffer->writeData(0, entries * sizeof(Mat4),
                    &grass->inst_transform[offset]);
                //Draw grass instanced
                Engine::getGrassMesh()->DrawInstanced(entries);
            }
        }
    }
}

void Engine::TerrainProperty::onRender(Engine::Renderer* pipeline) {
    if (data.hasHeightmapChanged) {
        this->data.updateGeometryBuffersGL();
        data.hasHeightmapChanged = false;
    }

    if (data.hasPaintingChanged) {
        this->data.updateTextureBuffersGL();
        data.hasPaintingChanged = false;
    }

    Engine::MaterialProperty* mat = go_link.updLinkPtr()->getPropertyPtr<Engine::MaterialProperty>();
    if (mat == nullptr) return;

    int dtrue = 1;
    int dfalse = 0;
    //Binding terrain buffer
    pipeline->GetMaterialsUniformBuffer()->bind();
    //Iterate over all textures to use them
    for (unsigned int i = 0; i < static_cast<unsigned int>(this->textures_size); i++) {
        //Get pair pointer
        HeightmapTexturePair* pair = &this->textures[i];
        //Check, if pair has diffuse texture
        if (pair->diffuse != nullptr) {
            //Use diffuse texture
            pair->diffuse->Use(static_cast<int>(i));
            pipeline->GetMaterialsUniformBuffer()->writeData(16 * i, 4, &dtrue);
        }
        else {
            pipeline->GetMaterialsUniformBuffer()->writeData(16 * i, 4, &dfalse);
        }
        //Check, if pair has normal texture
        if (pair->normal != nullptr) {
            //Use normal texture
            pair->normal->Use(static_cast<int>(12 + i));
            pipeline->GetMaterialsUniformBuffer()->writeData(16 * 12 + 16 * i, 4, &dtrue);
        }
        else {
            pipeline->GetMaterialsUniformBuffer()->writeData(16 * 12 + 16 * i, 4, &dfalse);
        }
    }
    //Tell shader, that we rendering terrain in normal mode (non picking)
    pipeline->GetMaterialsUniformBuffer()->writeData(16 * 12 * 2, 4, &dfalse);
    //Send dimensions to buffer
    pipeline->GetMaterialsUniformBuffer()->writeData(16 * 12 * 2 + 4, 4, &this->data.W);
    pipeline->GetMaterialsUniformBuffer()->writeData(16 * 12 * 2 + 8, 4, &this->data.H);

    //Apply material shader
    mat->onRender(pipeline);

}