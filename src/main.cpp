#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

std::string loadShaderSrc(const char* filename)
{
	std::ifstream file;
	std::stringstream buf;

	std::string ret = "";

	file.open(filename);
	if (file.is_open())
	{
		buf << file.rdbuf();
		ret = buf.str();
	}
	else
		std::cout << "Could not open " << filename << std::endl;

	file.close();
	return ret;
}

int main()
{
	int success;
	char infolog[512];

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL 4", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*
		shaders
	*/

	// compile vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertShaderSrc = loadShaderSrc("assets/vertex_core.glsl.shader");
	const GLchar* vertShader = vertShaderSrc.c_str();
	glShaderSource(vertexShader, 1, &vertShader, nullptr);
	glCompileShader(vertexShader);

	// catch error
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infolog);
		std::cout << "Error with vertex shader comp ..." << std::endl << infolog << std::endl;
	}

	// compile fragment shader
	unsigned int fragmentShaders[2];
	fragmentShaders[0] = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragShaderSrc = loadShaderSrc("assets/fragment_core.glsl.shader");
	const GLchar* fragShader = fragShaderSrc.c_str();
	glShaderSource(fragmentShaders[0], 1, &fragShader, nullptr);
	glCompileShader(fragmentShaders[0]);

	// catch error
	glGetShaderiv(fragmentShaders[0], GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaders[0], 512, nullptr, infolog);
		std::cout << "Error with fragment shader comp ..." << std::endl << infolog << std::endl;
	}

	fragmentShaders[1] = glCreateShader(GL_FRAGMENT_SHADER);
	fragShaderSrc = loadShaderSrc("assets/fragment_core2.glsl.shader");
	fragShader = fragShaderSrc.c_str();
	glShaderSource(fragmentShaders[1], 1, &fragShader, nullptr);
	glCompileShader(fragmentShaders[1]);

	// catch error
	glGetShaderiv(fragmentShaders[1], GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaders[1], 512, nullptr, infolog);
		std::cout << "Error with fragment shader comp ..." << std::endl << infolog << std::endl;
	}

	// create program
	unsigned int shaderPrograms[2];
	shaderPrograms[0] = glCreateProgram();
	glAttachShader(shaderPrograms[0], vertexShader);
	glAttachShader(shaderPrograms[0], fragmentShaders[0]);
	glLinkProgram(shaderPrograms[0]);

	// catch error
	glGetProgramiv(shaderPrograms[0], GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderPrograms[0], 512, nullptr, infolog);
		std::cout << "Linking Error: " << std::endl << infolog << std::endl;
	}

	shaderPrograms[1] = glCreateProgram();
	glAttachShader(shaderPrograms[1], vertexShader);
	glAttachShader(shaderPrograms[1], fragmentShaders[1]);
	glLinkProgram(shaderPrograms[1]);

	// catch error
	glGetProgramiv(shaderPrograms[1], GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderPrograms[1], 512, nullptr, infolog);
		std::cout << "Linking Error: " << std::endl << infolog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShaders[0]);

	//float vertices[] = {
	//	 0.5f,  0.5f, 0.0f, // top right
	//	-0.5f,  0.5f, 0.0f, // top left
	//	-0.5f, -0.5f, 0.0f, // bottom left
	//	 0.5f, -0.5f, 0.0f, // bottom right
	//};

	//unsigned int indices[] = {
	//	0, 1, 2, // first triangle
	//	2, 3, 0  // second triangle
	//};

	float vertices[] = {
		 -0.5f, -0.5f, 0.0f,
		-0.25f,  0.5f, 0.0f,
		 -0.1f, -0.5f, 0.0,

		  0.5f, -0.5f, 0.0f,
		 0.25f,  0.5f, 0.0f,
		  0.1f, -0.5f, 0.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		3, 4, 5
	};

	// VAO, VBO
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind VAO
	glBindVertexArray(VAO);

	// bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw shapes
		glBindVertexArray(VAO);

		glUseProgram(shaderPrograms[0]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

		glUseProgram(shaderPrograms[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(3 * sizeof(unsigned int)));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}
