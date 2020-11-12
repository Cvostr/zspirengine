#define _CRT_SECURE_NO_WARNINGS

#include "../../headers/misc/zs3m-master.h"
#include <fstream>
#include <iostream>
#include <cstring>


ZS3M::SceneFileExport::SceneFileExport(){
    rootNode = nullptr;

}
void ZS3M::SceneFileExport::pushMesh(Engine::Mesh* mesh){
    this->meshes_toWrite.push_back(mesh);
}
void ZS3M::SceneFileExport::setRootNode(ZS3M::SceneNode* node){
    this->rootNode = node;
}
void ZS3M::SceneFileExport::write(std::string output_file){
    std::cout << "ZS3M: Writing ZS3M scene " << output_file << std::endl;
    ZsStream stream;
    stream.open(output_file.c_str(), std::ofstream::binary);

    stream << "zs3mscene\n";
    unsigned int model_ver = 1000;
    unsigned int meshes_num = static_cast<unsigned int>(this->meshes_toWrite.size());
    unsigned int nodes_num = 0;
    getNodesNum(&nodes_num, this->rootNode);

    stream.writeBinaryValue(&model_ver);
    stream.writeBinaryValue(&meshes_num);
    stream.writeBinaryValue(&nodes_num);
    stream << "\n"; //Write divider
    //Write all nodes
    writeNode(&stream, rootNode);

    //Iterate over all meshes
    for(unsigned int mesh_i = 0; mesh_i < meshes_num; mesh_i ++){
        Engine::Mesh* mesh_ptr = this->meshes_toWrite[mesh_i];
        std::cout << "ZS3M: Writing Mesh " << mesh_ptr->mesh_label << std::endl;
        stream << "_MESH " << (mesh_ptr->mesh_label + '\0');

        unsigned int bonesNum = static_cast<unsigned int>(mesh_ptr->bones.size());
        //Write base numbers
        stream.writeBinaryValue(&mesh_ptr->mVerticesNum);
        stream.writeBinaryValue(&mesh_ptr->mIndicesNum);
        stream.writeBinaryValue(&bonesNum);
        stream << "\n"; //Write divider
        //Write all vertices
        for (unsigned int v_i = 0; v_i < static_cast<unsigned int>(mesh_ptr->mVerticesNum); v_i ++) {
            ZSVERTEX* v_ptr = &mesh_ptr->vertices_arr[v_i];
            //Write vertex vectors
            stream.write(reinterpret_cast<char*>(&v_ptr->pos), sizeof(float) * 3);
            stream.write(reinterpret_cast<char*>(&v_ptr->uv), sizeof(float) * 2);
            stream.write(reinterpret_cast<char*>(&v_ptr->normal), sizeof(float) * 3);
            stream.write(reinterpret_cast<char*>(&v_ptr->tangent), sizeof(float) * 3);
            stream.write(reinterpret_cast<char*>(&v_ptr->bitangent), sizeof(float) * 3);
            stream.write(reinterpret_cast<char*>(&v_ptr->bones_num), sizeof(unsigned int));
            for(unsigned int vb_i = 0; vb_i < v_ptr->bones_num; vb_i ++){
                unsigned int bone_id = v_ptr->ids[vb_i];
                float b_weight = v_ptr->weights[vb_i];
                //Write bone values
                stream.writeBinaryValue(&bone_id);
                stream.writeBinaryValue(&b_weight);
            }
        }
        for(unsigned int ind_i = 0; ind_i < static_cast<unsigned int>(mesh_ptr->mIndicesNum); ind_i ++){
            stream.writeBinaryValue(&mesh_ptr->indices_arr[ind_i]);
        }
        stream << "\n"; //Write divider

        for (unsigned int b_i = 0; b_i < bonesNum; b_i ++) {
            Engine::Bone* bone = &mesh_ptr->bones[b_i];
            //Write bone name
            stream.writeString(bone->bone_name);
            //Write offset matrix
            for(unsigned int m_i = 0; m_i < 4; m_i ++){
                for(unsigned int m_j = 0; m_j < 4; m_j ++){
                    float m_v = bone->offset.m[m_i][m_j];
                    stream.writeBinaryValue(&m_v);
                }
            }
            stream << "\n"; //Write divider
        }
        stream << "\n";
    }
    stream.close();
}

