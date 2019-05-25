#include "headers/Fractal3d.h"
#include "headers/Image.h"
#include "headers/GlError.h"

inline bool fileExists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void Fractal3D::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_REPEAT && action != GLFW_PRESS)
	{
		return;
	}

	if (key == GLFW_KEY_S && (mods && GLFW_MOD_CONTROL) == 1)
	{
		bool foundEmptyFile = false;
		const std::string baseName = "image";
		int count = 0;
		while (!foundEmptyFile)
		{
			if (!fileExists((baseName + std::to_string(count) + ".png")))
			{
				foundEmptyFile = true;
				SaveImage(baseName + std::to_string(count) + ".png");
			}
			count++;
		}
		return;
	}

	switch (key)
	{
		// Exit
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, 1);
		break;

		// WASD movement
	case GLFW_KEY_W:
		camera.ProcessMovement(Camera_Movement::forward, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;
	case GLFW_KEY_S:
		camera.ProcessMovement(Camera_Movement::back, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;
	case GLFW_KEY_A:
		camera.ProcessMovement(Camera_Movement::left, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;
	case GLFW_KEY_D:
		camera.ProcessMovement(Camera_Movement::right, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;

		// Up and down
	case GLFW_KEY_SPACE:
		camera.ProcessMovement(Camera_Movement::up, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;
	case GLFW_KEY_LEFT_SHIFT:
		camera.ProcessMovement(Camera_Movement::down, static_cast<float>(time.deltaTime));
		explorationShader.SetUniform(camera.position);
		break;

		// World stuff
	case GLFW_KEY_Z:
		camera.worldFlip.value *= -1;
		explorationShader.SetUniform(camera.worldFlip);
		break;
	case GLFW_KEY_X:
		camera.movementReverse *= -1;
		break;

		// Camera roll
	case GLFW_KEY_Q:
		camera.ProcessRoll(static_cast<float>(camera.rollSpeed * time.deltaTime));
		explorationShader.SetUniform(camera.rollMatrix);
		break;
	case GLFW_KEY_E:
		camera.ProcessRoll(-static_cast<float>(camera.rollSpeed * time.deltaTime));
		explorationShader.SetUniform(camera.rollMatrix);
		break;

	default:
		break;
	}
	GlErrorCheck();
}


Fractal3D::Fractal3D(Shader& explorationShader, Shader& renderShader, Camera& camera, glm::ivec2 screenSize, Time time)
	: Fractal(explorationShader, renderShader, screenSize), time(time), camera(camera)
{
}

Fractal3D::Fractal3D(Shader& explorationShader, Shader& renderShader)
	: Fractal(explorationShader, renderShader, glm::ivec2(DefaultWidth, DefaultHeight)), time(Time()), camera(*(new Camera(glm::vec3(1.8f, 0.8f, -0.6f), // Position
		169, -14, 0.001f, // Yaw, pitch, roll
		0.1f, 3, 200))) // mouseSensitivity, movementSpeed, rollSpeed
{}

void Fractal3D::MouseCallback(GLFWwindow* window, double x, double y)
{
	glm::vec2 newPos = glm::vec2(x, y);
	if (firstMouse)
	{
		mouseOffset = newPos;
		firstMouse = false;
	}

	camera.ProcessMouseMovement(static_cast<float>(newPos.x - mouseOffset.x),
		static_cast<float>(mouseOffset.y - newPos.y) // reversed since y-coordinates go from bottom to top
	);

	mouseOffset = newPos;

	explorationShader.SetUniform(camera.pitchMatrix);
	explorationShader.SetUniform(camera.yawMatrix);
	GlErrorCheck();
}

void Fractal3D::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	screenSize.value = glm::ivec2(width, height);
	Fractal::explorationShader.SetUniform(screenSize.id, width);
	Fractal::explorationShader.SetUniform(screenSize.id, height);

	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	GlErrorCheck();
}

void Fractal3D::SetUniforms(Shader& shader)
{
	shader.SetUniform(camera.position);
	shader.SetUniform(camera.yawMatrix);
	shader.SetUniform(camera.pitchMatrix);
	shader.SetUniform(camera.rollMatrix);
	shader.SetUniform(camera.worldFlip);
	shader.SetUniform(screenSize);
	GlErrorCheck();
}

void Fractal3D::SetUniformLocations(Shader& shader)
{
	camera.pitchMatrix.id = glGetUniformLocation(shader.id, camera.pitchMatrix.name.c_str());
	camera.rollMatrix.id = glGetUniformLocation(shader.id, camera.rollMatrix.name.c_str());
	camera.yawMatrix.id = glGetUniformLocation(shader.id, camera.yawMatrix.name.c_str());
	camera.position.id = glGetUniformLocation(shader.id, camera.position.name.c_str());
	camera.worldFlip.id = glGetUniformLocation(shader.id, camera.worldFlip.name.c_str());
	screenSize.id = glGetUniformLocation(shader.id, screenSize.name.c_str());
	GlErrorCheck();
}

void Fractal3D::SetUniformNames()
{
	camera.pitchMatrix.name = "pitchMatrix";
	camera.yawMatrix.name = "yawMatrix";
	camera.rollMatrix.name = "rollMatrix";
	camera.position.name = "eye";
	camera.worldFlip.name = "worldFlip";
	screenSize.name = "screenSize";
}

void Fractal3D::SaveImage(const std::string path)
{
	renderShader.use();
	SetUniformLocations(renderShader);
	SetUniforms(renderShader);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	Pixel* data = (Pixel*)malloc(screenSize.value.x * screenSize.value.y * 4);
	glReadPixels(0, 0, screenSize.value.x, screenSize.value.y, GL_RGBA, GL_UNSIGNED_BYTE, data);


	Image image(screenSize.value.x, screenSize.value.y, data);
	image.FlipVertically();
	image.Rotate180();

	image.Save(path.c_str());

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	explorationShader.use();
	GlErrorCheck();
}

void Fractal3D::Update()
{
	time.PollTime();
}
