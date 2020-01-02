<maxIterations>256</maxIterations>
<maxIterationsRelease>2048</maxIterationsRelease>

<color>
	<vec3(0.7, 0.1, 0.3)>,
</color>

<include>
	iterationColorRed, complexSquare, complexPow, complexTan, periodicLastPositionColor, complexSin, complexCos, escapeColorPeriodic
</include>

<loopReturn>
	<escapeColorLast>periodicLastPositionColor(w, parameter);</escapeColorLast>,
</loopReturn>

<loopReturn>
	<flowerSetup>
		c = complexPow(c,-3);
		c = complexSin(c);
		c = complexTan(c);
	</flowerSetup>,
</loopReturn>

<iterationColorRed>
vec3 iterationColorRed(float iterations)
{
	float c = 3 * log(iterations) / log(maxIterations)-clamp(exp(-zoom),0,1);

																		// Black interior- multiplication by 0- thus black if iterations==maxIterations
	return vec3(clamp(c, 0, 1), clamp(c - 1, 0, 1), clamp(c - 2, 0, 1))*(step(iterations,maxIterations-1));
}
</iterationColorRed>

<periodicLastPositionColor>
vec3 periodicLastPositionColor(vec2 w, float frequency)
{
	return vec3(sin(w.x/length(w)*frequency),cos(w.y*dot(w,w)*frequency), cos(20*length(w)*frequency));
}
</periodicLastPositionColor>

<escapeColorPeriodic>
vec3 escapeColorPeriodic(float iterations, float frequency, vec3 colA, vec3 colB)
{
	return (cos(colA + colB * iterations * frequency) * -0.5 + 0.5) * (step(iterations, maxIterations - 1));
}
</escapeColorPeriodic>
