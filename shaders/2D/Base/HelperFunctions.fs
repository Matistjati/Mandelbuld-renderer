<complexSquare>
vec2 complexSquare(vec2 z)
{
	return mat2(z, -z.y, z.x) * z;
}
</complexSquare>

<complexPow>
vec2 complexPow(vec2 z, float power)
{
	float arg = atan(z.y,z.x) * power;
	return pow(length(z), power) * vec2(cos(arg), sin(arg));
}
</complexPow>

<iterationColorRed>
vec3 iterationColorRed(float iterations)
{
	float c = 3 * log(iterations) / log(maxIterations)-clamp(1-(zoom),0,1);

																		// Black interior- multiplication by 0- thus black if iterations==maxIterations
	return vec3(clamp(c, 0, 1), clamp(c - 1, 0, 1), clamp(c - 2, 0, 1))*sign(1-iterations/maxIterations);
}
</iterationColorRed>

<periodicLastPositionColor>
vec3 periodicLastPositionColor(vec2 w)
{
	return vec3(sin(w.x/length(w)),cos(w.y*dot(w,w)),  cos(20*length(w)));
}
</periodicLastPositionColor>

<intHash>
uint intHash(uint x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3bU;
    x = ((x >> 16) ^ x) * 0x45d9f3bU;
    x = (x >> 16) ^ x;
    return x;
}
</intHash>

<hash2>
vec2 hash2(uint n, out uint hash)
{
    uint ih =intHash(n);
    hash = intHash(ih);
    uvec2 k = uvec2(ih,hash);
    return vec2(k & uvec2(0xffffffffU))/float(0xffffffffU);
}
</hash2>

<notInMainCardioid>
/*reference: https://en.wikipedia.org/wiki/Mandelbrot_set#Optimizations */
bool notInMainCardioid(vec2 z)
{
	vec2 c=z-vec2(0.25,0);
	float q = dot(c,c);
	return q*(q+(z.x-0.25))>0.25*z.y*z.y;
}
</notInMainCardioid>

<notInMainBulb>
/*reference: https://en.wikipedia.org/wiki/Mandelbrot_set#Optimizations */
bool notInMainBulb(vec2 z)
{
    z += vec2(1,0);
    return bool(step(0.062499999,dot(z,z)));
}
</notInMainBulb>

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