<flags>
	<mainLoopOff>
</flags>

<uniforms>
	/*<GuiHint>GuiType: slider, Name: Brightness, Range: (1, 10000), Parent: color</GuiHint>*/
	uniform float brightness = 150;
	
	/*<GuiHint>GuiType: slider, Name: exposure, Range: (0, 2), Parent: color</GuiHint>*/
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
#define IndexPoints(X,Y) uint(X+Y*screenSize.x)
</buffers>

<main>
	vec4 col = points[IndexPoints(gl_FragCoord.x, gl_FragCoord.y)];

	color = vec4(col.xyz/(brightness*time), 1);
</main>