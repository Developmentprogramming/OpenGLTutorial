#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	int success;
	char infolog[512];

	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	m_Id = glCreateProgram();
	glAttachShader(m_Id, vertexShader);
	glAttachShader(m_Id, fragmentShader);
	glLinkProgram(m_Id);

	// catch error
	glGetProgramiv(m_Id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_Id, 512, nullptr, infolog);
		std::cout << "Linking Error: " << std::endl << infolog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::activate()
{
	glUseProgram(m_Id);
}

std::string Shader::loadShaderSrc(const char* filepath)
{
	std::ifstream file;
	std::stringstream buf;

	std::string ret = "";

	file.open(filepath);
	if (file.is_open())
	{
		buf << file.rdbuf();
		ret = buf.str();
	}
	else
		std::cout << "Could not open " << filepath << std::endl;

	file.close();
	return ret;
}

GLuint Shader::compileShader(const char* filepath, GLenum type)
{
	int success;
	char infolog[512];

	GLuint shader = glCreateShader(type);
	std::string shaderSrc = loadShaderSrc(filepath);
	const GLchar* src = shaderSrc.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	// catch error
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, nullptr, infolog);
		std::cout << "Error with shader comp ..." << std::endl << infolog << std::endl;
	}

	return shader;
}

void Shader::SetBool(const std::string& name, bool val)
{
	glUniform1i(glGetUniformLocation(m_Id, name.c_str()), (int)val);
}

void Shader::SetInt(const std::string& name, int val)
{
	glUniform1i(glGetUniformLocation(m_Id, name.c_str()), val);
}

void Shader::SetFloat(const std::string& name, float val)
{
	glUniform1f(glGetUniformLocation(m_Id, name.c_str()), val);
}

void Shader::Set4Float(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(glGetUniformLocation(m_Id, name.c_str()), v0, v1, v2, v3);
}

void Shader::SetMat4(const std::string& name, glm::mat4 val)
{
	glUniformMatrix4fv(glGetUniformLocation(m_Id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

