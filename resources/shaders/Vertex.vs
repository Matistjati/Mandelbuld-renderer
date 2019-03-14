#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aColor;

uniform int width = 1080;
uniform int height = 1080;

void main()
{
	gl_Position = aPosition;
};