// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[complexPow(power), translate], [complexPow(power), translate, lineFold(vec2(cos(time), sin(time)))]]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>escapeColor</loopReturn>,
	</mainLoop>
}