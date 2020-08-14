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
		<rot1>[[vec3(0.448,0.68,0.425)], [vec3(0.605,0.842,0.581)], [vec3(0.625,0.27,0.666666)], [vec3(0.633967817, 0.178958789, 0.580999970)]]</rot1>,
		<rot2>vec3(0.319,0.646,0.733)</rot2>,
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
		<distanceBreakCondition>foldBreak</distanceBreakCondition>,
	</distanceEstimator>

	<variables>
		<scale>2</scale>,
		<offset>vec3(1, 0, 0)</offset>,
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

// Cube 5
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceBody>[[rotate(rotMat1), cubeFold, rotate(rotMat2), scaleTranslate],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>
		<scale>2.2</scale>,
		<offset>vec3(1)</offset>,
	</variables>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}

// Hybrid 6
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceReturn>mandelFoldDistParam(4*2.7)</distanceReturn>,
		<distanceBreakCondition>foldBreak</distanceBreakCondition>,
		<distanceBody>[[rotate(rotMat1), sierpinskiFold, rotate(rotMat2), scaleTranslateParam(2, vec3(1)),
						rotate(rotMat1), mengerFold,	 rotate(rotMat2), mengerScaleParam(2.7, vec3(1)),
						rotate(rotMat1), sierpinskiFold, rotate(rotMat2), scaleTranslateParam(2, vec3(1))],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}

// Hybrid 7
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceReturn>mandelFoldDistParam(1.3*2)</distanceReturn>,
		<distanceBreakCondition>foldBreak</distanceBreakCondition>,
		<distanceBody>[[rotate(rotMat1), mushroomFold, rotate(rotMat2), scaleTranslateParam(1.3, vec3(2,4.8,0)),
						rotate(rotMat1), sierpinskiFold, rotate(rotMat2), scaleTranslateParam(2, vec3(1))
						],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}

// Hybrid 8
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceReturn>mandelFoldDistParam(2.7*2*2.2)</distanceReturn>,
		<distanceBreakCondition>foldBreak</distanceBreakCondition>,
		<distanceBody>[[rotate(rotMat1), mengerFold,	 rotate(rotMat2), mengerScaleParam(2.7, vec3(1)),
						rotate(rotMat1), cubeFold, rotate(rotMat2), scaleTranslateParam(2.2, vec3(1)),
						rotate(rotMat1), sierpinskiFold, rotate(rotMat2), scaleTranslateParam(2, vec3(1)),
						],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}