void ZS3M::SceneFileExport::writeNode(ZsStream *stream, ZS3M::SceneNode* node){
    //Write node header
    *stream << "_NODE " << (node->node_label + '\0');

    unsigned int meshesNum = static_cast<unsigned int>(node->mesh_names.size());
    unsigned int childrenNum = static_cast<unsigned int>(node->children.size());

    stream->writeBinaryValue(&meshesNum);
    stream->writeBinaryValue(&childrenNum);

    *stream << "\n"; //Write divider

    for(unsigned int mesh_i = 0; mesh_i < node->mesh_names.size(); mesh_i ++){
        //Write mesh name
        stream->writeString(node->mesh_names[mesh_i]);
    }
    for(unsigned int ch_i = 0; ch_i < node->children.size(); ch_i ++){
        //Write child node string
        stream->writeString(node->children[ch_i]->node_label);
    }
    //Write node base matrix
    for(unsigned int m_i = 0; m_i < 4; m_i ++){
        for(unsigned int m_j = 0; m_j < 4; m_j ++){
            float m_v = node->node_transform.m[m_i][m_j];
            stream->writeBinaryValue(&m_v);
        }
    }
    stream->writeBinaryValue(&node->node_translation.X);//Writing position X
    stream->writeBinaryValue(&node->node_translation.Y); //Writing position Y
    stream->writeBinaryValue(&node->node_translation.Z); //Writing position Z

    stream->writeBinaryValue(&node->node_scaling.X);//Writing scale X
    stream->writeBinaryValue(&node->node_scaling.Y); //Writing scale Y
    stream->writeBinaryValue(&node->node_scaling.Z); //Writing scale Z

    stream->writeBinaryValue(&node->node_rotation.X);//Writing rotation X
    stream->writeBinaryValue(&node->node_rotation.Y); //Writing rotation Y
    stream->writeBinaryValue(&node->node_rotation.Z); //Writing rotation Z
    stream->writeBinaryValue(&node->node_rotation.W); //Writing rotation W
    *stream << "\n"; //Write divider
    //Write all children
    for(unsigned int ch_i = 0; ch_i < node->children.size(); ch_i ++){
        writeNode(stream, node->children[ch_i]);
    }
}

void ZS3M::SceneFileExport::getNodesNum(unsigned int* nds_ptr, ZS3M::SceneNode* node){
    *nds_ptr += 1;
    //Write all children
    for(unsigned int ch_i = 0; ch_i < node->children.size(); ch_i ++){
        getNodesNum(nds_ptr, node->children[ch_i]);
    }
}

