#version 330 core

layout(location = 0) out vec4 color;

uniform vec2 screenSize = vec2(1080, 1080);
uniform int worldFlip = -1;
uniform mat2 yawMatrix;
uniform mat2 pitchMatrix;
uniform mat2 rollMatrix;
uniform vec3 position;
uniform vec3 sun;

%uniforms%

const int maxIterations = %maxIterations%;
const int maxSteps = %maxSteps%;
const float sunSize = 1.0;
const float sunTightness = 16.0; // Probably has some physical name, the amount the sun "spreads"
const vec3 light = vec3( -0.707, 0.000,  0.707 );

%constants%

struct Ray
{
	vec3 origin;
	vec3 dir;
};

// Helper functions
%helperFunctions%

// Distance estimator
%distanceEstimator%

// Stuff like multiple distance estimators
%sceneDistance%

// The actual ray marching, implement things like bounding geometry
%trace%

%lightingFunctions%

// Transforming some inputs, calling trace and calculating color and shadow
%render%

void main()
{
	%main%
}
