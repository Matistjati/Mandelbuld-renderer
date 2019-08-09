// Normal 0
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[complexPow(power), translate], [complexPow(power), translate, lineFold(vec2(cos(time), sin(time)))], [complexPow(power), translateRotated(time)], 
				   [complexPow(power), translate, complexTan(w)], [complexPow(power), complexTan(w)], [complexPow(power), translate, complexSin(w)],
				   [complexPow(power), translateRotated(time), lineFold(vec2(cos(time), sin(time))), complexTan(w)],]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>[[escapeColorPeriodicCos(0.05, vec3(0.7, 0.5, 0.1), vec3(0.2, 0.9, 0.9))], [escapeColor]]</loopReturn>,
	</mainLoop>
}