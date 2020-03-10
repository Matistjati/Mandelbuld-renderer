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
uniform bool view3D = false;


/*<GuiHint>GuiType: submenu, Name: Fractal Parameters, Identifier: fractalParams</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Rendering Parameters, Identifier: renderParams</GuiHint>*/

/*<GuiHint>GuiType: submenu, Name: Coloring, Identifier: color</GuiHint>*/


<uniforms>

/*<GuiHint>GuiType: slider, Name: Max Iterations, Parent: renderParams, Range: (1, 4096)</GuiHint>*/
uniform float maxIterations = 256;

/*<GuiHint>GuiType: slider, Name: Escape Radius, Parent: renderParams, Range: (1, 1024)</GuiHint>*/
uniform float escapeRadius = 8.;



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
