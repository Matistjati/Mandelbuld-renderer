// Mandelbrot and julia 0
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopSetup>[[defaultSetup], [juliaSetupMouse], [juliaSetupPosition]]</loopSetup>,
		<loopBody>[[complexPow(power), translate], [complexPow(power), translate, complexTan(w)], [complexPow(power), complexTan(w)], [complexPow(power), translate, complexSin(w)],
				   [tricornIter], [celticIter],]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>[[escapeColorPeriodicCosSmooth(0.05, vec3(0.7, 0.5, 0.1), vec3(0.2, 0.9, 0.9))], [escapeColor], [escapeColorLast], 
					 [escapeColorPeriodicCos(0.05, vec3(0.7, 0.5, 0.1), vec3(0.2, 0.9, 0.9))]]</loopReturn>,
	</mainLoop>
}

// Exp-fractal 1
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[setW(exp(w.x*power)*vec2(cos(w.y*power)*power,sin(w.y*power)*power))]]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>[[escapeColorLast]]</loopReturn>,
	</mainLoop>
	<variables>
		<escapeRadius>1000.</escapeRadius>,
	</variables>
}