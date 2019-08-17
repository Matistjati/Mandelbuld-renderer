<escapeRadius>20</escapeRadius>
<maxIterations>200</maxIterations>
<maxIterationsRelease>1000</maxIterationsRelease>
<pointsPerFrame>70</pointsPerFrame>
<startPointAttempts>20</startPointAttempts>
<renderFrequency>50</renderFrequency>

<type>compute</type>
<render>buddhabrotRender.fs</render>
<localSizeDimensions>2</localSizeDimensions>


<buffers>
/*<bufferType>mainBuffer</bufferType>*/
layout(std430, binding = 0) buffer densityMap
{
	uvec4 points[];
};

/*<bufferType>privateBuffer</bufferType>*/
layout(std430, binding = 2) buffer desirabilityMap
{
	// We only really need a vec3- xy for position and z for iteration count. However, due to buggy drivers, the last float is required as padding
	vec4 desirability[];
};
</buffers>

uniform int count;

<constants>
	const float maxIterationsGreen = maxIterations/2;
	const float maxIterationsBlue = maxIterations/4;
	const int minIterations = 50;
	const vec4 screenEdges = vec4(vec2(-2.5, 1), vec2(1, -1));
	const int pointsPerFrame = <pointsPerFrame>;
	const int startPointAttempts = <startPointAttempts>;
</constants>

<extraSections>
[pointsPerFrame], [startPointAttempts]
</extraSections>

<extraParameters>
[vec2 minVal, vec2 maxVal]
</extraParameters>

<main>
	uint fragCoord = gl_GlobalInvocationID.y*int(screenSize.x)+gl_GlobalInvocationID.x;

	vec2 uv = gl_GlobalInvocationID.xy/screenSize.xy;
    vec2 minVal = map01ToInterval(uv, screenEdges);
    vec2 maxVal = minVal + vec2(abs(screenEdges.x) + abs(screenEdges.z), abs(screenEdges.y) + abs(screenEdges.w))/screenSize.xy;
    
	float _;    
    uvec4 sum = uvec4(0);
    for(int i = 0; i < pointsPerFrame; i++)
    {
        int seed = (abs(int(frame))*pointsPerFrame * 2 + i * 2 + int(step(uv.y, 0.5)));

    	vec2 pos = getStartValue(seed);
		if(pos.x<-100) continue;
    	sum += mainLoop(pos,_,minVal,maxVal);
    }

    uvec4 prev = points[fragCoord];
    points[fragCoord] = prev + sum;
</main>

<include>
	complexSquare, intHash, hash2, notInMainCardioid, notInMainBulb, map01ToInterval, EscapeCount, getStartValue, complexTan
</include>

<loopTrap>
	<addIfWithinMinMax>count += int(clamp(3.-length((w-p)*screenSize.xy),0.,1.))*uvec4(1, step(i,maxIterationsGreen), step(i,maxIterationsBlue), 1);</addIfWithinMinMax>,
</loopTrap>

<loopReturn>
	<steppedCount>uvec4(step(4.0,dot(w,w))*count);</steppedCount>,
</loopReturn>

<loopSetup>
	<countSetup>vec2 c = w; uvec4 count = uvec4(0.0); vec2 p = mix(minVal, maxVal, 0.5);</countSetup>,
</loopSetup>

<loopBreakCondition>
	<distanceBreakReturnCount>if (dot(w,w) > <escapeRadius>) return count;</distanceBreakReturnCount>,
</loopBreakCondition>

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

	float c = abs(fract(sin(seed)*62758.5453123)); // Do a random choice based on the seed

	uint index = uint(gl_GlobalInvocationID.y*screenSize.x+gl_GlobalInvocationID.x); // Accessing desirability like a 2d array
	vec4 prev = desirability[index];
	
	if (c > 0.5)
	{
		for(int i = 0; i <startPointAttempts; ++i)
		{
			// Generate a random point
			vec2 random = hash2(hash,hash);
			// Map it from [0,1) to fractal space
			vec2 point = vec2(random.x * 3.5-2.5,random.y*1.55);

			// Checking if it is inside the largest parts of the set which do not escape (avoiding alot of computations, ~10x speedup)
			if (notInMainBulb(point) && notInMainCardioid(point))
			{
				int escapeCount = EscapeCount(point);
				// We only want to iterate points that are interesting enough
				if (escapeCount > minIterations)
				{
					// Is the point better than the old champion for this pixel? If so, replace it. Also has a 20% chance to mutate as to ensure that we get a wide coverage of different points
					if (float(escapeCount) > prev.z || c > 0.8)
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
		return prev.xy+hash2(hash,hash)*0.01; // Return a point we already know is good with a small mutation
	}
}
</getStartValue>

<map01ToInterval>
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
	uvec4 mainLoop(vec2 w, out float iterations<extraParameters>)
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

		iterations = i/maxIterations;

		return <loopReturn>
	}
</mainLoop>