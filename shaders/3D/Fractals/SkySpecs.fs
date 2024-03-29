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
		[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative, setW(abs(w))],
		[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative, icosaFold(w)],
		[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative, sierpinskiFold(w)],
		[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative, mengerFold(w)],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<colorA>vec3(0, 0.707, 0.707)</colorA>,
		<colorB>vec3(0, 0.707, 0.707)</colorB>,
	</variables>

	<cpuVariables>
		<position>[7, 0, 0]</position>,
	</cpuVariables>
}

// Folding 1
{
	<include>
		MandelBox
	</include>

	<uniforms>
		</*<GuiHint>GuiType: slider, Name: Folding plane, Parent: fractalParams, Range: (-2, 2)</GuiHint>*/
		uniform vec3 plane = vec3(0.6403136678865026,0.952709374335516,0.1672384066895063);>,
	</uniforms>

	<distanceEstimator>
		<distanceBody>[
		[planeFold(sun), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		[planeFold(normalize(plane)), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		[planeFold(vec3(0.577)), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		[planeFold(vec3(0.461, 0.877, 0.854)), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		[planeFold(vec3(1.,0,0)*rotation), boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<colorA>vec3(0, 0.707, 0.707)</colorA>,
		<colorB>vec3(0, 0.707, 0.707)</colorB>,
	</variables>

	<cpuVariables>
		<position>[7, 0, 0]</position>,
	</cpuVariables>
}

// Iteration based fog 2
{
	<include>
		MandelBox, FogIteration
	</include>

	<uniforms>
		</*<GuiHint>GuiType: slider, Name: Color period, Parent: color, Range: (0, 8)</GuiHint>*/
		uniform float frequency = 1;>,
	</uniforms>

	<distanceEstimator>
		<distanceBody>[[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative], [boxFold, sphereFold, boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative]]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<colorA>vec3(0.7, 0.1, 0.3)</colorA>,
		<colorB>vec3(0.7, 0.1, 0.3)</colorB>,
		<fogDarkness>1.098</fogDarkness>,
		<maxIterations>100</maxIterations>,
		<zoomDetailRatio>0.003</zoomDetailRatio>,
	</variables>

	<indices>
		<edgeGlow>3</edgeGlow>,
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

	<uniforms>
		</*<GuiHint>GuiType: slider, Name: Power, Parent: fractalParams, Range: (-2, 8)</GuiHint>*/
		uniform float power = 1;>,
	</uniforms>

	<distanceEstimator>
		<distanceBody>[[boxFold, sphereFold, boxFold, sphereFold, sinY, scaleAndTranslate, mandelBoxDerivative], [boxFold, sphereFold, boxFold, sphereFold, sinY, triplexPow, scaleAndTranslate, mandelBoxDerivative]]</distanceBody>,
		<distanceTrap>[bulbForest]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<innerRadius>0</innerRadius>, 
		<outerRadius>2</outerRadius>, 
		<scale>2</scale>,
		<foldingLimit>1</foldingLimit>,
		<zoomDetailRatio>.001</zoomDetailRatio>,
		<maxIterations>8</maxIterations>,
		<colorA>vec3(0, 0.707, 0.707)</colorA>,
	</variables>

	<indices>
		<edgeGlow>1</edgeGlow>,
	</indices>

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

	<flags>
		<edgeGlowOff>
	</flags>

	<variables>
		<colorA>vec3(0, 1, 0)</colorA>,
		<colorB>vec3(0, 1, 0)</colorB>,
		<maxIterations>4</maxIterations>,
		<innerRadius>0</innerRadius>, 
		<outerRadius>4</outerRadius>, 
		<scale>2</scale>,
		<foldingLimit>1</foldingLimit>,
		<zoomDetailRatio>.001</zoomDetailRatio>,
	</variables>

	<cpuVariables>
		<position>[(0)[1.97544038, 1.99667835, 1.43940282], [-0.974210978, -0.814095318, -3.66229343]]</position>,
		<genericParameter>2</genericParameter>,
	</cpuVariables>
}
// Weird floaters 5
{
	<include>
		MandelBox
	</include>
	<distanceEstimator>
		<distanceBody>[boxFold, sphereFold, cosZsinY, scaleAndTranslate, mandelBoxDerivative]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<colorA>vec3(0.707, 0, 0.707)</colorA>,
		<colorB>vec3(0.707, 0, 0.707)</colorB>,
		<innerRadius>0</innerRadius>,
		<outerRadius>2</outerRadius>,
		<scale>0.363999933</scale>,
		<foldingLimit>1</foldingLimit>,
		<zoomDetailRatio>.001</zoomDetailRatio>,
		<maxIterations>4</maxIterations>,
		<maxIterationsRelease>4</maxIterationsRelease>
	</variables>

	<cpuVariables>
		<position>[(1)[1.97544038, 1.99667835, 1.43940282], [-0.974210978, -0.814095318, -3.66229343]]</position>,
	</cpuVariables>
}

// Curious infinity 6
{
	<include>
		MandelBox
	</include>

	<distanceEstimator>
		<distanceBody>[[boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative, mengerIterate], [icosaIterate, boxFold, sphereFold, scaleAndTranslate, mandelBoxDerivative, mengerIterate]]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<colorA>vec3(0.707, 0, 0.707)</colorA>,
		<colorB>vec3(0.707, 0, 0.707)</colorB>,
		<innerRadius>0.1</innerRadius>,
		<outerRadius>1</outerRadius>,
		<scale>1</scale>,
		<foldingLimit>1.</foldingLimit>,
	</variables>

	<cpuVariables>
		<position>[0.1, 0., 0.]</position>,
	</cpuVariables>
}
