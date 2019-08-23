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
/*<cpuInitialize>buddhaBrotImportanceMap(200, 30, -2.5, 1, 1, -1)</cpuInitialize>*/
layout(std430, binding = 2) buffer desirabilityMap
{
	// We only really need a vec3- xy for position and z for iteration count. However, due to buggy drivers, the last float is required as padding
	vec4 desirability[];
};
</buffers>

uniform int count;

<constants>
	const float maxIterationsRed = maxIterations/5;
	const float maxIterationsGreen = maxIterations/20;
	const float maxIterationsBlue = maxIterations;
	const int minIterations = 50;
	const int mutationAttemps = 4;
	// The area in the complex plane
	const vec4 screenEdges = vec4(vec2(-2.5, -1), vec2(1, 1));
	const int pointsPerFrame = <pointsPerFrame>;
	const int startPointAttempts = <startPointAttempts>;

	// Compute shaders are weird, for some reason i need to shift x
	#define IndexPoints(X,Y) uint((X)+(Y)*screenSize.x+screenSize.x*(.5))

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
		int seed = int(intHash(abs(int(frame))+i*2+intHash(gl_GlobalInvocationID.x))*intHash(gl_GlobalInvocationID.y))+int(intHash(frame));

    	vec2 w = getStartValue(seed);

		if(w.x<-100) continue;

		mainLoop(w);
	}
</main>

<include>
	complexSquare, intHash, hash2, notInMainCardioid, notInMainBulb, map01ToInterval, EscapeCount, getStartValue, complexTan, complexSin
</include>

<loopTrap>
	<incrementWPosition>points[IndexPoints(int(clamp(mapRange(screenEdges.x, screenEdges.z, 0, screenSize.x, w.x),0,screenSize.x)),
										   int(clamp(screenSize.y-mapRange(screenEdges.y, screenEdges.w, 0, screenSize.y, w.y),0,screenSize.y)))]+=
										   vec4(smoothstep(0,maxIterationsRed,i),smoothstep(0,maxIterationsGreen,i),smoothstep(0,maxIterationsBlue,i),1);</incrementWPosition>,
</loopTrap>

<loopReturn>
	<nothing>;</nothing>,
</loopReturn>

<EscapeCount>
int EscapeCount(vec2 z)
{
	vec2 c = z;
	for (int i = 0; i < maxIterations; i++)
	{
		z=mat2(z,-z.y,z.x)*z+c;
		if (dot(z,z)>4)
		{
			return i;
		}
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
					desirability[index] = vec4(point, escapeCount, -1);
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
			vec2 point = hash2(hash,hash)*0.01+prev.xy; // Return a point we already know is good with a small mutation
			if (notInMainBulb(point) && notInMainCardioid(point))
			{
				float escapeCount = EscapeCount(point);
				if (escapeCount > 0 || escapeCount > minIterations)
				{
					if (escapeCount > prev.z)
					{
						desirability[index] = vec4(point, escapeCount, -1);
					}
					return point;
				}
			}
		}
	}
}
</getStartValue>

<map01ToInterval>
float mapRange(float a1,float a2,float b1,float b2,float s)
{
	return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

float map01ToInterval(float value, vec2 range)
{
	return value*(range.y-range.x)+range.x;
}

vec2 mapTo01(vec2 value, vec4 range)
{
	return clamp(vec2((value.x-range.x)/(range.z-range.x), (value.y-range.y)/(range.w-range.y)),vec2(0), vec2(1));
}

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