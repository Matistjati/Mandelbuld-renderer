#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aColor;
out vec3 color;

void main()
{
	gl_Position = vec4(aPosition.x, aPosition.y, aPosition.z, 1.0f);
	color = aColor;
};