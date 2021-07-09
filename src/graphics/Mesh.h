#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

#include "../algorithms/Bounds.h"
#include "models/Box.hpp"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	static std::vector<Vertex> getList(float* vertices, int noVertices);
};

class Mesh
{
public:
	BoundingRegion br;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO;
	std::vector<Texture> textures;
	aiColor4D diffuse;
	aiColor4D specular;

	Mesh(BoundingRegion br, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});
	Mesh(BoundingRegion br, std::vector<Vertex> vertices, std::vector<unsigned int> indices, aiColor4D diffuse, aiColor4D specular);

	void render(Shader shader, glm::vec3 pos, glm::vec3 size, Box* box, bool doRender = true);

	void cleanup();

private:
	unsigned int VBO, EBO;

	bool noTex;

	void setup();
};
