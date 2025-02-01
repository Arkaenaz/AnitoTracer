#pragma once

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>     
#include "string"

class ModelImporter
{
	public:
		ModelImporter();
		~ModelImporter();

		const aiScene* LoadFile(std::string filePath);
};

