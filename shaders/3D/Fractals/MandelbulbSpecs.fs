// Exterior renderings 0
{
	<include>
		MandelBulb
	</include>

	<distanceEstimator>
		<distanceBody>[[triplexPow, translate], [triplexPow, translate, xMulSinX], [triplexPow, translate, sinX]]</distanceBody>,
	</distanceEstimator>

	<flags>
		<traceDefault>
	</flags>
}

// Interior renderings 1
{
	<include>
		MandelBulb
	</include>

	<distanceEstimator>
		<distanceBody>[[triplexPow, translate], [triplexPow, translate, mulYYXYYX]]</distanceBody>,
	</distanceEstimator>

	<cpuVariables>
		<position>[(0)[-0.301680839, 0, 0.435530365]]</position>,
	</cpuVariables>

	<flags>
		<traceDefault>
	</flags>
}

// Black region edge rendering 2
{
	<include>
		MandelBulb
	</include>

	<distanceEstimator>
		<distanceBody>[triplexPow, translate]</distanceBody>,
		<distanceSetup>[defaultSetup]</distanceSetup>,
	</distanceEstimator>

	<variables>
		<maxIterations>8</maxIterations>, // High maxiteration for interior rendering
		<maxSteps>60</maxSteps>,
		<maxIterationsRelease>8</maxIterationsRelease>,
		<maxStepsRelease>60</maxStepsRelease>,
		<antiAliasing>1</antiAliasing>,
	</variables>

	<flags>
		<traceDefault>
	</flags>

	<cpuVariables>
		<position>[(0)[0.847144186, 0.553977728, -0.392021567]]</position>,
	</cpuVariables>
}


// Interior/weird surface renderings 3
{
	<include>
		MandelBulb
	</include>
	<distanceEstimator>
		<distanceBody>[[triplexPow, translate], [triplexPow, translate, subXfromY], [triplexPow, translate, tanY], [triplexPow, translate, xAtanYX],
					   [triplexPow, translate, MulXY],]</distanceBody>,
		<distanceSetup>[defaultSetup]</distanceSetup>,
	</distanceEstimator>
	<flags>
		<traceDefault>,
		<edgeGlowOff>,
	</flags>

	<cpuVariables>
		<position>[(0)[-0.301680839, 0, 0.435530365]]</position>,
	</cpuVariables>

	<indices>
		<color>1</color>,
		<colorB>1</colorB>,
		<colorC>1</colorC>,
	</indices>
}