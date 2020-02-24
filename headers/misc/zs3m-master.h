#ifndef ZS3MMASTER_H
#define ZS3MMASTER_H
#include "../render/zs-math.h"
#include "../render/zs-mesh.h"
#include "../render/zs-animation.h"
#include <string>
#include <vector>
namespace ZS3M {

class SceneNode{

public:
    //Node name string
    std::string node_label;
    //Transform matrix
    ZSMATRIX4x4 node_transform;

    ZSVECTOR3 node_translation;
    ZSVECTOR3 node_scaling;
    ZSQUATERNION node_rotation;

    std::vector<std::string> child_node_labels;
    std::vector<SceneNode*> children;
    std::vector<std::string> mesh_names;

    SceneNode() {
    }
};

class SceneFileExport{
private:
    std::vector<Engine::Mesh*> meshes_toWrite;
    ZS3M::SceneNode* rootNode;
public:

    void pushMesh(Engine::Mesh* mesh);
    void setRootNode(ZS3M::SceneNode* node);
    void write(std::string output_file);
    void writeNode(std::ofstream *stream, ZS3M::SceneNode* node);
    void getNodesNum(unsigned int* nds_ptr, ZS3M::SceneNode* node);

    SceneFileExport();
};

class AnimationFileExport{
private:
    Engine::Animation* anim_ptr;
public:

    void writeChannel(std::ofstream* stream, unsigned int index);
    void write(std::string output_file);

    AnimationFileExport(Engine::Animation* anim_ptr);
};

class ImportedSceneFile{
private:
    std::vector<ZS3M::SceneNode*> nodes_list;
public:
    std::vector<Engine::Mesh*> meshes_toRead;
    ZS3M::SceneNode* rootNode;

    ZS3M::SceneNode* getSceneNodeWithName(std::string label);
    void loadFromBuffer(char* buffer, unsigned int buf_size);
    void loadFromFile(std::string file);

    void makeNodeHierarchy(ZS3M::SceneNode* node);
    ImportedSceneFile();
};

class ImportedAnimationFile{
public:
    Engine::Animation* anim_ptr;

    void loadFromBuffer(char* buffer, unsigned int size);
    void loadFromFile(std::string file);

    ImportedAnimationFile();
};

}

#endif // ZS3MMASTER_H
