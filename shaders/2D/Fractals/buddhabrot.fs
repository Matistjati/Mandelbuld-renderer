<include>
	complexPow, complexSquare, intHash, hash2, notInMainCardioid, notInMainBulb, map01ToInterval, complexTan, complexSin, EscapeCount, hslToRgb, getStartValue, 
</include>


<type>compute</type>
<renderFrequency>1</renderFrequency>
<render>buddhabrotRender.fs</render>
<localSizeDimensions>2</localSizeDimensions>

<uniforms>
	/*<GuiHint>GuiType: slider, Name: Power, Parent: fractalParams, Range: (-2, 8)</GuiHint>*/
	uniform float power = 2;
	
	/*<GuiHint>GuiType: slider, Name: X Rotation, Parent: fractalParams, Range: (-2, 2)</GuiHint>*/
	uniform vec3 xRot = vec3(0);

	/*<GuiHint>GuiType: slider, Name: Y Rotation, Parent: fractalParams, Range: (-2, 2)</GuiHint>*/
	uniform vec3 yRot = vec3(0);
	
	/*<GuiHint>GuiType: checkBox, Name: Color Wheel, Parent: color</GuiHint>*/
	uniform bool colorWheel = true;
	
	/*<GuiHint>GuiType: checkBox, Name: Sample partially wrong, Parent: color</GuiHint>*/
	uniform bool invalidSamples = false;
	
	/*<GuiHint>GuiType: Slider, Name: Color Offset, Parent: color, Range: (-1, 1)</GuiHint>*/
	uniform float colorOffset = 0;
	
	/*<GuiHint>GuiType: Slider, Name: Color Channel Max, Parent: color, Range: (0, 1)</GuiHint>*/
	uniform vec3 colorIteration = vec3(0.05, 0.2, 1);
	
	/*<GuiHint>GuiType: Slider, Name: Rendering Amount, Parent: renderParams, Range: (0.01, 1)</GuiHint>*/
	uniform float renderSize = 0.5;
	
	/*<GuiHint>GuiType: Slider, Name: Mutation size, Parent: renderParams, Range: (0.0001, 0.1)</GuiHint>*/
	uniform float mutationSize = 0.001;
	
	/*<GuiHint>GuiType: Slider, Name: Min Iterations, Parent: renderParams, Range: (1, 500)</GuiHint>*/
	uniform float minIterations = 50;
	
	/*<GuiHint>GuiType: Slider, Name: Point finding attempts by random, Parent: renderParams, Range: (1, 100)</GuiHint>*/
	uniform float startPointAttempts = 20;

	/*<GuiHint>GuiType: Slider, Name: Point finding attempts by mutation, Parent: renderParams, Range: (1, 30)</GuiHint>*/
	uniform float mutationAttemps = 4;

	/*<GuiHint>GuiType: Slider, Name: Chance for new point, Parent: renderParams, Range: (0, 1)</GuiHint>*/
	uniform float mutationChance = 0.8;
	
	/*<GuiHint>GuiType: Slider, Name: Points Per Frame, Parent: renderParams, Range: (1, 5)</GuiHint>*/
	uniform float pointsPerFrame = 1;
	
	// The area in the complex plane we render
	// ((left edge, bottom edge), (right edge, top edge))
	/*<GuiHint>GuiType: Slider, Name: Render Area, Parent: renderParams, Range: (-10, 10)</GuiHint>*/
	uniform vec4 renderArea = vec4(-2.5, -1, 1, 1);
</uniforms>

<buffers>
/*<bufferType>mainBuffer</bufferType>*/
/*<shouldBeCleared>checkBox, resetFrame, onUniformChange: [xRot, yRot, colorWheel, invalidSamples, colorOffset, colorIteration, renderArea, power, position, zoom, maxIterations, minIterations, mutationSize]</shouldBeCleared>*/
layout(std430, binding = 0) buffer densityMap
{
	vec4 points[];
};

/*<bufferType>privateBuffer</bufferType>*/
/*<cpuInitialize>buddhaBrotPoints</cpuInitialize>*/
/*<shouldBeCleared>button, onUniformChange: [power, maxIterations, minIterations, mutationSize]</shouldBeCleared>*/
layout(std430, binding = 1) buffer desirabilityMap
{
	// We only really need a vec3- xy for position and z for iteration count. However, due to buggy drivers, the last float is required as padding
	vec4 desirability[];
};
</buffers>

<constants>
	// Compute shaders are weird, for some reason i need to shift x
	#define IndexPoints(X,Y) uint((X)+(Y)*screenSize.x+screenSize.x*(.5))
	#define Camera 0
	// Numerical constants
	#define PI_ONE_POINT_FIVE 4.7123889803846898576939650749192543262957540990626587
	#define PI_TWO 6.283185307179586476925286766559005768394338798750211641949889184
	#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078
