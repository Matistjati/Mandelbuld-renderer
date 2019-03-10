#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aColor;

//out vec3 color;

out vec4 pos;
void main()
{
	gl_Position = vec4(aPosition.xyz, 1.0f);
	//color = aColor;
	pos = aPosition;
};