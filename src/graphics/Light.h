#pragma once

#include <glm/glm.hpp>

#include "Shader.h"

struct PointLight
{
	glm::vec3 position;

	// attenuation constants
	float k0;
	float k1;
	float k2;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};

struct DirLight
{
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader);
};

struct SpotLight
{
	glm::vec3 position;
	glm::vec3 direction;

	float cutOff;
	float outerCufOff;

	// attenuation constants
	float k0;
	float k1;
	float k2;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};