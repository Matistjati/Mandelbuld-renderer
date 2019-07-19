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