</constants>

<main>
	uint fragCoord = IndexPoints(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

	vec2 uv = gl_GlobalInvocationID.xy/screenSize.xy;

#if Camera
	vec4 area = renderArea;
#else
	// When multiplying to zoom, we zoom towards 0. However, we want to zoom towards the midPoint of screenSize.x and screenSize.z.
	// To accomplish this, we want to find a number, b such that "renderArea.x+b=renderArea.z+b". Solving this equation yields the definition of midPoint.
	// Thus, we add b to both screenSize.x and screenSize.z sides such that 0 is in the center and then translate back by subtracting b.
	vec2 midPoint = vec2(abs(renderArea.x)-abs(renderArea.z),abs(renderArea.y)-abs(renderArea.w))*0.5;
	vec4 area = (renderArea+midPoint.xyxy)*zoom-midPoint.xyxy;
	area += vec4(position.xyxy)*vec4(1,-1,1,-1);
#endif

	// Computing long paths area expensive. To run at a decent framerate, we don't render points for every pixel
	// renderSize is raised to the 4th power, due to the slider being linear while renderSize is not. We do not use "pow(renderSize, 4)", as this is most likely slower than multiplying it by itself 4 times
	if (gl_GlobalInvocationID.x < screenSize.x*(renderSize*renderSize*renderSize*renderSize) || gl_GlobalInvocationID.y < screenSize.y*(renderSize*renderSize*renderSize*renderSize))
	{
		for(int i = 0; i < pointsPerFrame; i++)
		{
			int seed = int(intHash(abs(int(frame))+i*2+intHash(gl_GlobalInvocationID.x))*intHash(gl_GlobalInvocationID.y));
    		vec2 w = getStartValue(seed, area);
			if (w.x<-100) continue;

			mainLoop(w, area);
		}
	}
</main>



<loopTrap>
	<incrementWPosition>
	//float t = time/PI_TWO;
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

	//vec2 coord = c;
	if(!insideBox(w,area))
	{
		continue;
	}

#if Camera
	vec3 p = vec3(w.xy,c.x);

	mat4 mat = getPosMatrix(vec3(p)) * cam;

	p = (vec4(p,1)*mat).xyz;
	p.xy /= p.z*zoom;
	vec2 coord = p.xy;
	if (p.z>0) continue;
#else
	vec4 pos = vec4(w, c);
	vec2 coord = pos.xy;
	coord.x = mix(coord.x, pos.y, xRot.x);
	coord.x = mix(coord.x, pos.z, xRot.y);
	coord.x = mix(coord.x, pos.w, xRot.z);
	
	coord.y = mix(coord.y, pos.x, yRot.x);
	coord.y = mix(coord.y, pos.z, yRot.y);
	coord.y = mix(coord.y, pos.w, yRot.z);
#endif
	
	// Converting a position in fractal space to image space- google "map one range to another"
	// We are mapping from [renderArea.x, renderArea.z) to [0, screenSize.x) for x, corresponding for y
	// That position is then turned into a linear index using 2d array math
	int x = int(clamp((coord.x-area.x)*map.x,0,screenSize.x)-0.5);
	// The steps are to avoid points outside of the image accumulating on the left and right sides
	int y = int(screenSize.y-(coord.y-area.y)*map.y);
	int index = int(x + screenSize.x * (y + 0.5));

	if (colorWheel)
	{
		points[index].xyz += color;
	}
	else
	{
		// Nebulabrot
		// Smoothstep- more smooth image
		/*points[index].xyz += smoothstep(vec3(pos.xyz), vec3(i),colorIteration*maxIterations);*/

		// Step- too detailed?
		points[index].xyz += color*step(vec3(i),colorIteration*maxIterations);
	}
	</incrementWPosition>,
</loopTrap>

<loopSetup>
	<mapSetup>
		vec2 map = vec2(screenSize.xy/vec2(area.z-area.x,area.w-area.y));
#if Camera
		vec3 eye = vec3(position.x, -yRot.y, position.y);
		mat4 cam = getRotMatrix(yRot) * getPosMatrix(eye) * getRotMatrix(xRot);
#endif
	</mapSetup>,

	<colorSetup>
		vec3 color;
		if (colorWheel)
		{
			vec2 d = vec2((c.x-renderArea.x)/(renderArea.z-renderArea.x),1.0-(c.y-renderArea.y)/(renderArea.w-renderArea.y))*2-1; 
			float hue = (acos(d.x / length(d))*sign(d.y)+(PI_ONE_POINT_FIVE))/PI_TWO;
			color = vec3(hslToRgb(vec3(hue + colorOffset, 1.0, 0.5)));
		}
		else
		{
			color = 1/(colorIteration);
		}
	</colorSetup>,

	<inverseSetup>
		vec2 w = vec2(0);
		c/=dot(c,c);
	</inverseSetup>,
</loopSetup>

<loopReturn>
	<nothing>;</nothing>,
</loopReturn>


<EscapeCount>
bool insideBox(vec2 v, vec4 box)
{
    vec2 s = step(box.xy, v) - step(box.zw, v);
    return bool(s.x * s.y);   
}

vec2 EscapeCount(vec2 w, vec4 area)
{
	// Checking the point is within the largest parts of the set which do not escape (avoiding alot of computations, ~10x speedup)
	if (InMainCardioid(w) || InMainBulb(w))
	{
		return vec2(-1000, 0);
	}

	if (invalidSamples)
	{
		vec2 c = w;
		vec2 oldW = w;

		int stepsTaken = 0;
		int stepLimit = 2;

		for (int i = 0; i < maxIterations; i++)
		{
			<loopBody>

			if (dot(w,w) > escapeRadius || w == oldW)
			{
				return vec2(i,1);
			}

			if (stepsTaken == stepLimit)
			{
				oldW = w;
				stepsTaken = 0;
				stepLimit *= 2;
			}

			stepsTaken++;
		}

		return vec2(-1000, 0);
	}
	else
	{
		vec2 c = w;

		int insideCount = 0;

		for (int i = 0; i < maxIterations; i++)
		{
			<loopBody>

			if (insideBox(w,area))
			{
				insideCount++;
			}

			if (dot(w,w)>escapeRadius) return vec2(i,insideCount);
		}

		return vec2(-1000, 0);
	}
}
</EscapeCount>

<hslToRgb>
vec3 hslToRgb(vec3 c)
{
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0,1.0);
    return c.z + c.y * (rgb-0.5)*(1.0-abs(2.0*c.z-1.0));
}
</hslToRgb>

