// Normal 0
{
	<include>
		MandelBox
	</include>
	<distanceEstimator>
		<distanceBody>[[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative], 
		[boxFold, sphereFold, sinSinhX, scaleAndTranslate, mandelBoxDerivative], 
		[addLength, boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative, addLength],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>
	<indices>
		<color>2</color>
	</indices>

	<cpuVariables>
		<position>[7, 0, 0]</position>,
	</cpuVariables>
}

// Folding 1
{
	<include>
		MandelBox
	</include>
	<distanceEstimator>
		<distanceBody>[
		[planeFold(sun), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		[planeFold(sun*-1), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		[planeFold(vec3(0.577)), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		[planeFold(vec3(0.461, 0.877, 0.854)), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>
	<indices>
		<color>2</color>
	</indices>

	<cpuVariables>
		<position>[7, 0, 0]</position>,
	</cpuVariables>
}

// Iteration based fog 2
{
	<include>
		MandelBox, FogIteration
	</include>

	<distanceEstimator>
		<distanceBody>[[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative], [boxFold, sphereFold, boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative]]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>
	<indices>
		<color>0</color>
	</indices>
	<cpuVariables>
		<position>[-1.01426327, -0.817652643, -3.69670177]</position>,
	</cpuVariables>
}

// Bulb forest 3
{
	<include>
		MandelBox
	</include>
	<distanceEstimator>
		<distanceBody>[boxFold, sphereFold, boxFold, sphereFold, sinY, triplexPow, scaleAndTranslate, mandelBoxDerivative]</distanceBody>,
		<distanceTrap>[bulbForest]</distanceTrap>,
	</distanceEstimator>

	<indices>
		<color>2</color>
	</indices>
	<variables>
		<zoomDetailRatio>.001</zoomDetailRatio>,
		<maxIterationsRelease>8</maxIterationsRelease>,
		<maxStepsRelease>1000</maxStepsRelease>,
	</variables>
	<cpuVariables>
		<position>[(0)[-1.40337825, 4.03520060, -0.485743016], [0.987792492, 1.87121308, 0.991404295]]</position>,
	</cpuVariables>
}

// Sphere inversion hell 4
{
	<include>
		MandelBox
	</include>
	<distanceEstimator>
		<distanceBody>[boxFold, sphereFold, cosZsinY, scaleAndTranslate, mandelBoxDerivative]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>
	<indices>
		<color>3</color>
	</indices>
	<flags>
		<edgeGlowOff>
	</flags>
	//power - distance(w, sun);
	//power + distance(w, sun);
	<variables>
		<innerRadius>power-1</innerRadius>, 
		<outerRadius>power+3</outerRadius>, 
		<scale>genericParameter</scale>,
		<foldingLimit>power</foldingLimit>,
		<zoomDetailRatio>.001</zoomDetailRatio>,
		<maxIterations>4</maxIterations>,
		<maxIterationsRelease>4</maxIterationsRelease>
		<genericParameter>0.363999933</genericParameter>,
	</variables>
	<cpuVariables>
		<position>[(0)[1.97544038, 1.99667835, 1.43940282], [-0.974210978, -0.814095318, -3.66229343]]</position>,
		<genericParameter>2</genericParameter>,
	</cpuVariables>
}
// Weird floaters 5
{
	<tip>
		"Increase genericParameter and decrease power"
	</tip>
	<include>
		MandelBox
	</include>
	<distanceEstimator>
		<distanceBody>[boxFold, sphereFold, cosZsinY, scaleAndTranslate, mandelBoxDerivative]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>
	<indices>
		<color>1</color>
	</indices>
	<variables>
		<innerRadius>power-1</innerRadius>,
		<outerRadius>power+1</outerRadius>,
		<scale>genericParameter</scale>,
		<foldingLimit>power</foldingLimit>,
		<zoomDetailRatio>.001</zoomDetailRatio>,
		<maxIterations>4</maxIterations>,
		<maxIterationsRelease>4</maxIterationsRelease>
	</variables>
	<cpuVariables>
		<position>[(1)[1.97544038, 1.99667835, 1.43940282], [-0.974210978, -0.814095318, -3.66229343]]</position>,
		<genericParameter>0.363999933</genericParameter>,
		<power>1</power>,
	</cpuVariables>
}

// Curious infinity 6
{
	<include>
		MandelBox
	</include>
	<distanceEstimator>
		<distanceBody>[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>
	<variables>
		<innerRadius>0.1</innerRadius>,
		<outerRadius>1.</outerRadius>,
		<scale>1.</scale>,
		<foldingLimit>1.</foldingLimit>,
	</variables>
	<cpuVariables>
		<position>[0.1, 0., 0.]</position>,
	</cpuVariables>
	<indices>
		<color>1</color>
	</indices>
}
