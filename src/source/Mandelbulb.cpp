#include <glew.h>
#include <windows.h>
#include "headers/Mandelbulb.h"
#include "glm.hpp"
#include "headers/shader.h"
#include "headers/camera.h"
#include <iostream>
#include "headers/Image.h"
#include "headers/Debug.h"
#include <string>
#include <fstream>

#define out 
#define maxIterations 4
#define atan atan2
//<DistanceEstimator>
float Mandelbulb::DistanceEstimator(glm::vec3 start, out glm::vec4 resColor, float Power)
{
	glm::vec3 w = start;
	float m = dot(w,w);

	glm::vec4 trap = glm::vec4(abs(w),m);
	float dz = 1.0;
    
    
	for(int i = 0; i < maxIterations; i++)
	{
#if 0
		float m2 = m*m;
		float m4 = m2*m2;
		dz = Power*sqrt(m4*m2*m)*dz + 1.0;

		float x = w.x; float x2 = x*x; float x4 = x2*x2;
		float y = w.y; float y2 = y*y; float y4 = y2*y2;
		float z = w.z; float z2 = z*z; float z4 = z2*z2;

		float k3 = x2 + z2;
		float k2 = inversesqrt( pow(k3, Power - 1) );
		float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
		float k4 = x2 - y2 + z2;

		w.x = start.x +  64.0*x*y*z*(x2-z2)*k4*(x4-6.0*x2*z2+z4)*k1*k2;
		w.y = start.y + -16.0*y2*k3*k4*k4 + k1*k1;
		w.z = start.z +  -Power*y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4*z4)*k1*k2;
#else
		dz = (Power * pow(sqrt(m), Power - 1)) * dz + 1.0;
		//dz = Power*pow(m,(Power-1)*0.5)*dz + 1.0;
        
		float r = length(w);
		float theta = Power * atan2(w.x, w.z);
		float phi = Power * acos(w.y / r);

		// Fun alternative: reverse sin and cos
		w = start + pow(r, Power) * glm::vec3(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi));
#endif
		//<deformation>

		trap = min(trap, glm::vec4(abs(w),m));
		//vec2 len = distCurve(w, genericParameter);
		//trap = min(trap, vec4(abs(len), atan(len.y,len.x),m));

		m = dot(w,w);
		if( m > 256.0 )
			break;
	}
	
	resColor = glm::vec4(m,trap.y,trap.z,trap.w);

	return 0.25f* log(m)*sqrt(m)/dz;
}
//</DistanceEstimator>

Mandelbulb::Mandelbulb(float power, Camera &camera, glm::vec3 sun, glm::ivec2 screenSize, Time time, int specIndex)
	: Fractal3D(GenerateShader(false, specIndex), GenerateShader(true, specIndex), camera, screenSize, time, sun), power(power), genericParameter(1)
{
	SetVariablesFromSpec(specIndex);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb(float power, Camera &camera, glm::vec3 sun, int specIndex)
	: Fractal3D(GenerateShader(false, specIndex), GenerateShader(true, specIndex), camera, glm::ivec2(Fractal::DefaultWidth, Fractal::DefaultHeight), Time(), sun), power(power), genericParameter(1)
{
	SetVariablesFromSpec(specIndex);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb(float power, Camera &camera, int specIndex)
	: Fractal3D(GenerateShader(false, specIndex), GenerateShader(true, specIndex), camera, glm::ivec2(Fractal::DefaultWidth, Fractal::DefaultHeight), Time(), glm::normalize(glm::dvec3(0.577, 0.577, 0.577))), power(power), genericParameter(1)
{
	SetVariablesFromSpec(specIndex);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

Mandelbulb::Mandelbulb(int specIndex)
	: Fractal3D(GenerateShader(false, specIndex), GenerateShader(true, specIndex)), power(defaultPower), genericParameter(1)
{
	SetVariablesFromSpec(specIndex);
	SetUniformNames();

	SetUniformLocations(explorationShader);
	SetUniforms(explorationShader);
	GlErrorCheck();
}

void Mandelbulb::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	Fractal3D::KeyCallback(window, key, scancode, action, mods);

	switch (key)
	{
		// Changing the power of the fractal
	case GLFW_KEY_C:
		power.value += 0.03f * parameterChangeRate;
		explorationShader.SetUniform(power);
		break;
	case GLFW_KEY_V:
		power.value -= 0.03f * parameterChangeRate;
		explorationShader.SetUniform(power);
		break;

		// Debugging
	case GLFW_KEY_R:
		genericParameter.value += 0.1f * parameterChangeRate;
		explorationShader.SetUniform(genericParameter);
		break;
	case GLFW_KEY_F:
		genericParameter.value -= 0.1f * parameterChangeRate;
		explorationShader.SetUniform(genericParameter);
		break;
		

	default:
		break;
	}
}

void Mandelbulb::SetUniforms(Shader& shader)
{
	GlErrorCheck();
	shader.use();

	shader.SetUniform(power);
	shader.SetUniform(genericParameter);
	Fractal3D::SetUniforms(shader);
}

void Mandelbulb::SetUniformLocations(Shader &shader)
{
	GlErrorCheck();
	power.id = glGetUniformLocation(shader.id, power.name.c_str());
	genericParameter.id = glGetUniformLocation(shader.id, genericParameter.name.c_str());
	Fractal3D::SetUniformLocations(shader);
}

void Mandelbulb::SetUniformNames()
{
	power.name = "power";
	genericParameter.name = "genericParameter";
	Fractal3D::SetUniformNames();
}

void Mandelbulb::Update()
{
	time.PollTime();


	// Move sun in shader
	double time = glfwGetTime();
	sun.value = glm::normalize(glm::vec3(sin(time * 0.25),
		std::abs(sin(time * 0.1)) * -1,
		cos(time * 0.25)));

	genericParameter.value = (float)(abs(cos(time * 0.025) * 0.5 - sin(time * 0.01) * 2));
	explorationShader.SetUniform(sun);
	explorationShader.SetUniform(genericParameter);
}


Shader& Mandelbulb::GenerateShader(bool highQuality, int specIndex)
{
	GlErrorCheck();

	std::string source = readFile(SourcePath);

	std::string base = readFile(Fractal3D::path3DBase);

	std::string specification = readFile(SpecificationPath);

	const size_t length = std::extent<decltype(mandelBulbShaderSections)>::value;
	Fractal3D::ParseShader(source, base, specification, highQuality, specIndex, mandelBulbShaderSections, length);

	std::string vertexSource = readFile(Fractal::pathRectangleVertexshader);

	return *(new Shader(vertexSource, base, false));
}

void Mandelbulb::SetVariablesFromSpec(int index)
{
	std::string specSection = GetSpecificationByIndex(readFile(SpecificationPath), index, readFile(presetSpec));
	std::string variables = getSection(Section("cpuVariables"), specSection);
	if (variables != "")
	{
		std::vector<std::string> variablesList = splitNotInChar(variables, ',', '[', ']');
		for (size_t i = 0; i < variablesList.size(); i++)
		{
			std::string value = getSectionValue(variablesList[i]);
			cleanString(value, { '[', ']' });
			SetVariable(getSectionName(variablesList[i]), value);
		}
	}
}

void Mandelbulb::SetVariable(std::string name, std::string value)
{
	if (name == "power")
	{
		power.value = std::stof(value);
	}
	else if (name == "genericParameter")
	{
		genericParameter.value = std::stof(value);
	}
	else
	{
		Fractal3D::SetVariable(name, value);
	}
}