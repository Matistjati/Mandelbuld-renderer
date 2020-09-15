<include>

</include>


<type>compute</type>
<renderFrequency>1</renderFrequency>
<render>rootDensityRender.fs</render>
<localSizeDimensions>2</localSizeDimensions>


<uniforms>
	/*<GuiHint>GuiType: slider, Name: Max Iterations, Parent:renderParams, Range: (1, 1024)</GuiHint>*/
	uniform float maxIterations = 256;
</uniforms>

<buffers>
/*<bufferType>mainBuffer</bufferType>*/
/*<shouldBeCleared>checkBox, resetFrame, onUniformChange: [maxIterations]</shouldBeCleared>*/
layout(std430, binding = 0) buffer densityMap
{
	vec4 points[];
};
</buffers>

<constants>
	// Compute shaders are weird, for some reason i need to shift x
	#define IndexPoints(X,Y) uint((X)+(Y)*screenSize.x)
</constants>


<main>
	uint fragCoord = IndexPoints(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

	vec2 uv = gl_GlobalInvocationID.xy/screenSize.xy;
	points[int(gl_GlobalInvocationID.x+gl_GlobalInvocationID.y*screenSize.x)] = vec4(0.5,0.2, 0.8, 1);
</main>


<mainLoop>
	void mainLoop(vec2 c, vec4 area)
	{

	}
</mainLoop>
