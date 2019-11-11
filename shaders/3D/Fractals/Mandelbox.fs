<antiAliasing>2</antiAliasing>

<uniforms>

	/*<GuiHint>GuiType: slider, Name: Fog Darkness, Range: (-2, 50)</GuiHint>*/
	uniform float fogDarkness = 50;

	/*<GuiHint>GuiType: slider, Name: Scale, Range: (-10, 10)</GuiHint>*/
	uniform float scale = 2;
	
	/*<GuiHint>GuiType: slider, Name: Inner Radius, Range: (-2, 4)</GuiHint>*/
	uniform float innerRadius = 0.5;
	
	/*<GuiHint>GuiType: slider, Name: Outer Radius, Range: (-2, 4)</GuiHint>*/
	uniform float outerRadius = 1;
	
	/*<GuiHint>GuiType: slider, Name: Folding limit, Range: (0, 4)</GuiHint>*/
	uniform float foldingLimit = 1;

	/*<GuiHint>GuiType: colorPicker, Name: Color A</GuiHint>*/
	uniform vec3 colorA = vec3(0, 0.707, 0.707);
	
	/*<GuiHint>GuiType: colorPicker, Name: Edge Color</GuiHint>*/
	uniform vec3 edgeColor = vec3(0, 0.707, 0.707);
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

<coloring>
	<col = vec3(0.1);
	col = mix(col, colorA, clamp(pow(trap.w,6.0), 0, 1));
	col += colorA * 0.1;>,


	<vec3 p = (ray.origin + ray.dir * t);
	col.x += 1-cos(trap.y);
	col.y += 0.8*cos(trap.x);
	col.z += sqrt(sin(trap.z));>,

	//col *= steps;
	//col *= 1 - length(uv); // Flashlight
	//col = sqrt(col);
</coloring>

<edgeGlow>
	col += edgeColor * pow(1-steps,fogDarkness); // Fog
	//col = mix(<color>, vec3(0.), steps);
</edgeGlow>
