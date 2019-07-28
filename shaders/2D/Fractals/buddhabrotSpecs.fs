// Normal
{

	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[complexSquare, translate]]</loopBody>,
		<loopTrap>addIfWithinMinMax</loopTrap>,
		<loopReturn>steppedCount</loopReturn>,
		<loopSetup>countSetup</loopSetup>,
		<loopBreakCondition>distanceBreakReturnCount</loopBreakCondition>,
	</mainLoop>
}