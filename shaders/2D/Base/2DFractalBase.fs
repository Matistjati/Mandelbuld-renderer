#version 330 core

layout(location = 0) out vec4 color;

uniform vec2 screenSize = vec2(1920, 1080);
uniform vec2 position = vec2(0,0);
uniform float zoom = 1;
uniform float time = 0;

<uniforms>

const int maxIterations = <maxIterations>;
const float antiAliasing = <antiAliasing>;

<constants>

// Helper functions
<helperFunctions>

// Iterate a given point and return a color
<mainLoop>

void main()
{
	<main>
}
