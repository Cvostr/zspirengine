#include "../../headers/world/Terrain.hpp"
#include "../../headers/render/zs-pipeline.h"
#include "../../headers/world/go_properties.h"
#include <GL/glew.h>

void TerrainMeshGL::generate() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
}
void TerrainMeshGL::destroy() {
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

void TerrainData::initGL() {
    meshGL.generate();

    glGenTextures(1, &this->painting.texture_mask1);
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask1);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &this->painting.texture_mask2);
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask2);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &this->painting.texture_mask3);
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask3);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TerrainData::destroyGL() {
    meshGL.destroy();
}

void TerrainData::Draw(bool picking) {
    //if opengl data not generated, exit function
    if (!created) return;
    //small optimization in terrain painting
    if (!picking) {
        glActiveTexture(GL_TEXTURE24);
        glBindTexture(GL_TEXTURE_2D, painting.texture_mask1);
        glActiveTexture(GL_TEXTURE25);
        glBindTexture(GL_TEXTURE_2D, painting.texture_mask2);
        glActiveTexture(GL_TEXTURE26);
        glBindTexture(GL_TEXTURE_2D, painting.texture_mask3);
    }
    glBindVertexArray(meshGL.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshGL.EBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshGL.VBO);
    glDrawElements(GL_TRIANGLES, (W - 1) * (H - 1) * 2 * 3, GL_UNSIGNED_INT, nullptr);
}

void TerrainData::updateTextureBuffersGL() {
    //Create texture masks texture
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask1);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        static_cast<int>(W),
        static_cast<int>(H),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        painting._texture
    );
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask2);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        static_cast<int>(W),
        static_cast<int>(H),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        painting._texture1
    );
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask3);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        static_cast<int>(W),
        static_cast<int>(H),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        painting._texture2
    );

}

void TerrainData::updateGeometryBuffersGL() {
    glBindVertexArray(meshGL.VAO); //Bind vertex array
    glBindBuffer(GL_ARRAY_BUFFER, meshGL.VBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(W * H)* static_cast<int>(sizeof(HeightmapVertex)), vertices, GL_STATIC_DRAW); //send vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshGL.EBO); //Bind index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>((W - 1) * (H - 1) * 2 * 3) * sizeof(unsigned int), indices, GL_STATIC_DRAW); //Send indices to buffer

    //Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), nullptr);
    glEnableVertexAttribArray(0);
    //Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    //Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 5));
    glEnableVertexAttribArray(2);
    //Vertex Tangent 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
    //Vertex BiTangent 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 11));
    glEnableVertexAttribArray(4);

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

void Engine::TerrainProperty::DrawMesh(RenderPipeline* pipeline) {
    //Draw terrain mesh
    data.Draw(false);
    //Draw Grass
    DrawGrass(pipeline);
}

void Engine::TerrainProperty::DrawGrass(RenderPipeline* pipeline) {
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
                pipeline->instancedTransformBuffer->writeData(0, entries * sizeof(ZSMATRIX4x4),
                    &grass->inst_transform[offset]);
                //Draw grass instanced
                Engine::getGrassMesh()->DrawInstanced(entries);
            }

        }
    }
}

void Engine::TerrainProperty::onRender(Engine::RenderPipeline* pipeline) {
    terrainUniformBuffer = pipeline->terrainUniformBuffer;
    transformBuffer = pipeline->transformBuffer;

    if (data.hasHeightmapChanged) {
        this->data.updateGeometryBuffersGL();
        data.hasHeightmapChanged = false;
    }

    if (data.hasPaintingChanged) {
        this->data.updateTextureBuffersGL();
        data.hasPaintingChanged = false;
    }

    //Binding terrain buffer
    pipeline->terrainUniformBuffer->bind();

    Engine::MaterialProperty* mat = go_link.updLinkPtr()->getPropertyPtr<Engine::MaterialProperty>();
    if (mat == nullptr) return;

    int dtrue = 1;
    int dfalse = 0;

    //Iterate over all textures to use them
    for (unsigned int i = 0; i < static_cast<unsigned int>(this->textures_size); i++) {
        //Get pair pointer
        HeightmapTexturePair* pair = &this->textures[i];
        //Check, if pair has diffuse texture
        if (pair->diffuse != nullptr) {
            //Use diffuse texture
            pair->diffuse->Use(static_cast<int>(i));
            terrainUniformBuffer->writeData(16 * i, 4, &dtrue);
        }
        else {
            terrainUniformBuffer->writeData(16 * i, 4, &dfalse);
        }
        //Check, if pair has normal texture
        if (pair->normal != nullptr) {
            //Use normal texture
            pair->normal->Use(static_cast<int>(12 + i));
            terrainUniformBuffer->writeData(16 * 12 + 16 * i, 4, &dtrue);
        }
        else {
            terrainUniformBuffer->writeData(16 * 12 + 16 * i, 4, &dfalse);
        }
    }
    //Tell shader, that we rendering terrain in normal mode (non picking)
    terrainUniformBuffer->writeData(16 * 12 * 2, 4, &dfalse);
    //Send dimensions to buffer
    terrainUniformBuffer->writeData(16 * 12 * 2 + 4, 4, &this->data.W);
    terrainUniformBuffer->writeData(16 * 12 * 2 + 8, 4, &this->data.H);

    //Apply material shader
    mat->onRender(pipeline);

}