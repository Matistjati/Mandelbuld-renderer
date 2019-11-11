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

/*<GuiHint>GuiType: slider, Name: Max Iterations, Range: (1, 100)</GuiHint>*/
uniform int maxIterations = 8;

/*<GuiHint>GuiType: slider, Name: Max Steps, Range: (1, 200)</GuiHint>*/
uniform int maxSteps = 100;

/*<GuiHint>GuiType: slider, Name: Anti Aliasing, Range: (1, 5)</GuiHint>*/
uniform float antiAliasing = 1;

/*<GuiHint>GuiType: slider, Name: ray acceptance, Range: (0.00001, 0.1)</GuiHint>*/
uniform float zoomDetailRatio = 0.1;

/*<GuiHint>GuiType: slider, Name: Max Ray Distance, Range: (1, 64)</GuiHint>*/
uniform float maxDist = 16;

/*<GuiHint>GuiType: slider, Name: Shadow hardness, Range: (1, 64)</GuiHint>*/
uniform float shadowSoftness = 4;

/*<GuiHint>GuiType: slider, Name: Escacpe Radius, Range: (1, 1024)</GuiHint>*/
uniform float escapeRadius = 256;

/*<GuiHint>GuiType: slider, Name: Sun Size, Range: (-1, 10)</GuiHint>*/
uniform float sunSize = 1.0;

/*<GuiHint>GuiType: slider, Name: Sun Spread, Range: (-1, 128)</GuiHint>*/
uniform float sunSpread = 16.0;



<uniforms>

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
