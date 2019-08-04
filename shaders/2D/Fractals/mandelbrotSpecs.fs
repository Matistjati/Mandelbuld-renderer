// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[complexPow(power), translate], [complexPow(power), translate, lineFold(vec2(cos(time), sin(time)))], [complexPow(power), translateRotated(time)], 
				   [complexPow(power), translate, complexTan(w)], [complexPow(power), complexTan(w)], 
				   [complexPow(power), translateRotated(time), lineFold(vec2(cos(time), sin(time))), complexTan(w)],]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>escapeColor</loopReturn>,
	</mainLoop>
}