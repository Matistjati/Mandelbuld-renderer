<FogIteration>
{
	<flags>
		<skyOff>
		<sunOff>
	</flags>
	<variables>
		<maxDist>0</maxDist>,
		<maxDistRelease>0</maxDistRelease>,
		<maxSteps>100</maxSteps>,
		<maxStepsRelease>100</maxStepsRelease>,
	</variables>
	<alternateFunctions>
		<trace>traceIterationPercent</trace>
	</alternateFunctions>
}
</FogIteration>

<MandelBox>
{
	<distanceEstimator>
		<distanceReturn>[mandelBoxDist]</distanceReturn>,
		<distanceTrap>[defaultTrap]</distanceTrap>,
		<distanceSetup>[[scaleInit, defaultSetup], [sinTanW, scaleInit, defaultSetup], [scaleInit, defaultSetup, sinTanW], [tanW, scaleInit, defaultSetup],
						[sinWxz, scaleInit, defaultSetup], [subWLength, scaleInit, defaultSetup],]</distanceSetup>,
		<distanceTrapReturn>[defaultTrapReturn]</distanceTrapReturn>,
		<distanceBreakCondition>[none]</distanceBreakCondition>,
	</distanceEstimator>
	<cpuVariables>
		<power>1</power>,
		<genericParameter>2</genericParameter>
	</cpuVariables>
}
</MandelBox>

<MandelBulb>
{
<distanceEstimator>
	<distanceReturn>[mandelBulbDist]</distanceReturn>,
	<distanceTrap>[defaultTrap]</distanceTrap>,
	<distanceSetup>[[mandelBulbInit], [sinTanW, mandelBulbInit], [sinW, mandelBulbInit], [sinWxz, mandelBulbInit]]</distanceSetup>,
	<distanceTrapReturn>[mandelBulbTrapReturn]</distanceTrapReturn>,
	<distanceBreakCondition>[defaultBreak]</distanceBreakCondition>,
</distanceEstimator>
<indices>
	<color>0</color>,
	<colorB>0</colorB>,
	<colorC>0</colorC>,
</indices>
<cpuVariables>
	<power>8</power>,
</cpuVariables>
}
</MandelBulb>