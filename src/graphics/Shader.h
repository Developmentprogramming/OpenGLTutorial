#pragma once

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	unsigned int m_Id;

public:
	Shader();
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

	void generate(const char* vertexShaderPath, const char* fragmentShaderPath);
	void activate();

	// utility functions
	std::string loadShaderSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);

	// uniform functions
	void SetBool(const std::string& name, bool val);
	void SetInt(const std::string& name, int val);
	void SetFloat(const std::string& name, float val);
	void Set3Float(const std::string& name, glm::vec3 v);
	void Set3Float(const std::string& name, float v1, float v2, float v3);
	void Set4Float(const std::string& name, float v0, float v1, float v2, float v3);
	void Set4Float(const std::string& name, aiColor4D color);
	void Set4Float(const std::string& name, glm::vec4 v);
	void SetMat4(const std::string& name, glm::mat4 val);
};