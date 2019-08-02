// Normal
{

	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[complexSquare, translate], [complexSquare, translate, lineFold(vec2(cos(time), sin(time)))]]</loopBody>,
		<loopTrap>addIfWithinMinMax</loopTrap>,
		<loopReturn>steppedCount</loopReturn>,
		<loopSetup>countSetup</loopSetup>,
		<loopBreakCondition>distanceBreakReturnCount</loopBreakCondition>,
	</mainLoop>
}