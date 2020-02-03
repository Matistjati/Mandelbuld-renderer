// Mandelbrot and julia 0
{
	<include>
		Mandelbrot
	</include>
	<uniforms>

	</uniforms>
	<cpuVariables>
		<power>2</power>
	</cpuVariables>
	<mainLoop>
		<loopSetup>[[defaultSetup], [juliaSetupMouse], [inverseSetup], [cSetup], [inverseSetupEye], [flowerSetup, defaultSetup], [inverseSetupAnimation], [inverseSetupAlt]]</loopSetup>,
		<loopBody>[[complexPow(power), translate], [complexPow(power), translate, complexTan(w)], [complexPow(power), complexTan(w)], [complexPow(power), translate, complexSin(w)],
				   [tricornIter], [celticIter], [complexPow(power), setW(w.xy+w.x), translate], [setW(complexPow(w,2)+0.19*complexPow(w,3)+c)]]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>[[escapeColorPeriodicCosSmooth(colorPeriodicity, colorA, colorB)], [escapeColor], [escapeColorLast(colorPeriodicity)], 
					 [escapeColorPeriodicCos(colorPeriodicity, colorA, colorB)]]</loopReturn>,
	</mainLoop>
}

// Exp-fractal 1
{
	<tip>"Use escapeColorLast and a negative power to find a mandelbrot set"</tip>
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[setW(mat2(w,-w.y,w.x)*power*exp(w.x*power)*vec2(cos(w.y*power),sin(w.y*power))), translate]]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>[[escapeColorPeriodicCosSmooth(colorPeriodicity, colorA, colorB)], [escapeColorLast(colorPeriodicity)]]</loopReturn>,
	</mainLoop>
	<cpuVariables>
		<power>1.21</power>
	</cpuVariables>
	<variables>
		<escapeRadius>1000.</escapeRadius>,
	</variables>
}