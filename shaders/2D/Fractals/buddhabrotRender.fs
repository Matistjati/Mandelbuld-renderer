<flags>
	<mainLoopOff>
</flags>

<uniforms>

	/*<GuiHint>GuiType: slider, Name: Brightness, Range: (0, 3000)</GuiHint>*/
	uniform float brightness = 3000;

</uniforms>

<buffers>
/*<bufferType>renderInput</bufferType>*/
layout(std430, binding=0) buffer renderInput
{
    vec4 points[];
};
#define IndexPoints(X,Y) uint(X+(Y-step(3,Y)*3)*screenSize.x+screenSize.x)
</buffers>

<main>
	vec4 col = points[IndexPoints(gl_FragCoord.x, gl_FragCoord.y)];


#if 0
	// Buddhabrot Wx,Wy
	vec4 brightness = points[IndexPoints(0.35875 * screenSize.x, 0.5 * screenSize.y)];

	color = vec4((col.xyz/brightness.xyz)*0.1, 1);
#else
	// An approximation used for buddhagrams. May need manual tweaking for optimal results (it depends on alot of paramters, such as screenSize, miniterations and maxiterations)
	color = vec4(col.xyz/(brightness*float(frame)), 1);
#endif
</main>