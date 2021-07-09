#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>

#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "io/Joystick.h"
#include "io/Camera.h"
#include "io/Screen.h"
#include "graphics/models/Cube.hpp"
#include "graphics/models/Lamp.hpp"
#include "graphics/models/Gun.hpp"
#include "graphics/Light.h"
#include "graphics/models/Sphere.hpp"
#include "physics/Environment.h"
#include "graphics/models/Box.hpp"

unsigned int SRC_WIDTH = 800, SRC_HEIGHT = 600;
Screen screen;
float mixVal = 0.5f;

Joystick mainJ(0);

Camera Camera::defaultCamera(glm::vec3(0.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool flashLightOn = true;

SphereArray launchObjects;

void launchItem(float dt)
{
	RigidBody rb(1.0f, Camera::defaultCamera.cameraPos);
	rb.transferEnergy(100.0f, Camera::defaultCamera.cameraFront);
	rb.applyAcceleration(Environment::gravitationalAcceleration);
	launchObjects.instances.push_back(rb);
}

void processInput(double dt)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE))
		screen.setShouldClose(true);

	if (Keyboard::key(GLFW_KEY_W))
		Camera::defaultCamera.updateCameraPos(CameraDirection::FORWARD, dt);

	if (Keyboard::key(GLFW_KEY_S))
		Camera::defaultCamera.updateCameraPos(CameraDirection::BACKWARD, dt);

	if (Keyboard::key(GLFW_KEY_A))
		Camera::defaultCamera.updateCameraPos(CameraDirection::LEFT, dt);

	if (Keyboard::key(GLFW_KEY_D))
		Camera::defaultCamera.updateCameraPos(CameraDirection::RIGHT, dt);

	if (Keyboard::key(GLFW_KEY_SPACE))
		Camera::defaultCamera.updateCameraPos(CameraDirection::UP, dt);

	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT))
		Camera::defaultCamera.updateCameraPos(CameraDirection::DOWN, dt);

	if (Keyboard::keyWentDown(GLFW_KEY_L))
		flashLightOn = !flashLightOn;

	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0)
		Camera::defaultCamera.updateCameraDirection(dx, dy);

	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0)
		Camera::defaultCamera.updateCameraZoom(scrollDy);

	if (Keyboard::keyWentDown(GLFW_KEY_F))
		launchItem(dt);
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
	Shader lampShader("assets/instanced/instanced.vs.shader", "assets/lamp.fs.shader");
	Shader launchShader("assets/instanced/instanced.vs.shader", "assets/object.fs.shader");
	Shader boxShader("assets/instanced/box.vs.shader", "assets/instanced/box.fs.shader");

	/*Gun g;
	g.loadModel("assets/models/m4a1/scene.gltf");*/

	launchObjects.init();

	Box box;
	box.init();

	Model m(BoundType::AABB, glm::vec3(0.0f), glm::vec3(0.05f));
	m.loadModel("assets/models/lotr_troll/scene.gltf");

	DirLight dirLight = 
	{ 
		glm::vec3(-0.2f, -1.0f, -0.3f), 
		glm::vec4(glm::vec3(0.1f), 1.0f),
		glm::vec4(glm::vec3(0.4f), 1.0f),
		glm::vec4(glm::vec3(0.75f), 1.0f)
	};

	glm::vec3 pointLightPositions[] = 
	{
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient = glm::vec4(glm::vec3(0.05f), 1.0f);
	glm::vec4 diffuse = glm::vec4(glm::vec3(0.8f), 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float k0 = 1.0f;
	float k1 = 0.07f;
	float k2 = 0.032f;

	/*Lamp lamps[4];*/
	/*for (unsigned int i = 0; i < 4; i++) 
	{
		lamps[i] = Lamp(glm::vec3(1.0f),
			ambient, diffuse, specular,
			k0, k1, k2,
			pointLightPositions[i], glm::vec3(0.25f));
		lamps[i].init();
	}*/

	LampArray lamps;
	lamps.init();
	for (unsigned int i = 0; i < 4; i++)
	{
		lamps.lightInstances.push_back({
			pointLightPositions[i],
			k0, k1, k2,
			ambient, diffuse, specular
		});
	}

	SpotLight s =
	{
		Camera::defaultCamera.cameraPos, Camera::defaultCamera.cameraFront,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.5f)),
		1.0f, 0.7f, 0.032f,
		glm::vec4(glm::vec3(0.0f), 1.0f), glm::vec4(glm::vec3(1.0f), 1.0f), glm::vec4(glm::vec3(1.0f), 1.0f)
	};

	mainJ.update();
	if (mainJ.isPresent())
		std::cout << mainJ.getName() << " is present" << std::endl;
	else
		std::cout << "Not present" << std::endl;

	while (!screen.shouldClose())
	{
		box.offsets.clear();
		box.sizes.clear();

		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		processInput(deltaTime);

		screen.update();

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = Camera::defaultCamera.getViewMatrix();
		projection = glm::perspective(glm::radians(Camera::defaultCamera.getZoom()), (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.0f);

		shader.activate();
		shader.Set3Float("viewPos", Camera::defaultCamera.cameraPos);

		launchShader.activate();
		launchShader.Set3Float("viewPos", Camera::defaultCamera.cameraPos);

		dirLight.direction = glm::vec3(
			glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f)) * 
			glm::vec4(dirLight.direction, 1.0f));
		shader.activate();
		dirLight.render(shader);
		launchShader.activate();
		dirLight.render(launchShader);

		for (unsigned int i = 0; i < 4; i++)
		{
			shader.activate();
			lamps.lightInstances[i].render(shader, i);
			launchShader.activate();
			lamps.lightInstances[i].render(launchShader, i);
		}
		
		shader.activate();
		shader.SetInt("noPointLights", 4);
		launchShader.activate();
		launchShader.SetInt("noPointLights", 4);

		if (flashLightOn)
		{
			s.position = Camera::defaultCamera.cameraPos;
			s.direction = Camera::defaultCamera.cameraFront;
			shader.activate();
			s.render(shader, 0);
			shader.SetInt("noSpotLights", 1);
			launchShader.activate();
			s.render(launchShader, 0);
			launchShader.SetInt("noSpotLights", 1);
		}
		else
		{
			shader.activate();
			shader.SetInt("noSpotLights", 0);
		}

		shader.activate();
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);
		m.render(shader, deltaTime, &box);

		std::stack<int> removeObjects;
		for (int i = 0; i < launchObjects.instances.size(); i++)
		{
			if (glm::length(Camera::defaultCamera.cameraPos - launchObjects.instances[i].pos) > 50.0f)
			{
				removeObjects.push(i);
				continue;
			}
		}

		for (int i = 0; i < removeObjects.size(); i++)
		{
			launchObjects.instances.erase(launchObjects.instances.begin() + removeObjects.top());
			removeObjects.pop();
		}

		launchShader.activate();
		if (launchObjects.instances.size() > 0)
		{
			launchShader.SetMat4("view", view);
			launchShader.SetMat4("projection", projection);
			launchObjects.render(launchShader, deltaTime, &box);
		}

		lampShader.activate();
		lampShader.SetMat4("view", view);
		lampShader.SetMat4("projection", projection);
		lamps.render(lampShader, deltaTime, &box);

		// render boxes
		if (box.offsets.size() > 0)
		{
			// if instances exists
			boxShader.activate();
			boxShader.SetMat4("view", view);
			boxShader.SetMat4("projection", projection);
			box.render(boxShader);
		}

		screen.newFrame();
	}

	launchObjects.cleanup();

	lamps.cleanup();

	box.cleanup();

	glfwTerminate();
	return 0;
}
