#version 330 core

layout(location = 0) out vec4 color;

uniform vec2 screenSize = vec2(1920, 1080);
uniform vec3 position = vec3(0);
uniform mat3 rotation;
uniform vec2 mousePosition = vec2(0);
uniform vec2 clickPosition = vec2(0);
uniform float zoom = 1;
uniform float time = 0;
uniform float deltaTime = 1/30;
uniform uint frame = uint(0);
uniform bool view3D = true;


/*<GuiHint>GuiType: submenu, Name: Rendering options, Identifier: render</GuiHint>*/


/*<GuiHint>GuiType: submenu, Name: Path tracing, Identifier: pathTrace</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Water, Parent: pathTrace, Identifier: waterSettings</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Cloud coloring, Parent: pathTrace, Identifier: cloudColoring</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Cloud noise settings, Parent: pathTrace, Identifier: cloudNoise</GuiHint>*/


/*<GuiHint>GuiType: submenu, Name: Sky, Identifier: sky</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Fractal Parameters, Identifier: fractalParams</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Coloring, Identifier: color</GuiHint>*/



/*<GuiHint>GuiType: slider, Name: Max Iterations, Parent: render, Range: (1, 100)</GuiHint>*/
uniform int maxIterations = 40;

/*<GuiHint>GuiType: slider, Name: Max Steps, Parent: render, Range: (1, 1000)</GuiHint>*/
uniform int maxSteps = 300;

/*<GuiHint>GuiType: slider, Name: Anti Aliasing, Parent: render, Range: (1, 5)</GuiHint>*/
uniform float antiAliasing = 1;

/*<GuiHint>GuiType: slider, Name: Fudge Factor, Parent: render, Range: (0.1, 3)</GuiHint>*/
uniform float fudgeFactor = 1;

/*<GuiHint>GuiType: slider, Name: Gamma Correction, Parent: render, Range: (0, 2)</GuiHint>*/
uniform float gamma = 1;

/*<GuiHint>GuiType: slider, Name: ray acceptance, Parent: render, Range: (0.0001, 0.001)</GuiHint>*/
uniform float zoomDetailRatio = 0.001;

/*<GuiHint>GuiType: slider, Name: Max Ray Distance, Parent: render, Range: (1, 64)</GuiHint>*/
uniform float maxDist = 16;

/*<GuiHint>GuiType: slider, Name: Escacpe Radius, Parent: render, Range: (1, 1024)</GuiHint>*/
uniform float escapeRadius = 256;

/*<GuiHint>GuiType: slider, Name: Shadow Darkness, Parent: render, Range: (1, 3)</GuiHint>*/
uniform float shadowDarkness = 1;

/*<GuiHint>GuiType: slider, Name: Specular Strength, Parent: render, Range: (0, 2)</GuiHint>*/
uniform float specularStrength = 1;



/*<GuiHint>GuiType: checkBox, Name: Path Trace Image, Parent: pathTrace</GuiHint>*/
uniform bool pathTrace = false;

/*<GuiHint>GuiType: checkBox, Name: Display cloud noise, Parent: pathTrace</GuiHint>*/
uniform bool displayCloudNoise = false;

/*<GuiHint>GuiType: slider, Name: Bounces, Parent: pathTrace, Range: (1, 10)</GuiHint>*/
uniform float bounces = 3;

/*<GuiHint>GuiType: slider, Name: Anti aliasing amount, Parent: pathTrace, Range: (0, 1)</GuiHint>*/
uniform float antiAliasingMagnitude = 1;

/*<GuiHint>GuiType: slider, Name: Brightness, Parent: pathTrace, Range: (0.2, 10)</GuiHint>*/
uniform float brightness = 5;

/*<GuiHint>GuiType: colorPicker, Name: Surface Color, Parent: pathTrace</GuiHint>*/
uniform vec3 surfaceColor = vec3(0.1)*vec3(1.2,1.1,1.0);


/*<GuiHint>GuiType: slider, Name: Water Height, Parent: waterSettings, Range: (-5, 5)</GuiHint>*/
uniform float waterHeight = 0.5;

/*<GuiHint>GuiType: slider, Name: Water bumpiness, Parent: waterSettings, Range: (0, 2)</GuiHint>*/
uniform float bumpFactor = 0.05;

/*<GuiHint>GuiType: slider, Name: Water and sky max distance, Parent: waterSettings, Range: (0, 1000)</GuiHint>*/
uniform float maxWaterDist = 1000;

/*<GuiHint>GuiType: slider, Name: Water darkness, Parent: waterSettings, Range: (0.001, 1)</GuiHint>*/
uniform float waterDarkness = 0.223;

/*<GuiHint>GuiType: slider, Name: Water distance brightness, Parent: waterSettings, Range: (1, 100)</GuiHint>*/
uniform float waterDistScale = 2.57;

