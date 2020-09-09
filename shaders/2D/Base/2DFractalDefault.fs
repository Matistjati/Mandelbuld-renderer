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
	<multibrotIter>w=complexPow(w,power)+c;</multibrotIter>,
	<tricornIter>w=vec2(w.x*w.x-w.y*w.y,-2*w.x*w.y)+c;</tricornIter>,
	<celticIter>w=vec2(abs(w.x*w.x-w.y*w.y),2*w.x*w.y)+c;</celticIter>,
	<heartIter>w=vec2(w.x*w.x-w.y*w.y,2*w.x*w.y)+c;</heartIter>,
	<buffaloIter>w=vec2(abs(w.x*w.x-w.y*w.y),-2*abs(w.x*w.y))+c;</buffaloIter>,
	<setW>w=parameter;</setW>,
	<saveW>vec2 W = w; w = vec2(0);</saveW>,
	<addTerm>w += parameter*complexPoW(oldW, parameter1);</addTerm>,
	<addTerm1>w += parameter*W;</addTerm1>,
	<addTerm2>w += parameter*mat2(W,-W.y,W.x)*W;</addTerm2>,
	<addTerm3>w += parameter*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*W;</addTerm3>,
	<addTerm4>w += parameter*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*W;</addTerm4>,
	<addTerm5>w += parameter*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*W;</addTerm5>,
	<addTerm6>w += parameter*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*W;</addTerm6>,
	<addTerm7>w += parameter*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*W;</addTerm7>,
	<addTerm8>w += parameter*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*mat2(W,-W.y,W.x)*W;</addTerm8>,

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
	<defaultBuddhaSetup>vec2 w = startValue;</defaultBuddhaSetup>,
	<juliaSetupMouse>vec2 w = c; c = clickPosition;</juliaSetupMouse>,
	<inverseSetup>vec2 w = vec2(0); c/=dot(c,c);</inverseSetup>,
	<inverseSetupAlt>vec2 w = c; c/=dot(c,c);</inverseSetupAlt>,
	<inverseSetupAnimation>vec2 w = vec2(0); c*=pow(dot(c,c),-pow(abs(sin(time)),2.));</inverseSetupAnimation>,
	<inverseSetupAnimationAlt>vec2 w = vec2(0); vec2 t=mix(c,mat2(c,-c.y,c.x)*c,sin(time));c=t/dot(c,c);</inverseSetupAnimation>,
	<inverseSetupEye>vec2 w = vec2(0); c = mat2(c,-c.y,c.x)*c; c/=dot(c,c);</inverseSetupEye>,
	<cSetup>vec2 w = c;</cSetup>,
	<flowerSetup>
		c = complexPow(c,-3);
		c = complexSin(c);
		c = complexTan(c);
	</flowerSetup>,
</loopSetup>

<loopReturn>
	<escapeColor>iterationColorRed(float(i));</escapeColor>,
	<escapeColorPeriodicCos>escapeColorPeriodic(i, parameter, parameter1, parameter2);</escapeColorPeriodicCos>,
	<escapeColorPeriodicCosSmooth>escapeColorPeriodic(i+1-log2(log2(dot(w,w)/log2(escapeRadius)))/log2(power), parameter, parameter1, parameter2);</escapeColorPeriodicCosSmooth>,
</loopReturn>

<loopTrap>
	<none></none>,
</loopTrap>

<loopBreakCondition>
	<none></none>,
	<defaultBreak>if (dot(w,w) > escapeRadius) break;</defaultBreak>,
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

		// Normalize iterations between 0 and 1
		iterations = i/maxIterations;

		// Reduce error for smooth coloring
		w = mat2(w,-w.y,w.x)*w+c; i++;
		w = mat2(w,-w.y,w.x)*w+c; i++;
		w = mat2(w,-w.y,w.x)*w+c; i++;

		return <loopReturn>
	}
</mainLoop>

<main>
	vec3 col = vec3(0.0);
	int AA = int(antiAliasing);

	// Anti aliasing by sampling subpixels and choosing the value with the least iteration count (reduce non-escaping pixels)
	if (antiAliasingMin)
	{
		float minIterations = 1e9;
		vec2 p = (2*gl_FragCoord.xy-screenSize)/screenSize.y;
		vec2 p2 = (2*(gl_FragCoord.xy+1)-screenSize)/screenSize.y;
		for (float i = 0; i < AA; i++)
		{
			vec2 c = mix(p, p2, i/AA) * zoom + position.xy;

			float iterations = 0;
			vec3 tempCol = mainLoop(c, iterations);
			col = (iterations < minIterations) ? tempCol : col;
			minIterations = min(minIterations, iterations);
		}
	}
	// Anti aliasing by taking the average color of subpixels
	else if (!stupidAntiAliasing)
	{
		vec2 p = (2*gl_FragCoord.xy-screenSize)/screenSize.y;
		vec2 p2 = (2*(gl_FragCoord.xy+1)-screenSize)/screenSize.y;
		for (float i = 0; i < AA; i++)
		{
			vec2 c = mix(p, p2, i/AA) * zoom + position.xy;

			float iterations = 0;
			col += mainLoop(c, iterations);
		}
		col /= AA;
	}
	else
	{
		float minIterations = 100;
		vec2 p = (2*gl_FragCoord.xy-screenSize)/screenSize.y;
		for (float i = 0; i < antiAliasing; i++)
		{
			for (float j = 0; j < antiAliasing; j++)
			{
				vec2 c = ((2*gl_FragCoord.xy-screenSize)/screenSize.y+(vec2(i,j)/antiAliasing)) * zoom + position.xy; 

				float iterations = 0;
				vec3 tempCol = mainLoop(c, iterations);
				col = (iterations < minIterations) ? tempCol : col;
				minIterations = min(minIterations, iterations);
			}
		}
	}

	color = vec4(col.xyz, 1.0);
</main>