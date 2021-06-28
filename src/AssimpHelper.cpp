#include "AssimpHelper.h"

// Assimp
// http://assimp.sourceforge.net/lib_html/usage.html
#include <Assimp/Importer.hpp>      // C++ importer interface
#include <Assimp/scene.h>           // Output data structure
#include <Assimp/postprocess.h>     // Post processing flags

///////////////////// GENERAL ASSIMP HELPERS FUNCTIONS //////////////////////////////////////////////////////////////////////////////

void MeshProcessing(
	const aiMesh* mesh,
	const aiScene* scene,
	std::vector<glm::vec3>& positions,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals,
	std::vector<glm::vec3>& tangents,
	std::vector<glm::vec3>& bitangents,
	std::vector<unsigned int>& indices)
{
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;

		positions.push_back(vector);

		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;

		normals.push_back(vector);

		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;

			uvs.push_back(vec);
		}
		else
		{
			uvs.push_back(glm::vec2(0.0f, 0.0f));
		}

		// tangents
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		tangents.push_back(vector);

		// bitangents
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		bitangents.push_back(vector);
	}

	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
}

void NodeProcessing(
	const aiScene* scene,
	const aiNode* node,
	std::vector<glm::vec3>& positions,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals,
	std::vector<glm::vec3>& tangents,
	std::vector<glm::vec3>& bitangents,
	std::vector<unsigned int>& indices)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		MeshProcessing(mesh, scene, positions, uvs, normals, tangents, bitangents, indices);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		NodeProcessing(scene, node->mChildren[i], positions, uvs, normals, tangents, bitangents, indices);
	}
}

void SceneProcessing(
	const aiScene* scene,
	std::vector<glm::vec3>& positions,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals,
	std::vector<glm::vec3>& tangents,
	std::vector<glm::vec3>& bitangents,
	std::vector<unsigned int>& indices)
{
	NodeProcessing(scene, scene->mRootNode, positions, uvs, normals, tangents, bitangents, indices);
}

// http://assimp.sourceforge.net/lib_html/usage.html
bool AssimpHelper::ImportMesh(
	const std::string& pFile,
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals,
	std::vector<unsigned int>& indices,
	std::vector<glm::vec3>& tangents,
	std::vector<glm::vec3>& bitangents)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene)
	{
		std::cout << "Assimp loading error::\n" << importer.GetErrorString() << std::endl;
		return false;
	}
	// Now we can access the file's contents. 
	SceneProcessing(scene, vertices, uvs, normals, tangents, bitangents, indices);

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
