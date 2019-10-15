#version 330 core

layout(location = 0) out vec4 color;

uniform vec2 screenSize = vec2(1920, 1080);
uniform int worldFlip = -1;
uniform vec3 position;
uniform vec3 sun;
uniform mat3 rotation;
uniform float zoom;
uniform float time;
uniform uint frame = uint(0);

/*<GuiHint>slider, Max Iterations, 1, 100</GuiHint>*/
uniform int maxIterations = 8;

/*<GuiHint>slider, Max Steps, 1, 200</GuiHint>*/
uniform int maxSteps = 100;

/*<GuiHint>slider, ray acceptance, 0.0000001, 0.1</GuiHint>*/
uniform float zoomDetailRatio = 0.1;

/*<GuiHint>slider, Max Ray Distance, 1, 64</GuiHint>*/
uniform float maxDist = 16;

/*<GuiHint>slider, Shadow hardness, 1, 64</GuiHint>*/
uniform float shadowSoftness = 4;

/*<GuiHint>slider, Escacpe Radius, 1, 1024</GuiHint>*/
uniform float escapeRadius = 256;

/*<GuiHint>slider, Sun Size, -1, 10</GuiHint>*/
uniform float sunSize = 1.0;

/*<GuiHint>slider, Sun Spread, -1, 128</GuiHint>*/
uniform float sunSpread = 16.0;



<uniforms>

const float antiAliasing = <antiAliasing>;
const vec3 light = vec3(-0.707107, 0.000, 0.707107);

<constants>

struct Ray
{
	vec3 origin;
	vec3 dir;
};

// Helper functions
<helperFunctions>

// Distance estimator
<distanceEstimator>

// Stuff like multiple distance estimators
<sceneDistance>

// The actual ray marching, implement things like bounding geometry
<trace>

<lightingFunctions>

// Transforming some inputs, calling trace and calculating color and shadow
<render>

void main()
{
	<main>
}
