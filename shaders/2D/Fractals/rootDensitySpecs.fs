// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopSetup>[[defaultBuddhaSetup]</loopSetup>,
		<loopBody>[[multibrotIter]]</loopBody>,
		<loopTrap></loopTrap>,
		<loopReturn>nothing</loopReturn>,
		<loopBreakCondition>defaultBreak</loopBreakCondition>,
	</mainLoop>
}