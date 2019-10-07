<escapeRadius>8.</escapeRadius>
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
	<tricornIter>w=vec2(w.x*w.x-w.y*w.y,-2*w.x*w.y)+c;</tricornIter>,
	<celticIter>w=vec2(abs(w.x*w.x-w.y*w.y),2*w.x*w.y)+c;</celticIter>,
	<heartIter>w=vec2(w.x*w.x-w.y*w.y,2*w.x*w.y)+c;</heartIter>,
	<buffaloIter>w=vec2(abs(w.x*w.x-w.y*w.y),-2*abs(w.x*w.y))+c;</buffaloIter>,
	<setW>w=parameter;</setW>,

	// Header magic for certain buddhabrots
	<firstMandelbrotIter>
	#ifndef mandelbrotIter
	#define mandelbrotIter
	w = complexSquare(w)+c;
	#endif</firstMandelbrotIter>,

	<notFirstComplexSin>
	#if ComplexSinFirstPassed
		parameter = complexSin(parameter);
	#else
		#define ComplexSinFirstPassed 1
	#endif</notFirstComplexSin>,
	
	<notFirstComplexTan>
	#if ComplexTanFirstPassed
		parameter = complexTan(parameter);
	#else
		#define ComplexTanFirstPassed 1
	#endif
	</notFirstComplexTan>,
</operations>

<loopSetup>
	<defaultSetup>vec2 w = vec2(0);</defaultSetup>,
	<juliaSetupMouse>vec2 w = c; c = clickPositions.xy + clickPositions.zw;</juliaSetupMouse>,
	<inverseSetup>vec2 w = vec2(0); vec2 t=mix(c,mat2(c,-c.y,c.x)*c,sin(time));c=t/dot(c,c);</inverseSetup>,
	<inverseSetupEye>vec2 w = vec2(0); c = mat2(c,-c.y,c.x)*c; c/=dot(c,c);</inverseSetupEye>,
	<cSetup>vec2 w = c;</cSetup>,
</loopSetup>

<loopReturn>
	<escapeColor>iterationColorRed(float(i));</escapeColor>,
	<escapeColorPeriodicCos>escapeColorPeriodic(i, parameter, parameter1, parameter2);</escapeColorPeriodicCos>,
	<escapeColorPeriodicCosSmooth>escapeColorPeriodic(i+1-log2(log2(dot(w,w)/log2(<escapeRadius>)))/log2(power), parameter, parameter1, parameter2);</escapeColorPeriodicCosSmooth>,
</loopReturn>

<loopTrap>
	<none></none>,
</loopTrap>

<loopBreakCondition>
	<none></none>,
	<defaultBreak>if (dot(w,w) > <escapeRadius>) break;</defaultBreak>,
</loopBreakCondition>

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

	/*if (distance(c, (2*vec2(mousePosition.x,screenSize.y-mousePosition.y)-screenSize)/screenSize.y * zoom + position) < 0.1)
	{
		color = vec4(0);
		return;
	}*/


	float iterations;
	vec3 col = mainLoop(c, iterations);

    color = vec4(col.xyz, 1.0);
</main>

<mainAA>
// Average
#if 0
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

	float minIterations = 1e9;
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