#version 330 core

layout(location = 0) in vec4 pixelPos;

void main()
{
	gl_Position = pixelPos;
};