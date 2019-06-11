<extraSections>
[innerRadius], [outerRadius], [foldingLimit], [scale]
</extraSections>

<maxIterations>8</maxIterations>
<maxSteps>100</maxSteps>
<maxIterationsRelease>16</maxIterationsRelease>
<maxStepsRelease>5000</maxStepsRelease>
<antiAliasing>2</antiAliasing>

<maxDist>power*2</maxDist>
<maxDistRelease>power*20</maxDistRelease>

<innerRadius>power-1</innerRadius> // power - distance(w, sun);
<outerRadius>power+1</outerRadius> // power + distance(w, sun);
<scale>genericParameter</scale>
<foldingLimit>power</foldingLimit>
<zoom>.01</zoom>

<uniforms>
	uniform float power = 1;
	uniform float genericParameter = 1;
</uniforms>

<deformation>
	<w.x = sin(w.x);
	w.x = sinh(w.x)*sin(w.x);>,
			
	<w.z = cos(w.z)*sin(w.z);
	w.y = cos(w.y)*sin(w.y);>,

</deformation>

<include>
boxFold, sphereFold, triplexPow
</include>

<distanceTrap>
	<bulbForest>trap = mix(trap, vec4(w*w, m), 0.5);</bulbForest>
	<mandelBoxTrap>trap = max(trap, vec4(abs(w),m));</mandelBoxTrap>
</distanceTrap>

<color>
	<vec3(1, 0, 0.3)>,
	<vec3(1, 0, 1)>,
	<vec3(0, 1, 1)>,
	<vec3(0, 1, 0)>,
	<vec3(1, 1, 1)>,
	<vec3(0.9, 0.8, 0.2)>,
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
	col *= <color> * (1-t*pow(steps,32));
	//col *= steps;
	//col *= 1 - length(uv); // Flashlight
	//col = sqrt(col);
</coloring>

<edgeGlow>
	col += <color> * (steps * steps+0.25) * 0.45; // Fog
	// If you only want edge, i suggest disabling the sun and sky gradient
</edgeGlow>
