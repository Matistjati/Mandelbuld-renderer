<flags>
	<mainLoopOff>
</flags>

<uniforms>
	/*<GuiHint>GuiType: slider, Name: Brightness, Range: (1, 200), Parent: color</GuiHint>*/
	uniform float brightness = 150;
	
	/*<GuiHint>GuiType: slider, Name: exposure, Range: (0.01, 2), Parent: color</GuiHint>*/
	uniform float exposure = 1;
	
	/*<GuiHint>GuiType: slider, Name: Gamma Correction, Range: (0, 2), Parent: color</GuiHint>*/
	uniform float gamma = 1;
</uniforms>

<buffers>
/*<bufferType>renderInput</bufferType>*/
layout(std430, binding=0) buffer renderInput
{
	vec4 points[];
};
#define IndexPoints(X,Y) uint(X + screenSize.x * Y)
</buffers>

<main>
	// Remap from [0.5, screenSize+0.5] to [0, screenSize]
	vec4 coord = gl_FragCoord - 0.5;

	vec4 col = points[IndexPoints(coord.x, coord.y)];

	color = pow(vec4(col.xyz/(brightness/exposure*frame), 1), vec4(gamma));
</main>