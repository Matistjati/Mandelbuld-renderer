<include>
	iterationColorRed, complexSquare, complexPow, complexTan, periodicLastPositionColor, complexSin, complexCos, escapeColorPeriodic
</include>

<uniforms>
	/*<GuiHint>GuiType: slider, Name: Power, Parent: fractalParams, Range: (-2, 8)</GuiHint>*/
	uniform float power = 2;

	/*<GuiHint>GuiType: slider, Name: Color change rate, Parent: color, Range: (0.01, 2)</GuiHint>*/
	uniform float colorPeriodicity = 0.1;

	/*<GuiHint>GuiType: colorPicker, Name: Color A, Parent: color</GuiHint>*/
	uniform vec3 colorA = vec3(0.44, 0, 1);

	/*<GuiHint>GuiType: colorPicker, Name: Color B, Parent: color</GuiHint>*/
	uniform vec3 colorB = vec3(0.2, 0.9, 0.9);

	/*<GuiHint>GuiType: slider, Name: Anti Aliasing, Parent: renderParams, Range: (1, 5)</GuiHint>*/
	uniform float antiAliasing = 1;

	/*<GuiHint>GuiType: checkBox, Name: Min anti aliasing, Parent: renderParams</GuiHint>*/
	uniform bool antiAliasingMin = false;
	
	/*<GuiHint>GuiType: checkBox, Name: Bad anti aliasing, Parent: renderParams</GuiHint>*/
	uniform bool stupidAntiAliasing = false;
</uniforms>


<mainLoop>
	vec3 mainLoop(vec2 c, out float iterations<extraParameters>)
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

		// Normalize iterations between 0 and 1
		iterations = i/maxIterations;

		// Reduce error for smooth coloring
		w = mat2(w,-w.y,w.x)*w+c; i++;
		w = mat2(w,-w.y,w.x)*w+c; i++;
		w = mat2(w,-w.y,w.x)*w+c; i++;

		return <loopReturn>
	}
</mainLoop>


<loopReturn>
	<escapeColorLast>periodicLastPositionColor(w, parameter);</escapeColorLast>,
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