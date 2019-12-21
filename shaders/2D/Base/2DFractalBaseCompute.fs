#version 430 core

layout(local_size_x = <sizeX>, local_size_y = <sizeY>, local_size_z = <sizeZ>) in;


uniform vec2 screenSize = vec2(1920, 1080);
uniform vec2 position = vec2(0,0);
uniform vec2 mousePosition = vec2(0,0);
uniform float zoom = 1;
uniform float time = 0;
uniform float power = 2;
uniform uint frame = uint(0);

/*<GuiHint>GuiType: submenu, Name: Fractal Parameters, Identifier: fractalParams</GuiHint>*/


/*<GuiHint>GuiType: slider, Name: Max Iterations, Range: (1, 1024)</GuiHint>*/
uniform float maxIterations = 256;

/*<GuiHint>GuiType: slider, Name: Escape Radius, Range: (1, 128)</GuiHint>*/
uniform float escapeRadius = 8.;



<uniforms>

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