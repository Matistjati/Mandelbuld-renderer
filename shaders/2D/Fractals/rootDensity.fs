<include>
	polynomial,
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
	const int size = 3;
</constants>


<main>
	uint fragCoord = IndexPoints(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

	vec2 uv = gl_GlobalInvocationID.xy/screenSize.xy;
	
	int degree = 2;
	vec2 poly[size] = vec2[](vec2(2,0),vec2(-2,0),vec2(1,0));

	// Gpu debugging
	vec2 difference = (FindRoot(poly, degree));
	difference /= 10;
	float signum = ((difference.x > 0) ? 0.5 : 0) + ((difference.y > 0) ? 0.25 : 0);
	points[int(gl_GlobalInvocationID.x+gl_GlobalInvocationID.y*screenSize.x)] = vec4(abs(difference.xy), signum, 1);
</main>


<mainLoop>
	void mainLoop(vec2 c, vec4 area)
	{

	}
</mainLoop>
