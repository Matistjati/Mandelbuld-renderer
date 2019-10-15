// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopBody>[[multibrotIter], [multibrotIter, complexTan(w)], [tricornIter], [firstMandelbrotIter, notFirstComplexTan(w)], [multibrotIter, complexSin(w)],
				   [firstMandelbrotIter, notFirstComplexSin(w)], [multibrotIter, lineFold(vec2(cos(time), sin(time)))], [celticIter], [heartIter], ]</loopBody>,
		<loopTrap>incrementWPosition</loopTrap>,
		<loopReturn>nothing</loopReturn>,
		<loopSetup>[[buddhaMapSetup], [buddhaMapSetupInverse]]</loopSetup>,
		<loopBreakCondition>defaultBreak</loopBreakCondition>,
	</mainLoop>
}