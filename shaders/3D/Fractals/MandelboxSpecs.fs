// Normal 0
{
	<include>
		MandelBox
	</include>
	<distanceEstimator>
		<distanceBody>[(0)[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative], 
		[boxFold, sphereFold, sinSinhX, scaleAndTranslate, mandelBoxDerivative], // Try lower maxiteration
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>
	<indices>
		<color>4</color>
	</indices>

	<cpuVariables>
		<position>[7, 0, 0]</position>,
	</cpuVariables>
}

// Iteration based fog 1
{
	<include>
		MandelBox, FogIteration
	</include>

	<distanceEstimator>
		<distanceBody>[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>
	<indices>
		<color>4</color>
	</indices>
	<cpuVariables>
		<position>[-1.01426327, -0.817652643, -3.69670177]</position>,
	</cpuVariables>
}

// Bulb forest 2
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

// Sphere inversion hell 3
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
	<variables>
		<innerRadius>power-1</innerRadius>, // power - distance(w, sun);
		<outerRadius>power+3</outerRadius>, // power + distance(w, sun);
		<scale>genericParameter</scale>,
		<foldingLimit>power</foldingLimit>,
		<zoomDetailRatio>.001</zoomDetailRatio>,
		<maxIterations>4</maxIterations>,
		<maxIterationsRelease>4</maxIterationsRelease>
		<genericParameter>0.363999933</genericParameter>,
	</variables>
	<cpuVariables>
		<position>[(1)[1.97544038, 1.99667835, 1.43940282], [-0.974210978, -0.814095318, -3.66229343]]</position>,
		<genericParameter>2</genericParameter>,
	</cpuVariables>
}

// Weird floaters 4
{
	// Tip: try increasing genericParameter
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

// Curious infinity 5
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
		<outerRadius>1</outerRadius>,
		<scale>1</scale>,
		<foldingLimit>1</foldingLimit>,
	</variables>
	<indices>
		<color>1</color>
	</indices>
}