void ZS3M::ImportedSceneFile::loadFromBuffer(char* buffer, unsigned int buf_size){
    char header[10];
    memcpy(&header, buffer, 9);
    header[9] = '\0'; //Terminate string

    if(strcmp(header, "zs3mscene") == 1){
        //File's header is incorrect
        return;
    }

    unsigned int cur_pos = 10;

    unsigned int model_ver = 0;
    unsigned int meshes_num = 0;
    unsigned int nodes_num = 0;

    readBinaryValue(&model_ver, &buffer[cur_pos], cur_pos);
    readBinaryValue(&meshes_num, &buffer[cur_pos], cur_pos);
    readBinaryValue(&nodes_num, &buffer[cur_pos], cur_pos);

    if (model_ver != 1000) {
        return;
    }
    
    char prefix[5];

    while(cur_pos < buf_size){
        cur_pos += 1;
        memcpy(&prefix, &buffer[cur_pos], 5);

        if(prefix[0] == '_' && prefix[1] == 'N' && prefix[2] == 'O' && prefix[3] == 'D' && prefix[4] == 'E'){
            cur_pos += 6;
            ZS3M::SceneNode* node = new ZS3M::SceneNode;

            //Read NODE label string
            readString(node->node_label, buffer, cur_pos);

            unsigned int meshesNum = 0;
            unsigned int childrenNum = 0;

            //Read data
            readBinaryValue(&meshesNum, &buffer[cur_pos], cur_pos);
            readBinaryValue(&childrenNum, &buffer[cur_pos], cur_pos);
            cur_pos += 1;

            //iterate over all meshes, connected to this node
            for(unsigned int mesh_i = 0; mesh_i < meshesNum; mesh_i ++){
                std::string mesh_label;
                //Read NODE label string
                readString(mesh_label, buffer, cur_pos);
                //Put mesh name
                node->mesh_names.push_back(mesh_label);
            }
            //iterate over all children nodes, connected to this node
            for(unsigned int ch_i = 0; ch_i < childrenNum; ch_i ++){
                //Read child name
                std::string child_node_label;
                readString(child_node_label, buffer, cur_pos);
                //Put child name
                node->child_node_labels.push_back(child_node_label);
            }
            //Read node base matrix
            for(unsigned int m_i = 0; m_i < 4; m_i ++){
                for(unsigned int m_j = 0; m_j < 4; m_j ++){
                    float* m_v = &node->node_transform.m[m_i][m_j];
                    readBinaryValue(m_v, &buffer[cur_pos], cur_pos);
                }
            }
            //Read node Translation
            readBinaryValue(&node->node_translation.X, &buffer[cur_pos], cur_pos);
            readBinaryValue(&node->node_translation.Y, &buffer[cur_pos], cur_pos);
            readBinaryValue(&node->node_translation.Z, &buffer[cur_pos], cur_pos);
            //Read node Scale
            readBinaryValue(&node->node_scaling.X, &buffer[cur_pos], cur_pos);
            readBinaryValue(&node->node_scaling.Y, &buffer[cur_pos], cur_pos);
            readBinaryValue(&node->node_scaling.Z, &buffer[cur_pos], cur_pos);
            //Read node Rotation Quaternion
            readBinaryValue(&node->node_rotation.X, &buffer[cur_pos], cur_pos);
            readBinaryValue(&node->node_rotation.Y, &buffer[cur_pos], cur_pos);
            readBinaryValue(&node->node_rotation.Z, &buffer[cur_pos], cur_pos);
            readBinaryValue(&node->node_rotation.W, &buffer[cur_pos], cur_pos);

            //Add node to list
            nodes_list.push_back(node);
        }

        //We reached mesh
        if(prefix[0] == '_' && prefix[1] == 'M' && prefix[2] == 'E' && prefix[3] == 'S' && prefix[4] == 'H'){
            cur_pos += 6;

            std::string mesh_label;
            //Read mesh label string
            readString(mesh_label, buffer, cur_pos);

            int vertexNum = 0;
            int indexNum = 0;
            unsigned int bonesNum = 0;
            //Read mesh info
            //Read vertex amount
            readBinaryValue(&vertexNum, &buffer[cur_pos], cur_pos);
            //Read indices amount
            readBinaryValue(&indexNum, &buffer[cur_pos], cur_pos);
            //Read bones amount
            readBinaryValue(&bonesNum, &buffer[cur_pos], cur_pos);
            cur_pos += 1;

            Engine::Mesh* newmesh = Engine::allocateMesh();
            newmesh->mesh_label = mesh_label;
            newmesh->mVerticesNum = vertexNum;
            newmesh->mIndicesNum = indexNum;
            //Allocate arrays for vectors
            newmesh->vertices_arr = new ZSVERTEX[static_cast<unsigned int>(vertexNum)];
            newmesh->indices_arr = new unsigned int[static_cast<unsigned int>(indexNum)];
            newmesh->vertices_coord = new float[static_cast<unsigned int>(vertexNum) * 3];

            for(unsigned int v_i = 0; v_i < static_cast<unsigned int>(vertexNum); v_i ++){
                ZSVERTEX v_ptr;
                //Read vertex vectors
                memcpy(reinterpret_cast<char*>(&v_ptr.pos), &buffer[cur_pos], sizeof(float) * 3);
                memcpy(reinterpret_cast<char*>(&newmesh->vertices_coord[v_i * 3]), &buffer[cur_pos], sizeof(float) * 3);
                cur_pos += sizeof(float) * 3;
                memcpy(reinterpret_cast<char*>(&v_ptr.uv), &buffer[cur_pos], sizeof(float) * 2);
                cur_pos += sizeof(float) * 2;
                memcpy(reinterpret_cast<char*>(&v_ptr.normal), &buffer[cur_pos], sizeof(float) * 3);
                cur_pos += sizeof(float) * 3;
                memcpy(reinterpret_cast<char*>(&v_ptr.tangent), &buffer[cur_pos], sizeof(float) * 3);
                cur_pos += sizeof(float) * 3;
                memcpy(reinterpret_cast<char*>(&v_ptr.bitangent), &buffer[cur_pos], sizeof(float) * 3);
                cur_pos += sizeof(float) * 3;
                readBinaryValue(&v_ptr.bones_num, &buffer[cur_pos], cur_pos);

                //Read vertex bone data
                for(unsigned int vb_i = 0; vb_i < 12; vb_i ++){
                    unsigned int* bone_id = &v_ptr.ids[vb_i];
                    float* b_weight = &v_ptr.weights[vb_i];

                    *bone_id = 0;
                    *b_weight = 0.0f;
                }
                for(unsigned int vb_i = 0; vb_i < v_ptr.bones_num; vb_i ++){
                    unsigned int* bone_id = &v_ptr.ids[vb_i];
                    float* b_weight = &v_ptr.weights[vb_i];
                    //Read bone values
                    readBinaryValue(bone_id, &buffer[cur_pos], cur_pos);
                    readBinaryValue(b_weight, &buffer[cur_pos], cur_pos);
                }

                newmesh->vertices_arr[v_i] = v_ptr;
            }
            //Read mesh indices
            for(unsigned int in_i = 0; in_i < static_cast<unsigned int>(indexNum); in_i ++){
                readBinaryValue(&newmesh->indices_arr[in_i], &buffer[cur_pos], cur_pos);
            }
            cur_pos += 1;

            newmesh->Init();
            newmesh->setMeshData(newmesh->vertices_arr, newmesh->indices_arr, newmesh->mVerticesNum, newmesh->mIndicesNum);
            //newmesh->mBoundingBox.CreateFromVertexArray(newmesh->vertices_arr, newmesh->mVerticesNum);

            delete[] newmesh->vertices_arr;
            delete[] newmesh->indices_arr;
            newmesh->vertices_arr = 0x0;
            newmesh->indices_arr = 0x0;

            for (unsigned int b_i = 0; b_i < bonesNum; b_i ++) {
                std::string bone_label;
                //Read mesh label string
                readString(bone_label, buffer, cur_pos);

                Engine::Bone bone(bone_label);
                for(unsigned int m_i = 0; m_i < 4; m_i ++){
                    for(unsigned int m_j = 0; m_j < 4; m_j ++){
                        float* m_v = &bone.offset.m[m_i][m_j];
                        readBinaryValue(m_v, &buffer[cur_pos], cur_pos);
                    }
                }
                newmesh->bones.push_back(bone);
                cur_pos += 1;
            }

            //Push mesh as output result
            this->meshes_toRead.push_back(newmesh);
        }
    }

    rootNode = nodes_list[0];

    makeNodeHierarchy(rootNode);
}

