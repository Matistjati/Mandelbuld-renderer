#version 430

//How many orbits should be drawn per frame? 
//Setting this too high on low-end hardware will cause the browser to become sluggish,
//but setting it so low that the shader runs at the 60 fps cap is a waste of time.
#define orbitsPerFrame 100

//How many iterations per orbit the code should do. Setting this higher will lead
//to more pronounced features, but rendering will take longer.
//See https://en.wikipedia.org/wiki/Buddhabrot for (post-processed...) example images.
const float stepsPerOrbit1 = 1000.;
const float stepsPerOrbit2 = 500.;
const float stepsPerOrbit3 = 250.;

#define startPointAttempts 20
#define startPointAttemptsIter 500


uniform vec2 screenSize = vec2(500, 500);
uniform int time;
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 0) buffer densityMap
{
	vec4 points[];
};

uint intHash(uint x) {
    x = ((x >> 16) ^ x) * 0x45d9f3bU;
    x = ((x >> 16) ^ x) * 0x45d9f3bU;
    x = (x >> 16) ^ x;
    return x;
}
vec2 hash2(uint n, out uint hash)
{
    uint ih =intHash(n);
    hash = intHash(ih);
    uvec2 k = uvec2(ih,hash);
    return vec2(k & uvec2(0xffffffffU))/float(0xffffffffU);
}

bool notInMainCardioid(vec2 z)
{
	vec2 c=z-vec2(0.25,0);
	float q = dot(c,c);
	return q*(q+(z.x-0.25))>0.25*z.y*z.y;
}


bool notInMainBulb(vec2 z)
{
    z += vec2(1,0);
    return bool(step(0.062499999,dot(z,z)));
}

vec3 mandel(vec2 coords, vec2 minVal, vec2 maxVal)
{
    vec3 cnt = vec3(0.0);
    vec2 w = coords;
    for(int i=0;i < stepsPerOrbit1; ++i)
    {
        w = mat2(w,-w.y,w.x)*w+coords;
        vec2 wUpper = vec2(w.x,abs(w.y));

		vec2 stepped = step(minVal,wUpper)*step(wUpper,maxVal);
		//vec2 d = clamp(wUpper,minVal,maxVal)-wUpper;
		//float v = float(abs(d.x)<0.001&&abs(d.y)<0.001);
        //cnt += vec3(v) * vec3(1, step(i/stepsPerOrbit2,1.), step(i/stepsPerOrbit3,1.));
        cnt += vec3(stepped.x*stepped.y) * vec3(1, step(i,stepsPerOrbit2), step(i,stepsPerOrbit3));

        if(dot(w,w) > 20.0)
            return cnt;
    }
    return step(4.0,dot(w,w))*cnt;
}

bool pointEscapes(vec2 c)
{
	vec2 z = c;
	for (int i = 0; i<startPointAttemptsIter;i++)
	{
		z = mat2(z,-z.y,z.x)*z+c;
		if (dot(z,z)>4&&i>200)
		{
			return true;
		}
	}
	return false;
}

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


void main()
{
	uint fragCoord = gl_GlobalInvocationID.y*int(screenSize.x)+gl_GlobalInvocationID.x;

	vec2 uv = gl_GlobalInvocationID.xy/screenSize.xy;
    vec2 minVal = vec2(uv.x*3.5-2.5,abs(uv.y*2.0-1.0));
    vec2 maxVal = minVal + vec2(3.5,2.0)/screenSize.xy;
    
    
    vec3 sum = vec3(0.0);
    for(int idx=0;idx < orbitsPerFrame;++idx)
    {
        int seed = (time*orbitsPerFrame*2+idx*2+int(step(uv.y,0.5)));

    	vec2 pos = getStartValue(seed);
		if (pos.x<-1000) continue; // We didn't find a point
    	sum += mandel(pos,minVal,maxVal);
    }

    vec3 prev = points[fragCoord].rgb;
    points[fragCoord] = vec4(prev + sum,1);
}