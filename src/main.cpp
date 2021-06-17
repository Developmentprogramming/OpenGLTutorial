#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "io/Joystick.h"
#include "io/Camera.h"
#include "io/Screen.h"
#include "graphics/models/Cube.hpp"

unsigned int SRC_WIDTH = 800, SRC_HEIGHT = 600;
Screen screen;
float mixVal = 0.5f;

Joystick mainJ(0);

Camera cameras[2] = 
{
	glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(10.0f, 10.0f, 10.0f)
};
int activeCam = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SRC_WIDTH = width;
	SRC_HEIGHT = height;
}

void processInput(double dt)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE))
		screen.setShouldClose(true);

	if (Keyboard::key(GLFW_KEY_UP))
	{
		mixVal += .05f;
		if (mixVal > 1)
			mixVal = 1.0f;
	}

	if (Keyboard::key(GLFW_KEY_DOWN))
	{
		mixVal -= .05f;
		if (mixVal < 0)
			mixVal = 0.0f;
	}

	if (Keyboard::key(GLFW_KEY_W))
		cameras[activeCam].updateCameraPos(CameraDirection::FORWARD, dt);

	if (Keyboard::key(GLFW_KEY_S))
		cameras[activeCam].updateCameraPos(CameraDirection::BACKWARD, dt);

	if (Keyboard::key(GLFW_KEY_A))
		cameras[activeCam].updateCameraPos(CameraDirection::LEFT, dt);

	if (Keyboard::key(GLFW_KEY_D))
		cameras[activeCam].updateCameraPos(CameraDirection::RIGHT, dt);

	if (Keyboard::key(GLFW_KEY_SPACE))
		cameras[activeCam].updateCameraPos(CameraDirection::UP, dt);

	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT))
		cameras[activeCam].updateCameraPos(CameraDirection::DOWN, dt);

	if (Keyboard::keyWentDown(GLFW_KEY_TAB))
		activeCam = !activeCam;

	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0)
		cameras[activeCam].updateCameraDirection(dx, dy);

	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0)
		cameras[activeCam].updateCameraZoom(scrollDy);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

	if (!screen.init())
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	screen.setParameters();

	glEnable(GL_DEPTH_TEST);

	/*
		shaders
	*/

	Shader shader("assets/object.vs.shader", "assets/object.fs.shader");

	Cube cube(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.75f));
	cube.init();

	mainJ.update();
	if (mainJ.isPresent())
		std::cout << mainJ.getName() << " is present" << std::endl;
	else
		std::cout << "Not present" << std::endl;

	while (!screen.shouldClose())
	{
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		processInput(deltaTime);

		screen.update();

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = cameras[activeCam].getViewMatrix();
		projection = glm::perspective(glm::radians(cameras[activeCam].getZoom()), (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.0f);

		shader.activate();
		shader.SetFloat("mixVal", mixVal);
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);

		cube.render(shader);

		screen.newFrame();
	}

	cube.cleanup();

	glfwTerminate();
	return 0;
}