void ZS3M::ImportedSceneFile::makeNodeHierarchy(ZS3M::SceneNode* node){
    for(unsigned int i = 0; i < node->child_node_labels.size(); i ++){
        std::string child_name = node->child_node_labels[i];

        ZS3M::SceneNode* nn = getSceneNodeWithName(child_name);

        if(nn == nullptr)
            continue;

        node->children.push_back(nn);

        makeNodeHierarchy(node->children[i]);
    }
}

ZS3M::SceneNode* ZS3M::ImportedSceneFile::getSceneNodeWithName(std::string label){
    for(unsigned int i = 0; i < this->nodes_list.size(); i ++){
        ZS3M::SceneNode* node = nodes_list[i];
        if(node->node_label.compare(label) == false){
            return node;
        }
    }
    return nullptr;
}

void ZS3M::ImportedSceneFile::loadFromFile(std::string file){
    std::ifstream stream;
    stream.open(file, std::iostream::binary | std::iostream::ate);
    //Get size of file
    unsigned int zs3m_size = static_cast<unsigned int>(stream.tellg());
    //Seek to start
    stream.seekg(0, std::ifstream::beg);
    //Allocate memory for file
    char* file_buffer = new char[zs3m_size];
    stream.read(file_buffer, zs3m_size);

    loadFromBuffer(file_buffer, zs3m_size);
    //free memory
    delete[] file_buffer;
}

