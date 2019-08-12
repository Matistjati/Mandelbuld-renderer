<escapeRadius>20</escapeRadius>
<maxIterations>1000</maxIterations>
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
	vec4 points[];
};
</buffers>

uniform int count;

<constants>
	const float maxIterationsGreen = maxIterations/2;
	const float maxIterationsBlue = maxIterations/4;
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
    vec3 sum = vec3(0.0);
    for(int i = 0; i < pointsPerFrame; i++)
    {
        int seed = (abs(int(frame))*pointsPerFrame * 2 + i * 2 + int(step(uv.y, 0.5)));

    	vec2 pos = getStartValue(seed);
		if (pos.x<-1000) continue; // We didn't find a point
    	sum += mainLoop(pos,_,minVal,maxVal);
    }

    vec3 prev = points[fragCoord].rgb;
    points[fragCoord] = vec4(prev + sum,1);
</main>

<include>
	complexSquare, intHash, hash2, notInMainCardioid, notInMainBulb, getStartValue, map01ToInterval, complexTan
</include>

<loopTrap>
	<addIfWithinMinMax>vec2 wUpper = vec2(w.x,abs(w.y));
					   vec2 stepped = step(minVal,wUpper)*step(wUpper,maxVal);
					   count += vec3(stepped.x*stepped.y) * vec3(1, step(i,maxIterationsGreen), step(i,maxIterationsBlue));</addIfWithinMinMax>,
</loopTrap>

<loopReturn>
	<steppedCount>step(4.0,dot(w,w))*count;</steppedCount>,
</loopReturn>

<loopSetup>
	<countSetup>vec2 c = w; vec3 count = vec3(0.0);</countSetup>,
</loopSetup>

<loopBreakCondition>
	<distanceBreakReturnCount>if (dot(w,w) > <escapeRadius>) return count;</distanceBreakReturnCount>,
</loopBreakCondition>

<getStartValue>
vec2 getStartValue(int seed)
{
    uint hash = uint(seed);

    vec2 retval = vec2(-1000);
    for(int i = 0; i <startPointAttempts; ++i)
    {
        vec2 random = hash2(hash,hash);
        vec2 point = vec2(random.x * 3.5-2.5,random.y*1.55);

        if (notInMainBulb(point) && notInMainCardioid(point))
		{
			return point;
		}
    }
    return retval;
}
</getStartValue>

<map01ToInterval>
float map01ToInterval(float value, vec2 range)
{
	return value*(range.y-range.x)+range.x;
}

vec2 map01ToInterval(vec2 value, vec4 range)
{
	return vec2(value.x*(range.z-range.x)+range.x, value.y*(range.w-range.y)+range.y);
}
</map01ToInterval>