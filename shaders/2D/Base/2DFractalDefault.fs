<escapeRadius>4</escapeRadius>
<maxIterations>256</maxIterations>
<maxIterationsRelease>2048</maxIterationsRelease>
<antiAliasing>9</antiAliasing>

<operations>
	<complexSquare>w = complexSquare(w);</complexSquare>,
	<complexPow>w = complexPow(w, parameter);</complexPow>,
	<translate>w += c;</translate>,
	<translateRotated>w+=c*mat2(cos(parameter),-sin(parameter),sin(parameter),cos(parameter));</translateRotated>,
	<lineFold>w-=2 * min(0, dot(w, parameter)) * parameter;</lineFold>,
</operations>

<loopSetup>
	<defaultSetup>vec2 c = w;</defaultSetup>,
</loopSetup>

<loopReturn>
	<escapeColor>iterationColorRed(float(i));</escapeColor>,
</loopReturn>

<loopTrap>
	<none></none>,
</loopTrap>

<loopBreakCondition>
	<none></none>,
	<defaultBreak>if (dot(w,w) > <escapeRadius>) break;</defaultBreak>,
</loopBreakCondition>

<mainLoop>
	vec3 mainLoop(vec2 w, out float iterations<extraParameters>)
	{
		<loopSetup>
		w*=mat2(cos(time),-sin(time),sin(time),cos(time));
		float i = 0;
		for (; i < maxIterations; i++)
		{
			<loopBody>

			<loopTrap>

			<loopBreakCondition>
		}

		iterations = i/maxIterations;

		return <loopReturn>
	}
</mainLoop>

<main>
    vec2 c = (2*gl_FragCoord.xy-screenSize)/screenSize.y * zoom + position;

	float iterations;
	vec3 col = mainLoop(c, iterations);

    color = vec4(col.xyz, 1.0);
</main>

<mainAA>
	vec3 col = vec3(0.0);
	float minIterations = 1e9; // Big starting value, assuring that iterations will always be less than this
	vec2 p = (2*gl_FragCoord.xy-screenSize)/screenSize.y;
	vec2 p2 = (2*(gl_FragCoord.xy+1)-screenSize)/screenSize.y;
	for (float i = 0; i < antiAliasing; i++)
	{
		vec2 c = mix(p, p2, i/antiAliasing) * zoom + position;

		float iterations = 0;
		vec3 tempCol = mainLoop(c, iterations);
		col = (iterations < minIterations) ? tempCol : col;
		minIterations = min(minIterations, iterations);
	}

    color = vec4(col.xyz, 1.0);
</mainAA>

/*
	vec3 col = vec3(0.0);
	float minIterations = 100;
	vec2 p = (2*gl_FragCoord.xy-screenSize)/screenSize.y;
	for (float i = 0; i < antiAliasing; i++)
	{
		for (float j = 0; j < antiAliasing; j++)
		{
		    vec2 c = ((2*gl_FragCoord.xy-screenSize)/screenSize.y+(vec2(i,j)/antiAliasing)) * zoom + position; 

			float iterations = 0;
			vec3 tempCol = mainLoop(c, iterations);
			col = (iterations < minIterations) ? tempCol : col;
			minIterations = min(minIterations, iterations);
		}
	}

    color = vec4(col.xyz, 1.0);
*/