void ZS3M::ImportedSceneFile::clearMeshes(){
    for(unsigned int i = 0; i < meshes_toRead.size(); i ++){
        delete meshes_toRead[i];
    }
    meshes_toRead.clear();
}

ZS3M::ImportedSceneFile::ImportedSceneFile(){
    rootNode = nullptr;
}

ZS3M::ImportedSceneFile::~ImportedSceneFile(){
}

ZS3M::AnimationFileExport::AnimationFileExport(Engine::Animation* anim_ptr){
    this->anim_ptr = anim_ptr;
}

void ZS3M::AnimationFileExport::writeChannel(std::ofstream* stream, unsigned int index){
    Engine::AnimationChannel* channel = &anim_ptr->channels[index];
    //Write channel prefix and its name
    *stream << "_CHAN" << '\0' << channel->bone_name + '\0';

    stream->write(reinterpret_cast<char*>(&channel->posKeysNum), sizeof (unsigned int));
    stream->write(reinterpret_cast<char*>(&channel->scaleKeysNum), sizeof (unsigned int));
    stream->write(reinterpret_cast<char*>(&channel->rotationKeysNum), sizeof (unsigned int));

    for(unsigned int pos_i = 0; pos_i < channel->posKeysNum; pos_i ++){
        //Writing pos vector
        stream->write(reinterpret_cast<char*>(&channel->pos[pos_i].X), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->pos[pos_i].Y), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->pos[pos_i].Z), sizeof (float));
        //Write pos key time
        stream->write(reinterpret_cast<char*>(&channel->posTimes[pos_i]), sizeof (double));

    }
    for(unsigned int sca_i = 0; sca_i < channel->scaleKeysNum; sca_i ++){
        //Writing scale vector
        stream->write(reinterpret_cast<char*>(&channel->scale[sca_i].X), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->scale[sca_i].Y), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->scale[sca_i].Z), sizeof (float));
        //Write pos key time
        stream->write(reinterpret_cast<char*>(&channel->scaleTimes[sca_i]), sizeof (double));
    }
    for(unsigned int rot_i = 0; rot_i < channel->rotationKeysNum; rot_i ++){
        //Writing pos vector
        stream->write(reinterpret_cast<char*>(&channel->rot[rot_i].X), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->rot[rot_i].Y), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->rot[rot_i].Z), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->rot[rot_i].W), sizeof (float));
        //Write pos key time
        stream->write(reinterpret_cast<char*>(&channel->rotTimes[rot_i]), sizeof (double));
    }
    *stream << '\n';
}
void ZS3M::AnimationFileExport::write(std::string output_file){
    ZsStream stream;
    stream.open(output_file.c_str(), std::ofstream::binary);

    stream << "zs3manim" << '\0' << anim_ptr->name + '\0'; //Write animation
    //Write main animation data
    stream.writeBinaryValue(&anim_ptr->TPS);
    stream.writeBinaryValue(&anim_ptr->duration);
    stream.writeBinaryValue(&anim_ptr->NumChannels);
    //Write divider
    stream << "\n";

    for(unsigned int cha_i = 0; cha_i < anim_ptr->NumChannels; cha_i ++){
        writeChannel(&stream, cha_i);
    }

    stream.close();
}

ZS3M::ImportedAnimationFile::ImportedAnimationFile(){
    anim_ptr = nullptr;
}

