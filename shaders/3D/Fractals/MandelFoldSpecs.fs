// Tree 0
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceBody>[[rotate(rotMat1), mushroomFold, rotate(rotMat2), scaleTranslate],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
		<distanceBreakCondition>foldBreak</distanceBreakCondition>,
	</distanceEstimator>

	<variables>
		<scale>1.3</scale>,
		<rot1>vec3(0.53,0.83,0.79)</rot1>,
		<rot2>vec3(0,0.5,0.6)</rot2>,
		<offset>vec3(2,4.8,0)</offset>,
	</variables>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}

// Sierpinski 1
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceBody>[[rotate(rotMat1), sierpinskiFold, rotate(rotMat2), scaleTranslate],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
		<distanceBreakCondition>foldBreak</distanceBreakCondition>,
	</distanceEstimator>

	<variables>
		<scale>2</scale>,
	</variables>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}

// Octahedron 2
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceBody>[[rotate(rotMat1), octahedronFold, rotate(rotMat2), scaleTranslate],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<scale>2</scale>,
		<offset>vec3(1,0,0)</offset>,
	</variables>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}

// Menger sponge 3
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceBody>[[rotate(rotMat1), mengerFold, rotate(rotMat2), mengerScale],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<scale>2.7</scale>,
		<offset>vec3(1)</offset>,
	</variables>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}

// Icosahedron 4
{
	<include>
		MandelFold
	</include>

	<uniforms>
		</*<GuiHint>GuiType: slider, Name: Color period, Range: (0, 8)</GuiHint>*/
		uniform float frequency = 1;>,
	</uniforms>

	<distanceEstimator>
		<distanceBody>[[rotate(rotMat1), icosaFold, rotate(rotMat2), scaleTranslate],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<scale>2</scale>,
		<offset>vec3(0.850650808,0.525731112,0)</offset>,
	</variables>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}

// Icosahedron 4
{
	<include>
		MandelFold
	</include>

	<uniforms>
		</*<GuiHint>GuiType: slider, Name: Color period, Range: (0, 8)</GuiHint>*/
		uniform float frequency = 1;>,
	</uniforms>

	<distanceEstimator>
		<distanceBody>[[rotate(rotMat1), icosaFold, mengerFold, rotate(rotMat2), scaleTranslate],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<scale>2</scale>,
		<offset>vec3(0.850650808,0.525731112,0)</offset>,
	</variables>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}