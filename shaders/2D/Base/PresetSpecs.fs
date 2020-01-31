<2D>
{
	<shaderParameters>
		{name: "loopReturn", key: "CTRL A Q"},
		{name: "loopExtraOperations", key: "CTRL S W"},
		{name: "loopSetup", key: "CTRL D E"},
	</shaderParameters>
}
</2D>

<Mandelbrot>
{
	<include>
		2D
	</include>

	<mainLoop>
		<loopTrap>none</loopTrap>,
		<loopSetup>[defaultSetup]</loopSetup>,
		<loopBreakCondition>[defaultBreak]</loopBreakCondition>,
		<loopExtraOperations>[[], [lineFold(vec2(cos(time), sin(time)))], [translateRotated(time)],]</loopExtraOperations>,
	</mainLoop>
}
</Mandelbrot>