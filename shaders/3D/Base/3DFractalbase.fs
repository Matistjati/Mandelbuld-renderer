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

/*<GuiHint>GuiType: submenu, Name: Rendering options, Identifier: render</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Sky, Identifier: sky</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Fractal Parameters, Identifier: fractalParams</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Coloring, Identifier: color</GuiHint>*/



/*<GuiHint>GuiType: slider, Name: Max Iterations, Parent: render, Range: (1, 100)</GuiHint>*/
uniform int maxIterations = 40;

/*<GuiHint>GuiType: slider, Name: Max Steps, Parent: render, Range: (1, 1000)</GuiHint>*/
uniform int maxSteps = 100;

/*<GuiHint>GuiType: slider, Name: Anti Aliasing, Parent: render, Range: (1, 5)</GuiHint>*/
uniform float antiAliasing = 1;

/*<GuiHint>GuiType: slider, Name: Fudge Factor, Parent: render, Range: (0.1, 1)</GuiHint>*/
uniform float fudgeFactor = 1;

/*<GuiHint>GuiType: slider, Name: Gamma Correction, Parent: render, Range: (0, 2)</GuiHint>*/
uniform float gamma = 1;

/*<GuiHint>GuiType: slider, Name: ray acceptance, Parent: render, Range: (0.0001, 0.1)</GuiHint>*/
uniform float zoomDetailRatio = 0.001;

/*<GuiHint>GuiType: slider, Name: Max Ray Distance, Parent: render, Range: (1, 64)</GuiHint>*/
uniform float maxDist = 16;

/*<GuiHint>GuiType: slider, Name: Escacpe Radius, Parent: render, Range: (1, 1024)</GuiHint>*/
uniform float escapeRadius = 256;

/*<GuiHint>GuiType: slider, Name: Shadow Darkness, Parent: render, Range: (1, 3)</GuiHint>*/
uniform float shadowDarkness = 1;

/*<GuiHint>GuiType: slider, Name: Specular Strength, Parent: render, Range: (0, 2)</GuiHint>*/
uniform float specularStrength = 1;



/*<GuiHint>GuiType: checkBox, Name: Fog Coloring, Parent: color</GuiHint>*/
uniform bool fogColoring = false;

/*<GuiHint>GuiType: slider, Name: Shadow Softness, Range: (-4, 64), Parent: color</GuiHint>*/
uniform float shadowSoftness = 4;



/*<GuiHint>GuiType: slider, Name: Sun Size, Parent: sky, Range: (-1, 10)</GuiHint>*/
uniform float sunSize = 1.0;

/*<GuiHint>GuiType: slider, Name: Sun Spread, Parent: sky, Range: (-1, 128)</GuiHint>*/
uniform float sunSpread = 16.0;

/*<GuiHint>GuiType: colorPicker, Name: Sun Color, Parent: sky</GuiHint>*/
uniform vec3 sunColor = vec3(0.8,0.7,0.5);

/*<GuiHint>GuiType: colorPicker, Name: Sky Color, Parent: sky</GuiHint>*/
uniform vec3 skyColor = vec3(0.8, 0.95, 1.0);


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
