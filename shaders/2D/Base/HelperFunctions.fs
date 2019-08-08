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

<complexTan>
vec2 complexTan(vec2 w)
{
	vec2 w2 = w * 2;

	float denominator = cos(w2.x)+cosh(w2.y);

	return vec2(sin(w2.x)/denominator, sinh(w2.y)/denominator);
}
</complexTan>

<complexSin>
vec2 complexSin(vec2 w)
{
	return vec2(sin(w.x)*cosh(w.y), cos(w.x)*sinh(w.y));
}
</complexSin>

<complexCos>
vec2 complexCos(vec2 w)
{
	return vec2(cos(w.x)*cosh(w.y), -sin(w.x)*sinh(w.y));
}
</complexCos>

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