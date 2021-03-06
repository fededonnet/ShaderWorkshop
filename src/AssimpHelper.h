#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

class AssimpHelper
{
public:

	static bool ImportMesh(const std::string& pFile,
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals,
		std::vector<unsigned int>& indices,
		std::vector<glm::vec3>& tangents = std::vector<glm::vec3>(),
		std::vector<glm::vec3>& bitangents = std::vector<glm::vec3>());

};