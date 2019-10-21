#version 330 core

layout(location = 0) out vec4 color;

uniform vec2 screenSize = vec2(1920, 1080);
uniform vec2 position = vec2(0,0);
uniform vec2 mousePosition = vec2(0,0);
uniform vec4 clickPositions = vec4(0);
uniform float zoom = 1;
uniform float time = 0;
uniform float deltaTime = 1/30;
uniform float power = 2;
uniform uint frame = uint(0);

<uniforms>


/*<GuiHint>GuiType: slider, Name: Max Iterations, Range: (1, 1024)</GuiHint>*/
uniform float maxIterations = 256;

/*<GuiHint>GuiType: slider, Name: Escape Radius, Range: (1, 1024)</GuiHint>*/
uniform float escapeRadius = 8.;

/*<GuiHint>GuiType: slider, Name: Color change rate, Range: (0.01, 2)</GuiHint>*/
uniform float colorPeriodicity = 0.1;

/*<GuiHint>GuiType: colorPicker, Name: Color A</GuiHint>*/
uniform vec3 colorA = vec3(0.7, 0.5, 0.1);

/*<GuiHint>GuiType: colorPicker, Name: Color B</GuiHint>*/
uniform vec3 colorB = vec3(0.2, 0.9, 0.9);

const float antiAliasing = <antiAliasing>;


<buffers>

<constants>

// Helper functions
<helperFunctions>

// Iterate a given point and return a color
<mainLoop>

void main()
{
	<main>
}
