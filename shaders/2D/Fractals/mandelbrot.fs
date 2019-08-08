<maxIterations>256</maxIterations>
<maxIterationsRelease>2048</maxIterationsRelease>

<color>
	<vec3(0.7, 0.1, 0.3)>,
</color>

<include>
	iterationColorRed, complexSquare, complexPow, complexTan, periodicLastPositionColor, complexSin, complexCos, escapeColorPeriodic
</include>

<loopReturn>
	<escapeColorLast>periodicLastPositionColor(w);</escapeColorLast>,
</loopReturn>

<iterationColorRed>
vec3 iterationColorRed(float iterations)
{
	float c = 3 * log(iterations) / log(maxIterations)+clamp(log((zoom)),0,1);

																		// Black interior- multiplication by 0- thus black if iterations==maxIterations
	return vec3(clamp(c, 0, 1), clamp(c - 1, 0, 1), clamp(c - 2, 0, 1))*(step(iterations,maxIterations-1));
}
</iterationColorRed>

<periodicLastPositionColor>
vec3 periodicLastPositionColor(vec2 w)
{
	return vec3(sin(w.x/length(w)),cos(w.y*dot(w,w)), cos(20*length(w)));
}
</periodicLastPositionColor>

<escapeColorPeriodic>
vec3 escapeColorPeriodic(float iterations, float frequency, vec3 colA, vec3 colB)
{
	return (cos(colA + colB * iterations * frequency) * -0.5 + 0.5) * (step(iterations, maxIterations - 1));
}
</escapeColorPeriodic>
