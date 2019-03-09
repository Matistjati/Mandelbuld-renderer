#version 330 core

layout(location = 0) out vec4 color;
in vec4 pos;

void main()
{
	color = vec4(pos.x, pos.y, 0, 1.0);
};