<escapeRadius>4</escapeRadius>
<maxIterations>256</maxIterations>
<maxIterationsRelease>512</maxIterationsRelease>
<antiAliasing>2</antiAliasing>

<operations>
	<complexSquare>w = complexSquare(w);</complexSquare>,
	<translate>w += c;</translate>,
</operations>

<loopSetup>
	<defaultSetup>vec2 c = w;</defaultSetup>,
</loopSetup>

<iterateReturn>
	<escapeColor>iterationColorRed(float(i));</escapeColor>,
</iterateReturn>

<loopTrap>
	<none></none>,
</loopTrap>

<loopBreakCondition>
	<none></none>,
	<defaultBreak>if (dot(w,w) > <escapeRadius>) break;</defaultBreak>,
</loopBreakCondition>

<mainLoop>
	vec3 mainLoop(vec2 w)
	{
		<loopSetup>

		int i = 0;
		for (; i < maxIterations; i++)
		{
			<loopBody>

			<loopTrap>

			<loopBreakCondition>
		}

		return <loopReturn>
	}
</mainLoop>

<main>
    vec2 c = (2*gl_FragCoord.xy-screenSize)/screenSize.y * zoom + position;

	vec3 col = mainLoop(c);

    color = vec4(col.xyz, 1.0);
</main>

<mainAA>
	vec3 col = vec3(0.0);
	for (int i = 0; i < antiAliasing; i++)
	{
		for (int j = 0; j < antiAliasing; j++)
		{
			vec2 c = ((2*gl_FragCoord.xy-screenSize)/screenSize.y + .1 * (vec2(float(i)/antiAliasing), float(j)/antiAliasing)) * zoom + position;

			col += mainLoop(c);
		}
	}
	col /= float(antiAliasing*antiAliasing);

    color = vec4(col.xyz, 1.0);
</mainAA>