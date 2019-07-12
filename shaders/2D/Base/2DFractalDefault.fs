<escapeRadius>4</escapeRadius>
<maxIterations>256</maxIterations>
<maxIterationsRelease>512</maxIterationsRelease>
<antiAliasing>2<antiAliasing>

<loopSetup>
	<defaultSetup>vec2 c = w;</defaultSetup>,
</loopSetup>

<iterateReturn>
	<escapeColor>return iterationColorRed(float(i)/float(maxIterations))</escapeColor>,
</iterateReturn>

<loopTrap>
	<none></none>,
</loopTrap>

<loopBreakCondition>
	<none></none>,
	<defaultBreak>if (dot(w,w) > <escapeRadius>) break;</defaultBreak>,
</loopBreakCondition>

<iterate>
	vec3 iterate(vec2 w)
	{
		<loopSetup>

		int i = 0;
		for (; i < maxIterations; i++)
		{
			<loopBody>

			<loopTrap>

			<loopBreakCondition>
		}

		return <iterateReturn>
	}
</iterate>

<main>
    vec2 c = (2*gl_FragCoord.xy-screenSize)/screenSize.y * zoom + position;

	vec3 col = iterate(c);

    color = vec4(col.xyz, 1.0);
</main>

<mainAA>
	vec3 col = vec3(0.0);
	for (int i = 0; i < antiAliasing; i++)
	{
		for (int j = 0; j < antiAliasing; j++)
		{
			vec2 c = ((2*gl_FragCoord.xy-screenSize)/screenSize.y + vec2(float(i)/antiAliasing, float(j)/antiAliasing)) * zoom + position;

			vec3 col += iterate(c);
		}
	}
	col /= float(antiAliasing*antiAliasing);

    color = vec4(col.xyz, 1.0);
</mainAA>