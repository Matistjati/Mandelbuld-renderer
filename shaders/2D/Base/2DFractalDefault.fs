<escapeRadius>4.</escapeRadius>
<maxIterations>256</maxIterations>
<maxIterationsRelease>2048</maxIterationsRelease>
<antiAliasing>9</antiAliasing>

<operations>
	<complexSquare>w = complexSquare(w);</complexSquare>,
	<complexPow>w = complexPow(w, parameter);</complexPow>,
	<abs>parameter = abs(parameter);</abs>,
	<translate>w += c;</translate>,
	<translateRotated>w+=c*mat2(cos(parameter),-sin(parameter),sin(parameter),cos(parameter));</translateRotated>,
	<lineFold>w-=2 * min(0, dot(w, parameter)) * parameter;</lineFold>,
	<complexTan>parameter = complexTan(parameter);</complexTan>,
	<complexSin>parameter = complexSin(parameter);</complexSin>,
	<complexCos>parameter = complexCos(parameter);</complexCos>,
	<mandelbrotIter>w=mat2(w,-w.y,w.x)*w+c;</mandelbrotIter>,
	<setW>w=parameter;</setW>,
</operations>

<loopSetup>
	<defaultSetup>vec2 c = w;</defaultSetup>,
	<juliaSetupPosition>vec2 c = (2*(screenSize.xy/2)-screenSize)/screenSize.y * zoom + position;</juliaSetupPosition>,
	<juliaSetupMouse>vec2 c = (2*(mousePosition)-screenSize)/screenSize.y * zoom + position;</juliaSetupMouse>,
</loopSetup>

<loopReturn>
	<escapeColor>iterationColorRed(float(i));</escapeColor>,
	<escapeColorPeriodicCos>escapeColorPeriodic(i, parameter, parameter1, parameter2);</escapeColorPeriodicCos>,
	<escapeColorPeriodicCosSmooth>escapeColorPeriodic(i+1-log(log(dot(w,w)))/log(power), parameter, parameter1, parameter2);</escapeColorPeriodicCosSmooth>,
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

		float i = 0;
		for (; i < maxIterations; i++)
		{
			<loopBody>

			<loopExtraOperations>

			<loopTrap>

			<loopBreakCondition>
		}


		iterations = i/maxIterations;

		// Reduce error for smooth coloring
		w = mat2(w,-w.y,w.x)*w+c; i++;
		w = mat2(w,-w.y,w.x)*w+c; i++;
		w = mat2(w,-w.y,w.x)*w+c; i++;

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
// Average
#if 1
	vec3 col = vec3(0.0);

	vec2 p = (2*gl_FragCoord.xy-screenSize)/screenSize.y;
	vec2 p2 = (2*(gl_FragCoord.xy+1)-screenSize)/screenSize.y;
	for (float i = 0; i < antiAliasing; i++)
	{
		vec2 c = mix(p, p2, i/antiAliasing) * zoom + position;

		float iterations = 0;
		col += mainLoop(c, iterations);

	}
	col /= antiAliasing;
    color = vec4(col.xyz, 1.0);

#else
// Minimum iteration count

	vec3 col = vec3(0.0);

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
#endif
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