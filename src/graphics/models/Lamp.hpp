#pragma once

#include "Cube.hpp"

#include "../Light.h"

class Lamp : public Cube
{
public:
	glm::vec3 lightColor;

	// light strength values
	PointLight pointLight;

	Lamp(glm::vec3 lightColor, 
		glm::vec4 ambient,
		glm::vec4 diffuse,
		glm::vec4 specular,
		float k0,
		float k1,
		float k2,
		glm::vec3 pos,
		glm::vec3 size)
		: lightColor(lightColor), 
		pointLight({ pos, k0, k1, k2, ambient, diffuse, specular }),
		Cube(pos, size)
	{}

	Lamp() {}

	void render(Shader shader)
	{
		shader.Set3Float("lightColor", lightColor);

		Cube::render(shader);
	}
};