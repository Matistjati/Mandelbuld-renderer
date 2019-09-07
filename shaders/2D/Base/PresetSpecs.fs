<Mandelbrot>
{
	<mainLoop>
		<loopTrap>none</loopTrap>,
		<loopSetup>[defaultSetup]</loopSetup>,
		<loopBreakCondition>[defaultBreak]</loopBreakCondition>,
		<loopExtraOperations>[[], [lineFold(vec2(cos(time), sin(time)))], [translateRotated(time)],]</loopExtraOperations>,
	</mainLoop>
}
</Mandelbrot>