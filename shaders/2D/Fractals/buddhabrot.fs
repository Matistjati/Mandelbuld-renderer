<escapeRadius>20</escapeRadius>
<maxIterations>1000</maxIterations>
<maxIterationsRelease>1000</maxIterationsRelease>
<pointsPerFrame>70</pointsPerFrame>
<startPointAttempts>20</startPointAttempts>

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
    vec2 minVal = vec2(uv.x*3.5-2.5,abs(uv.y*2.0-1.0));
    vec2 maxVal = minVal + vec2(3.5,2.0)/screenSize.xy;
    
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
	complexSquare, intHash, hash2, notInMainCardioid, notInMainBulb, getStartValue
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