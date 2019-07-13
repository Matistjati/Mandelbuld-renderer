<complexSquare>
vec2 complexSquare(vec2 z)
{
	return mat2(z, -z.y, z.x) * z;
}
</complexSquare>

<iterationColorRed>
vec3 iterationColorRed(float iterations)
{
	vec4 localColor = vec4(0.0, 0.0, 0.0, 1.0);

	if (iterations >= maxIterations)
	{
		return vec4(0, 0, 0, 1.0);
	}
	else
	{
		float c = 3 * log(iterations) / log(maxIterations);

		return vec3(clamp(c, 0, 1), clamp(c - 1, 0, 1), clamp(c - 2, 0, 1));
	}
}
</iterationColorRed>