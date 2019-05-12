#version 430

struct Pixel
{
	float r;
	float g;
	float b;
	float a;
};

layout(std430, binding = 4) buffer Pixels
{
	Pixel p[];
};

void main()
{
	p[gl_GlobalInvocationID.x].r = 1;
	p[gl_GlobalInvocationID.x].g = 0;
	p[gl_GlobalInvocationID.x].b = 0;
	p[gl_GlobalInvocationID.x].a = 1;
}