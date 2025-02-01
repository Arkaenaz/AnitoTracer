#include "ModelImporter.h"

ModelImporter::ModelImporter()
{
}

ModelImporter::~ModelImporter()
{
}

const aiScene* ModelImporter::LoadFile(std::string filePath)
{
    Assimp::Importer importer;
    
    const aiScene* scene = importer.ReadFile(filePath.c_str(),
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    //scene->mMeshes[0]->mVertices;

    return scene;
}
