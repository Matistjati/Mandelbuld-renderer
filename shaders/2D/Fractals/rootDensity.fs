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

	// The area in the complex plane we render
	// ((left edge, bottom edge), (right edge, top edge))
	/*<GuiHint>GuiType: Slider, Name: Render Area, Parent: renderParams, Range: (-10, 10)</GuiHint>*/
	uniform vec4 renderArea = vec4(-1, -1, 1, 1);
</uniforms>

<buffers>
/*<bufferType>mainBuffer</bufferType>*/
/*<shouldBeCleared>checkBox, resetFrame, onUniformChange: [maxIterations, position, renderArea, zoom]</shouldBeCleared>*/
layout(std430, binding = 0) buffer densityMap
{
	vec4 points[];
};
</buffers>

<constants>
	// Compute shaders are weird, for some reason i need to shift x
	#define IndexPoints(X,Y) uint((X)+(Y)*screenSize.x)
	const int size = 10;
</constants>


<main>
	uint fragCoord = IndexPoints(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

	vec2 uv = gl_GlobalInvocationID.xy/screenSize.xy;
	
	int degree = 9;
	vec2 poly[size];
	vec2 roots[size-1];
	for (int i = 0; i < size - 1; i++)
	{
		roots[i] = vec2(0);
	}
	for (int i = 0; i < size; i++)
	{
		float theta = i*float(gl_GlobalInvocationID.x)*float(gl_GlobalInvocationID.y)*time;
		poly[i] = vec2(sin(theta),0);
	}
	
	FindAllRoots(poly, degree, roots);

	vec2 midPoint = vec2(abs(renderArea.x)-abs(renderArea.z),abs(renderArea.y)-abs(renderArea.w))*0.5;
	vec4 area = (renderArea+midPoint.xyxy)*zoom-midPoint.xyxy;
	area += vec4(position.xyxy)*vec4(1,-1,1,-1);
	vec2 map = vec2(screenSize.xy/vec2(area.z-area.x,area.w-area.y));

	for (int i = 0; i < size - 1; i++)
	{
		float x = round(clamp((roots[i].x-area.x)*map.x,0,screenSize.x)-0.5);

		float y = round(screenSize.y-(roots[i].y-area.y)*map.y);
		int index = int(x + screenSize.x * (y + 0.5));
		points[index] += vec4(0.4,0.2,0.8,0);
	}

	// Gpu debugging
	/*vec2 value = abs(roots[2]);
	value /= 10;
	float signum = ((value.x > 0) ? 0.5 : 0) + ((value.y > 0) ? 0.25 : 0);
	points[int(gl_GlobalInvocationID.x+gl_GlobalInvocationID.y*screenSize.x)] = vec4(abs(value.xy), signum, 1);*/
</main>


<mainLoop>
	void mainLoop(vec2 c, vec4 area)
	{

	}
</mainLoop>
