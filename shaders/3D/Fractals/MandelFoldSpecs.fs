// Normal 0
{
	<include>
		MandelFold
	</include>

	<distanceEstimator>
		<distanceBody>[[rotate(rot1), mushroomFold, rotate(rot2), scaleTranslate], [sierpinskiFold, scaleTranslate],
		]</distanceBody>,
		<distanceTrap>[mandelBoxTrap]</distanceTrap>,
	</distanceEstimator>

	<variables>

	</variables>

	<cpuVariables>
		<position>[4, 0, 0]</position>,
	</cpuVariables>
}