void ZS3M::ImportedAnimationFile::loadFromBuffer(char* buffer, unsigned int size){
    char prefix[64];
    unsigned int byte_offset = 0;

    strcpy(&prefix[0], &buffer[byte_offset]);
    byte_offset += static_cast<unsigned int>(strlen(prefix)) + 1;
    if(strcmp(prefix, "zs3manim") == 1){
        //File's header is incorrect
        return;
    }
    //Allocate animation class
    anim_ptr = new Engine::Animation; //allocate animation
    //Read animation name
    readString(anim_ptr->name, buffer, byte_offset);
    //Read Tick Per Second property
    readBinaryValue(&anim_ptr->TPS, &buffer[byte_offset], byte_offset);
    //Read duration property
    readBinaryValue(&anim_ptr->duration, &buffer[byte_offset], byte_offset);
    //Read Channel amount
    readBinaryValue(&anim_ptr->NumChannels, &buffer[byte_offset], byte_offset);
    //Allocate all channels
    anim_ptr->channels = new Engine::AnimationChannel[anim_ptr->NumChannels];
    unsigned int ch_i = 0;

    while (byte_offset < size) {
        byte_offset += 1;
        memcpy(&prefix, &buffer[byte_offset], 6);

        if(strcmp(prefix, "_CHAN") == 0){
            byte_offset += 6;
            //Read node, channel will work with
            //Allocate animation channel
            Engine::AnimationChannel* chan = &anim_ptr->channels[ch_i];
            readString(chan->bone_name, buffer, byte_offset);
            chan->anim_ptr = anim_ptr;

            readBinaryValue(&chan->posKeysNum, &buffer[byte_offset], byte_offset);
            readBinaryValue(&chan->scaleKeysNum, &buffer[byte_offset], byte_offset);
            readBinaryValue(&chan->rotationKeysNum, &buffer[byte_offset], byte_offset);

            //Allocate pos vectors and time intervals array
            chan->pos = new ZSVECTOR3[chan->posKeysNum];
            chan->posTimes = new double[chan->posKeysNum];
            //Allocate scale vectors and time intervals array
            chan->scale = new ZSVECTOR3[chan->scaleKeysNum];
            chan->scaleTimes = new double[chan->scaleKeysNum];
            //Allocate rotation vectors and time intervals array
            chan->rot = new ZSQUATERNION[chan->rotationKeysNum];
            chan->rotTimes = new double[chan->rotationKeysNum];

            for(unsigned int pos_i = 0; pos_i < chan->posKeysNum; pos_i ++){
                //reading pos vector
                readBinaryValue(&chan->pos[pos_i].X, &buffer[byte_offset], byte_offset);
                readBinaryValue(&chan->pos[pos_i].Y, &buffer[byte_offset], byte_offset);
                readBinaryValue(&chan->pos[pos_i].Z, &buffer[byte_offset], byte_offset);
                //Read position timings
                readBinaryValue(&chan->posTimes[pos_i], &buffer[byte_offset], byte_offset);
            }
            for(unsigned int sca_i = 0; sca_i < chan->scaleKeysNum; sca_i ++){
                //reading scale vector
                readBinaryValue(&chan->scale[sca_i].X, &buffer[byte_offset], byte_offset);
                readBinaryValue(&chan->scale[sca_i].Y, &buffer[byte_offset], byte_offset);
                readBinaryValue(&chan->scale[sca_i].Z, &buffer[byte_offset], byte_offset);
                //Read scale timings
                readBinaryValue(&chan->scaleTimes[sca_i], &buffer[byte_offset], byte_offset);
            }
            for(unsigned int rot_i = 0; rot_i < chan->rotationKeysNum; rot_i ++){
                //reading scale vector
                readBinaryValue(&chan->rot[rot_i].X, &buffer[byte_offset], byte_offset);
                readBinaryValue(&chan->rot[rot_i].Y, &buffer[byte_offset], byte_offset);
                readBinaryValue(&chan->rot[rot_i].Z, &buffer[byte_offset], byte_offset);
                readBinaryValue(&chan->rot[rot_i].W, &buffer[byte_offset], byte_offset);
                //Read rotation timings
                readBinaryValue(&chan->rotTimes[rot_i], &buffer[byte_offset], byte_offset);
            }
            ch_i += 1;
        }
    }
}

void ZS3M::ImportedAnimationFile::loadFromFile(std::string file){
    std::ifstream stream;
    stream.open(file, std::iostream::binary | std::iostream::ate);
    unsigned int zs3m_size = static_cast<unsigned int>(stream.tellg());
    stream.seekg(0, std::ifstream::beg);
    char* file_buffer = new char[zs3m_size];
    stream.read(file_buffer, zs3m_size);

    loadFromBuffer(file_buffer, zs3m_size);

    delete[] file_buffer;
}
