// Normal
{
	<include>
		Mandelbrot
	</include>
	<mainLoop>
		<loopSetup>[[defaultSetup, mapSetup, colorSetup], [inverseSetup, mapSetup, colorSetup], [juliaSetupMouse, mapSetup, colorSetup],]</loopSetup>,
		<loopBody>[[multibrotIter], [multibrotIter, complexTan(w)], [tricornIter], [firstMandelbrotIter, notFirstComplexTan(w)], [multibrotIter, complexSin(w)],
				   [firstMandelbrotIter, notFirstComplexSin(w)], [multibrotIter, lineFold(vec2(cos(time), sin(time)))], [celticIter], [heartIter], ]</loopBody>,
		<loopTrap>incrementWPosition</loopTrap>,
		<loopReturn>nothing</loopReturn>,
		<loopBreakCondition>defaultBreak</loopBreakCondition>,
	</mainLoop>
}