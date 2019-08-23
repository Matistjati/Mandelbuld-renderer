// Normal
{

	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[mandelbrotIter], [mandelbrotIter, complexTan(w)], [complexTan(w)], [mandelbrotIter, complexSin(w)], [complexSin(w)], [mandelbrotIter, lineFold(vec2(cos(time), sin(time)))]]</loopBody>,
		<loopTrap>incrementWPosition</loopTrap>,
		<loopReturn>nothing</loopReturn>,
		<loopSetup>defaultSetup</loopSetup>,
		<loopBreakCondition>defaultBreak</loopBreakCondition>,
	</mainLoop>
}