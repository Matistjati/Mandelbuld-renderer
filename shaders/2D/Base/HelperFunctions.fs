<complexSquare>
vec2 complexSquare(vec2 z)
{
	return mat2(z, -z.y, z.x) * z;
}
</complexSquare>

<iterationColorRed>
vec3 iterationColorRed(float iterations)
{
	float c = 3 * log(iterations) / log(maxIterations)-clamp(1-(zoom),0,1);

																		// Black interior- multiplication by 0- thus black if iterations==maxIterations
	return vec3(clamp(c, 0, 1), clamp(c - 1, 0, 1), clamp(c - 2, 0, 1))*sign(1-iterations/maxIterations);
}
</iterationColorRed>