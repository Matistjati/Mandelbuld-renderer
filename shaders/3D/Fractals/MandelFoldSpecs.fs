// Tree 0
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceBody>[[rotate(rot1), mushroomFold, rotate(rot2), scaleTranslate],
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
		<distanceBody>[[rotate(rot1), sierpinskiFold, rotate(rot2), scaleTranslate],
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
		<distanceBody>[[rotate(rot1), octahedronFold, rotate(rot2), scaleTranslate],
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
		<distanceBody>[[rotate(rot1), mengerFold, rotate(rot2), mengerScale],
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