// Exterior renderings 0
{
	<include>
		MandelBulb
	</include>

	<distanceEstimator>
		<distanceBody>[[triplexPow, translate], [triplexPow, translate, xMulSinX], [triplexPow, translate, sinX],
					   [triplexPow, translate, mulW(1.9)]]</distanceBody>,
	</distanceEstimator>
	
	<cpuVariables>
		<position>[1.69295061, 0.386302084, -0.380642384]</position>,
	</cpuVariables>
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
		<maxIterations>200</maxIterations>, // High maxiteration for interior rendering
		<maxSteps>60</maxSteps>,
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
					   [triplexPow, translate, MulXY], [triplexPow, translate, mulW(w), /*Go to the middle*/]]</distanceBody>,
		<distanceSetup>[defaultSetup]</distanceSetup>,
	</distanceEstimator>
	<flags>
		<traceDefault>,
		<edgeGlowOff>,
	</flags>

	<cpuVariables>
		<position>[(0)[-0.301680839, 0, 0.435530365]]</position>,
	</cpuVariables>

	<variables>
		<colorA>vec3(0.7,0.7,0.9)</colorA>,
		<colorB>vec3(0.6,0.6,0.9)</colorB>,
		<colorC>vec3(0.7,0.7,1)</colorC>,
	</variables>
}