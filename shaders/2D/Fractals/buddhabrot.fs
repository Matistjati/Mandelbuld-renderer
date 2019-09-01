<escapeRadius>1e3</escapeRadius>
<maxIterations>1000</maxIterations>
<maxIterationsRelease>1</maxIterationsRelease>
<pointsPerFrame>1</pointsPerFrame>
<startPointAttempts>20</startPointAttempts>
<renderFrequency>50</renderFrequency>

<type>compute</type>
<render>buddhabrotRender.fs</render>
<localSizeDimensions>2</localSizeDimensions>


<buffers>
/*<bufferType>mainBuffer</bufferType>*/
layout(std430, binding = 0) buffer densityMap
{
	vec4 points[];
};

/*<bufferType>privateBuffer</bufferType>*/
/*<cpuInitialize>buddhaBrotImportanceMap(<maxIterations>, 30, -2.5, 1, 1, -1)</cpuInitialize>*/
layout(std430, binding = 2) buffer desirabilityMap
{
	// We only really need a vec3- xy for position and z for iteration count. However, due to buggy drivers, the last float is required as padding
	vec4 desirability[];
};
</buffers>

<constants>
	const float maxIterationsRed = maxIterations/20;
	const float maxIterationsGreen = maxIterations/5;
	const float maxIterationsBlue = maxIterations;
	const int minIterations = 50;
	const int mutationAttemps = 4;
	// The area in the complex plane
	const vec4 screenEdges = vec4(vec2(-2.5, -1), vec2(1, 1));
	const int pointsPerFrame = <pointsPerFrame>;
	const int startPointAttempts = <startPointAttempts>;

	// Compute shaders are weird, for some reason i need to shift x
	#define IndexPoints(X,Y) uint((X)+(Y)*screenSize.x+screenSize.x*(.5))
	#define Colorwheel 1

</constants>

<extraSections>
[pointsPerFrame], [startPointAttempts]
</extraSections>

<extraParameters>
[vec2 minVal, vec2 maxVal]
</extraParameters>

<main>
	uint fragCoord = IndexPoints(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

	vec2 uv = gl_GlobalInvocationID.xy/screenSize.xy;

	float _;
	for(int i = 0; i < pointsPerFrame; i++)
	{
		int seed = int(intHash(abs(int(frame))+i*2+intHash(gl_GlobalInvocationID.x))*intHash(gl_GlobalInvocationID.y));

    	vec2 w = getStartValue(seed);
		if(w.x<-100) continue;

		mainLoop(w);
	}
</main>

<include>
	complexSquare, intHash, hash2, notInMainCardioid, notInMainBulb, map01ToInterval, EscapeCount, getStartValue, complexTan, complexSin
</include>

<loopTrap>
	<incrementWPosition>
	const float pi2 = 6.28318530717958647692528676655;
	float t = time/pi2;
	// The position we use. We can for example mix between zr, zi to cr, ci for a transition between the buddhabrot and mandelbrot sets.
	// Time will be in range [0,2pi), which we map to [0,1)

	// Mix outside 0,1?

	//vec2 coord = vec2(mix(c,w,t));
	//vec2 coord = vec2(w.x,mix(w.y, c.y, t));
	//vec2 coord = vec2(mix(w.x, c.x, t), mix(c.y,w.y, t));
	//vec2 coord = vec2(mix(w.x, c.x, t), mix(c.y,w.y, t));
	//vec2 coord = vec2(mix(c.x, w.y, t), mix(w.y,c.x, t));
	
	//vec2 coord = vec2(mix(c.x, w.x, 0.1), w.y);
	//vec2 coord = vec2(w.x, mix(w.y,c.x,0.3));
	//vec2 coord = vec2(mix(c.x, w.x,0.5), mix(mix(c.y,w.y,0.5),c.x,0.5));
	
	//vec2 coord = vec2(c.x,w.y); //vec2 c = vec2(w.y,0)
	//vec2 coord = vec2(c.x,mix(c.y,w.x,t)); //vec2 c = vec2(w.y,0) // Bifurcation diagram c = vec2(w.x,0);w=vec2(0);

	vec2 coord = vec2(w);

	// Converting a position in fractal space to image space- google "map one range to another"
	// We are mapping from [screenEdges.x, screenEdges.z) to [0, screenSize.x) for x, corresponding for y
	// That position is then turned into a linear index using 2d array math
	int x = int(clamp((coord.x-screenEdges.x)*map.x,0,screenSize.x));
	int y = int(clamp(screenSize.y-(coord.y-screenEdges.y)*map.y,0,screenSize.y));
	int index = int(
				// X component
				x+
				// Y component
				// The steps are to avoid points outside of the image accumulating on the left and right sides
			    step(1,x)*step(x,screenSize.x-1)*y*screenSize.x+screenSize.x*0.5);


#if Colorwheel
    // Into color
	points[index] += vec4(wheelColor, 1.0);
#else
	// Nebulabrot
	// Smoothstep- more smooth image
	/*points[index] += vec4(smoothstep(0,maxIterationsRed,i),smoothstep(0,maxIterationsGreen,i),smoothstep(0,maxIterationsBlue,i),1);*/

	// Step- too detailed?
	points[index] += vec4(step(i,maxIterationsRed),step(i,maxIterationsGreen),step(i,maxIterationsBlue),1);
#endif
	</incrementWPosition>,
</loopTrap>

<loopSetup>
	<buddhaMapSetup>vec2 c = w;vec2 map = vec2(screenSize.xy/vec2(screenEdges.z-screenEdges.x,screenEdges.w-screenEdges.y));
	#if Colorwheel
	vec2 d = vec2((c.x-screenEdges.x)/(screenEdges.z-screenEdges.x),1.0-(c.y-screenEdges.y)/(screenEdges.w-screenEdges.y))*2-1; 
	float hue = (acos(d.x / length(d))*sign(d.y)+(3.1415926535897932384*1.5))/6.283185307179586476925286766559005768394338798750211641949;
	vec3 wheelColor = hsl2rgb( vec3(hue, 1.0, 0.5));
	#endif</buddhaMapSetup>,
</loopSetup>

<loopReturn>
	<nothing>;</nothing>,
</loopReturn>

<EscapeCount>
vec3 hsl2rgb(vec3 c)
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0,1.0);
    return c.z + c.y * (rgb-0.5)*(1.0-abs(2.0*c.z-1.0));
}

