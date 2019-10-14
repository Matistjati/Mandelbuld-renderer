// Mandelbrot and julia 0
{
	<include>
		Mandelbrot
	</include>
	<uniforms>
		,

	</uniforms>
	<cpuVariables>
		<power>2</power>
	</cpuVariables>
	<mainLoop>
		<loopSetup>[[defaultSetup], [juliaSetupMouse], [inverseSetup], [cSetup], [inverseSetupEye]]</loopSetup>,
		<loopBody>[[complexPow(power), translate], [complexPow(power), translate, complexTan(w)], [complexPow(power), complexTan(w)], [complexPow(power), translate, complexSin(w)],
				   [tricornIter], [celticIter], [complexPow(power), setW(w.xy+w.x), translate],]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>[[escapeColorPeriodicCosSmooth(colorPeriodicity, vec3(0.7, 0.5, 0.1), vec3(0.2, 0.9, 0.9))], [escapeColor], [escapeColorLast(colorPeriodicity)], 
					 [escapeColorPeriodicCos(colorPeriodicity, vec3(0.7, 0.5, 0.1), vec3(0.2, 0.9, 0.9))]]</loopReturn>,
	</mainLoop>
}

// Exp-fractal 1
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[setW(exp(w.x*power)*vec2(cos(w.y*power)*power,sin(w.y*power)*power)), translate]]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>[[escapeColorPeriodicCosSmooth(colorPeriodicity, vec3(0.7, 0.5, 0.1), vec3(0.2, 0.9, 0.9))]]</loopReturn>,
	</mainLoop>
	<cpuVariables>
		<power>1.21</power>
	</cpuVariables>
	<variables>
		<escapeRadius>1000.</escapeRadius>,
	</variables>
}