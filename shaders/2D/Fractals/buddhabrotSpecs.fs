// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[mandelbrotIter], [mandelbrotIter, complexTan(w)], [trictornIter], [firstMandelbrotIter, notFirstComplexTan(w)], [mandelbrotIter, complexSin(w)], [firstMandelbrotIter, notFirstComplexSin(w)], [mandelbrotIter, lineFold(vec2(cos(time), sin(time)))]]</loopBody>,
		<loopTrap>incrementWPosition</loopTrap>,
		<loopReturn>nothing</loopReturn>,
		<loopSetup>buddhaMapSetup</loopSetup>,
		<loopBreakCondition>defaultBreak</loopBreakCondition>,
	</mainLoop>
}