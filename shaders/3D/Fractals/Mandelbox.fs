<extraSections>
[innerRadius], [outerRadius], [foldingLimit], [scale]
</extraSections>

<maxIterations>8</maxIterations>
<maxSteps>100</maxSteps>
<maxIterationsRelease>16</maxIterationsRelease>
<maxStepsRelease>1000</maxStepsRelease>
<antiAliasing>2</antiAliasing>
<zoomDetailRatio>.001</zoomDetailRatio>

<maxDist>power*7</maxDist>
<maxDistRelease>power*15</maxDistRelease>

<innerRadius>power-1</innerRadius> // power - distance(w, sun);
<outerRadius>power+1</outerRadius> // power + distance(w, sun); // Try adding something other than 1
<scale>genericParameter</scale>
<foldingLimit>power</foldingLimit>
<zoom>.01</zoom>

<uniforms>
	uniform float power = 1;
	uniform float genericParameter = 1;
</uniforms>

<operations>
	<sinSinhX>w.x = sin(w.x); w.x = sinh(w.x)*sin(w.x);</sinSinhX>,
	<cosZsinY>w.z = cos(w.z)*sin(w.z); w.y = cos(w.y)*sin(w.y);</cosZsinY>,
</operations>

<include>
	boxFold, sphereFold, triplexPow
</include>

<distanceTrap>
	<bulbForest>trap = mix(trap, vec4(w*w, m), 0.5);</bulbForest>
	<mandelBoxTrap>trap = min(trap, vec4(abs(w),m));</mandelBoxTrap>
</distanceTrap>

<color>
	<vec3(0.7, 0.1, 0.3)>,
	<vec3(0.707, 0, 0.707)>,
	<vec3(0, 0.707, 0.707)>,
	<vec3(0, 1, 0)>,
	<vec3(0.577, 0.577, 0.577)>,
	<vec3(1, 0, 0)>,
	<vec3(0.20739, 0.518, 0.8295)>,
	<vec3(0.78, 0.5, 0.13)>,
	<vec3(0.9, 0.15, 0.5)>,
	<vec3(0.5, 0, 0.5)>,
	<vec3(0.5, 0.5, 0.5)>,
	<vec3(0.3, 0.3, 0.3)>,
	<vec3(0.5, 0.65, 0.15)>,
	<vec3(0, 1, 0)>,
</color>

<coloring>
	col = vec3(0.1);
	col = mix(col, <color>, clamp(pow(trap.w,6.0), 0, 1));
	col += <color> * 0.1;
	//col *= <color>;
	//col *= steps;
	//col *= 1 - length(uv); // Flashlight
	//col = sqrt(col);
</coloring>

<edgeGlow>
	col += <color> * (steps * steps+0.25) * 0.45; // Fog
	// If you only want edge, i suggest disabling the sun and sky gradient
</edgeGlow>