int EscapeCount(vec2 z)
{
	vec2 c = z;
	for (int i = 0; i < maxIterations; i++)
	{
		z=mat2(z,-z.y,z.x)*z+c;
		if (dot(z,z)>4) return i;
	}
	return -1;
}
</EscapeCount>

<getStartValue>
vec2 getStartValue(int seed)
{
	uint hash = uint(seed);

	float c = abs(fract(sin(seed++)*62758.5453123)); // Do a random choice based on the seed

	uint index = uint(gl_GlobalInvocationID.y*screenSize.x+gl_GlobalInvocationID.x); // Accessing desirability like a 2d array
	vec4 prev = desirability[index];
	
	// 40 % chance to mutate with a whole new point, 60% to mutate an existing point with a small step
	if (c > 0.6)
	{
		for(int i = 0; i <startPointAttempts; ++i)
		{
			// Generate a random point
			vec2 random = hash2(hash,hash);
			// Map it from [0,1) to fractal space
			vec2 point = map01ToInterval(random, screenEdges);

			// Checking if it is inside the largest parts of the set which do not escape (avoiding alot of computations, ~10x speedup)
			if (notInMainBulb(point) && notInMainCardioid(point))
			{
				int escapeCount = EscapeCount(point);
				// We only want to iterate points that are interesting enough
				if (escapeCount > minIterations)
				{
					if (escapeCount > prev.z)
					{
						desirability[index] = vec4(point, escapeCount, -1);
					}
					return point;
				}
			}
		}
		return vec2(-1000);
	}
	else
	{
		for (int i = 0; i < mutationAttemps; i++)
		{
			vec2 point = (hash2(hash,hash)*2-1)*.001+prev.xy; // Return a point we already know is good with a small mutation
			if (notInMainBulb(point) && notInMainCardioid(point))
			{
				float escapeCount = EscapeCount(point);
				if (escapeCount > minIterations)
				{
					if (escapeCount > prev.z)
					{
						desirability[index] = vec4(point, escapeCount, -1);
					}
					return point;
				}
			}
		}
		return vec2(-1000);
	}
}
</getStartValue>

<map01ToInterval>
vec2 map01ToInterval(vec2 value, vec4 range)
{
	return vec2(value.x*(range.z-range.x)+range.x, value.y*(range.w-range.y)+range.y);
}
</map01ToInterval>


<mainLoop>
	void mainLoop(vec2 w)
	{
		<loopSetup>
		
		float i = 0;
		for (; i < maxIterations; i++)
		{
			<loopBody>

			<loopExtraOperations>

			<loopTrap>

			<loopBreakCondition>
		}

		return <loopReturn>
	}
</mainLoop>
