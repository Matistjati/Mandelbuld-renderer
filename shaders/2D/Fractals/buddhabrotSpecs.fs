// Normal
{

	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[complexSquare, translate], [complexSquare, translate, lineFold(vec2(cos(time), sin(time)))], [complexSquare, translate, complexTan(w)]]</loopBody>,
		<loopTrap>addIfWithinMinMax</loopTrap>,
		<loopReturn>steppedCount</loopReturn>,
		<loopSetup>countSetup</loopSetup>,
		<loopBreakCondition>distanceBreakReturnCount</loopBreakCondition>,
	</mainLoop>
}