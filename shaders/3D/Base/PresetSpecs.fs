<FogIteration>
{
	<variables>
		<sunColor>vec3(0,0,0)</sunColor>,
		<skyColor>vec3(0,0,0)</skyColor>,
		<fogColoring>true</fogColoring>,
	</variables>
}
</FogIteration>

<MandelBox>
{
	<distanceEstimator>
		<distanceReturn>[mandelBoxDist]</distanceReturn>,
		<distanceTrap>[defaultTrap]</distanceTrap>,
		<distanceSetup>[[scaleInit, defaultSetup], [sinTanW, scaleInit, defaultSetup], [scaleInit, defaultSetup, sinTanW], [tanW, scaleInit, defaultSetup],
						[sinWxz, scaleInit, defaultSetup], [subWLength, scaleInit, defaultSetup], [scaleInit, defaultSetup, powY], [scaleInit, defaultSetup, addYcMulRot],
						[scaleInit, defaultSetup, addWcMulRot], [scaleInit, defaultSetup, addYsinX], ]</distanceSetup>,
		<distanceTrapReturn>[defaultTrapReturn]</distanceTrapReturn>,
		<distanceBreakCondition>[defaultBreak]</distanceBreakCondition>,
		<distanceExtraOperations>[[], [mulW(rotation)], [addW(dot(position,position))]/*x=0.323416412 y=0.588775635 z=2.41064572*/, [addW(sin(dot(w,w)))], [addWY((c*rotation).x)], [setXY(sin(w.xy))],
								  [addWY(fract(c).y*sign(w.y))], [addWY(c.y*c.y)], [addWY(dot(c*rotation,sun))], [addWY(length(c))], [addW(c*dot(sun,c))], [mandelbulbModded()],
								  [mandelbulbModded(theta *= 2)], [mandelbulbModded(theta *= r; phi *= r)], [SwapXZ], ]</distanceExtraOperations>,
	</distanceEstimator>
}
</MandelBox>

<MandelFold>
{
	<distanceEstimator>
		<distanceReturn>[mandelFoldDist]</distanceReturn>,
		<distanceTrap>[defaultTrap]</distanceTrap>,
		<distanceSetup>[[mandelFoldSetup], ]</distanceSetup>,
		<distanceTrapReturn>[defaultTrapReturn]</distanceTrapReturn>,
		<distanceBreakCondition>[defaultBreak]</distanceBreakCondition>,
		<distanceExtraOperations>[[], ]</distanceExtraOperations>,
	</distanceEstimator>

	<variables>
		<fudgeFactor>0.95</fudgeFactor>,
		<maxIterations>40</maxIterations>,
	</variables>
}
</MandelFold>

<MandelBulb>
{
	<distanceEstimator>
		<distanceReturn>[mandelBulbDist]</distanceReturn>,
		<distanceTrap>[defaultTrap]</distanceTrap>,
		<distanceSetup>[[mandelBulbInit], [sinTanW, mandelBulbInit], [sinW, mandelBulbInit], [sinWxz, mandelBulbInit], [mandelBulbInit, subY9]]</distanceSetup>,
		<distanceTrapReturn>[mandelBulbTrapReturn]</distanceTrapReturn>,
		<distanceBreakCondition>[defaultBreak]</distanceBreakCondition>,
		<distanceExtraOperations>[[], [mulW(rotation)], [addW(cross(w, position))], [addW(sin(dot(w,w)))], [addW(c*rotation)], [addWY(fract(c).y*sign(w.y))], [addWY(c.y*c.y)],
								  [addWY(dot(c*rotation,sun))], [addWY(length(c))], [addW(c*dot(sun,c))], ]</distanceExtraOperations>,
	</distanceEstimator>

	<variables>
		<shadowSoftness>16</shadowSoftness>,
		<antiAliasing>1</antiAliasing>,
		<maxIterations>12</maxIterations>,
		<zoomDetailRatio>0.01</zoomDetailRatio>,
		<gamma>0.9</gamma>,
	</variables>
}
</MandelBulb>