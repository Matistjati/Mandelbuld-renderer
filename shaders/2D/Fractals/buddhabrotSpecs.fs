// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopSetup>[[defaultBuddhaSetup, mapSetup, colorSetup], [inverseSetup, mapSetup, colorSetup], [complexSetup, mapSetup, colorSetup], [juliaSetupMouse, mapSetup, colorSetup], [inverseSetupAnimation, mapSetup, colorSetup], 
					[inverseSetupAlt, mapSetup, colorSetup], ]</loopSetup>,
		<loopBody>[[multibrotIter], [multibrotIter, complexTan(w)], [tricornIter], [firstMandelbrotIter, notFirstComplexTan(w)], [multibrotIter, complexSin(w)],
				   [firstMandelbrotIter, notFirstComplexSin(w)], [multibrotIter, lineFold(vec2(cos(time), sin(time)))], [celticIter], [heartIter], [setW(complexPow(w,2)+0.19*complexPow(w,3)+c)],
				   [multibrotIter, lineFold(vec2(cos(time), sin(time))), celticIter], [multibrotIter, complexSin(w), complexSin(w), celticIter], [heartIter, complexSin(w), complexSin(w), celticIter, complexTan(w)], 
				   ]</loopBody>,
		<loopTrap>incrementWPosition</loopTrap>,
		<loopReturn>nothing</loopReturn>,
		<loopBreakCondition>defaultBreak</loopBreakCondition>,
	</mainLoop>
}

// Polynomial
{
	<include>
		Mandelbrot
	</include>

	<uniforms>
		<
			/*<GuiHint>GuiType: slider, Name: Coefficients, Parent: renderParams, Range: (-10, 10)</GuiHint>*/
			uniform vec4 coefficientsA = vec4(0,1,0,0); 
		>,
		<
			/*<GuiHint>GuiType: slider, Name: Coefficients, Parent: renderParams, Range: (-10, 10)</GuiHint>*/
			uniform vec4 coefficientsB = vec4(0);
		>,
	</uniforms>
		

	<variables>
		<correctAlternate>true</correctAlternate>,
		<pow2Optimize>false</pow2Optimize>,
	</variables>


	<mainLoop>
		<loopSetup>[[defaultBuddhaSetup, mapSetup, colorSetup], [inverseSetup, mapSetup, colorSetup], [complexSetup, mapSetup, colorSetup], [juliaSetupMouse, mapSetup, colorSetup], [inverseSetupAnimation, mapSetup, colorSetup], 
					[inverseSetupAlt, mapSetup, colorSetup], ]</loopSetup>,
		<loopBody>[[fourDegreePoly(coefficientsA)],
				   [eightDegreePoly(coefficientsB, coefficientsA)],
				   ]</loopBody>,
		<loopTrap>incrementWPosition</loopTrap>,
		<loopReturn>nothing</loopReturn>,
		<loopBreakCondition>defaultBreak</loopBreakCondition>,
	</mainLoop>
}