<getStartValue>
mat4 getRotMatrix(vec3 a)
{
	vec3 s = sin(a);
	vec3 c = cos(a);    
	mat4 ret;
	ret[0] = vec4(c.y*c.z,c.y*s.z,-s.y,0.0);
	ret[1] = vec4(s.x*s.y*c.z-c.x*s.z, s.x*s.y*s.z+c.x*c.z, s.x*c.y, 0.0);
	ret[2] = vec4(c.x*s.y*c.z+s.x*s.z, c.x*s.y*s.z-s.x*c.z, c.x*c.y, 0.0);    
	ret[3] = vec4(0.0,0.0,0.0,1.0);
	return ret;
}

mat4 getPosMatrix(vec3 p)
{
	mat4 ret;
	ret[0] = vec4(1.0,0.0,0.0,p.x);
	ret[1] = vec4(0.0,1.0,0.0,p.y);
	ret[2] = vec4(0.0,0.0,1.0,p.z);   
	ret[3] = vec4(0.0,0.0,0.0,1.0);
	return ret;
}

vec2 mutate(vec2 prev, inout uint hash, bool stepMutation)
{
	if (stepMutation)
	{
		// Generate a step with exponential distribution
		vec2 step = hash2(hash, hash) * vec2(1, PI_TWO);
		step = sqrt(step.x) * vec2(cos(step.y), sin(step.y));
		
		float s = length(step);
		step *= log(s) / s;
		return prev.xy+step*mutationSize; // Return a point we already know is good with a small mutation
	}
	else
	{
		// Generate a random point
		vec2 random = hash2(hash,hash);
		// Map it from [0,1) to fractal space
		return map01ToInterval(random, renderArea);
	}
}

vec2 getStartValue(int seed, vec4 area)
{
	uint index = uint(gl_GlobalInvocationID.y*screenSize.x+gl_GlobalInvocationID.x); // Accessing desirability like a 2d array
	vec4 prev = desirability[index];

	float c = abs(fract(sin(seed++)*62758.5453123)); // Do a random choice based on the seed
	int pointAttempts = int((c < mutationChance) ? mutationAttemps : startPointAttempts);
	bool stepMutation = c < mutationChance;

	uint hash = uint(seed);

	for(int i = 0; i < pointAttempts; ++i)
	{
		vec2 point = mutate(prev.xy, hash, stepMutation);

		vec2 escapeCount = EscapeCount(point, area);

		// We only want to iterate points that are interesting enough
		if (escapeCount.x > minIterations)
		{
			if (escapeCount.x * escapeCount.x * escapeCount.y > prev.z * prev.z * prev.w)
			{
				desirability[index] = vec4(point, escapeCount);
			}
			
			return point;
		}
	}

	// Signal that no appropriate point was found
	return vec2(-1000);
}
</getStartValue>

<map01ToInterval>
vec2 map01ToInterval(vec2 value, vec4 range)
{
	return vec2(value.x*(range.z-range.x)+range.x, value.y*(range.w-range.y)+range.y);
}
</map01ToInterval>


<mainLoop>
	void mainLoop(vec2 c, vec4 area)
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