/*<GuiHint>GuiType: slider, Name: Water distance max increase, Parent: waterSettings, Range: (1, 4)</GuiHint>*/
uniform float waterDistLimit = 2.51;


/*<GuiHint>GuiType: slider, Name: Darkness threshold, Parent: cloudColoring, Range: (-10, 10)</GuiHint>*/
uniform float darknessThreshold = 10;

/*<GuiHint>GuiType: slider, Name: Phase parameters, Parent: cloudColoring, Range: (0, 3)</GuiHint>*/
uniform vec4 phaseParams = vec4(0.794118, 0.838235, 1.01471,1.36765);

/*<GuiHint>GuiType: slider, Name: Light absorbtion sun, Parent: cloudColoring, Range: (0, 3)</GuiHint>*/
uniform float lightAbsorbtionSun = 0.016129;

/*<GuiHint>GuiType: slider, Name: Light absorbtion through cloud, Parent: cloudColoring, Range: (0, 3)</GuiHint>*/
uniform float lightAbsorptionThroughCloud = 0.532258;

/*<GuiHint>GuiType: slider, Name: Cloud brightness, Parent: cloudColoring, Range: (0.001, 1)</GuiHint>*/
uniform float cloudBrightness = 0.231952;

/*<GuiHint>GuiType: slider, Name: Steps to light, Parent: cloudColoring, Range: (1, 50)</GuiHint>*/
uniform float stepsToLight = 8;

/*<GuiHint>GuiType: slider, Name: Step size, Parent: cloudColoring, Range: (0.005, 0.5)</GuiHint>*/
uniform float stepSize = 0.005;


/*<GuiHint>GuiType: checkBox, Name: Render only clouds, Parent: cloudNoise</GuiHint>*/
uniform bool cloudOnly = false;

/*<GuiHint>GuiType: slider, Name: Noise Scale small, Parent: cloudNoise, Range: (0.0001, 1)</GuiHint>*/
uniform vec3 noiseScaleSmall = vec3(0.107232, 0.362383, 0.154846);

/*<GuiHint>GuiType: slider, Name: Noise weights, Parent: cloudNoise, Range: (0, 1)</GuiHint>*/
uniform vec3 noiseWeights = vec3(0.0357143, 0.107143, 0.0833333);

/*<GuiHint>GuiType: slider, Name: Detail noise weights, Parent: cloudNoise, Range: (0, 1)</GuiHint>*/
uniform vec3 detailWeights = vec3(0.0517241, 0.0238095, 0);

/*<GuiHint>GuiType: slider, Name: Detail noise weights, Parent: cloudNoise, Range: (0, 10)</GuiHint>*/
uniform float detailNoiseWeight = 4.42623;

/*<GuiHint>GuiType: slider, Name: Detail Noise Scale, Parent: cloudNoise, Range: (0.0001, 1)</GuiHint>*/
uniform vec3 detailNoiseScale = vec3(1);

/*<GuiHint>GuiType: slider, Name: Noise persistence, Parent: cloudNoise, Range: (0, 1)</GuiHint>*/
uniform float persistence = 0.293103;

/*<GuiHint>GuiType: slider, Name: Density threshold, Parent: cloudNoise, Range: (-4, 4)</GuiHint>*/
uniform float densityThreshold = -3.72414;

/*<GuiHint>GuiType: slider, Name: Height weight, Parent: cloudNoise, Range: (0.0001, 10)</GuiHint>*/
uniform float heightWeight = 0.800958;

/*<GuiHint>GuiType: slider, Name: Density level, Parent: cloudNoise, Range: (0, 20)</GuiHint>*/
uniform float densityLevel = 13.6905;

/*<GuiHint>GuiType: slider, Name: Density Offset, Parent: cloudNoise, Range: (0, 10)</GuiHint>*/
uniform float densityOffset = 2.14286;

/*<GuiHint>GuiType: slider, Name: Edge density, Parent: cloudNoise, Range: (0.001, 1)</GuiHint>*/
uniform float edgeDensity = 0.785929;

/*<GuiHint>GuiType: slider, Name: Cloud distance, Parent: cloudNoise, Range: (0.001, 0.1)</GuiHint>*/
uniform float cloudDist = 0.0572489;




/*<GuiHint>GuiType: slider, Name: Cloud box position, Parent: pathTrace, Range: (-50, 50)</GuiHint>*/
uniform vec3 boxPos = vec3(0,20,0);

/*<GuiHint>GuiType: slider, Name: Cloud box width, Parent: pathTrace, Range: (0, 500)</GuiHint>*/
uniform vec3 boxWidth = vec3(40,10,40);




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

#define sun normalize(vec3(sin(time * 0.25),abs(sin(time * 0.1)),cos(time * 0.25)))

<buffers>

<constants>

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
