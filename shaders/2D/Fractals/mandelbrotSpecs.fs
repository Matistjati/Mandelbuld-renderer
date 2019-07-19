// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[complexPow(power), translate]]</loopBody>,
		<loopTrap>none</loopTrap>,
		<loopReturn>escapeColor</loopReturn>,
	</mainLoop>
}