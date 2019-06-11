{
	<include>
		MandelBulb
	</include>

	<distanceEstimator>
		<distanceBody>[triplexPow, translate]</distanceBody>,
	</distanceEstimator>

	<flags>
		<traceDefault>
	</flags>
}

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

{
	<include>
		MandelBulb
	</include>
	<distanceEstimator>
		<distanceBody>[triplexPow, translate, sinX]</distanceBody>,
	</distanceEstimator>
	<flags>
		<traceDefault>,
		<edgeGlowOff>,
	</flags>

	<variables>
		<maxDistRelease>30</maxDistRelease>,
		<antiAliasing>4</antiAliasing>,
	</variables>
}