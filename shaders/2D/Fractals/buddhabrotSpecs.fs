// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[mandelbrotIter], [mandelbrotIter, complexTan(w)], [tricornIter], [firstMandelbrotIter, notFirstComplexTan(w)], [mandelbrotIter, complexSin(w)],
				   [firstMandelbrotIter, notFirstComplexSin(w)], [mandelbrotIter, lineFold(vec2(cos(time), sin(time)))], [celticIter], [heartIter], ]</loopBody>,
		<loopTrap>incrementWPosition</loopTrap>,
		<loopReturn>nothing</loopReturn>,
		<loopSetup>buddhaMapSetup</loopSetup>,
		<loopBreakCondition>defaultBreak</loopBreakCondition>,
	</mainLoop>
}