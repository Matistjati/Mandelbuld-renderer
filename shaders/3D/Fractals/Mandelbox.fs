<uniforms>
	/*<GuiHint>GuiType: slider, Name: Scale, Parent: fractalParams, Range: (-10, 10)</GuiHint>*/
	uniform float scale = 2;
	
	/*<GuiHint>GuiType: slider, Name: Inner Radius, Parent: fractalParams, Range: (-2, 4)</GuiHint>*/
	uniform float innerRadius = 0.5;
	
	/*<GuiHint>GuiType: slider, Name: Outer Radius, Parent: fractalParams, Range: (-2, 4)</GuiHint>*/
	uniform float outerRadius = 1;
	
	/*<GuiHint>GuiType: slider, Name: Folding limit, Parent: fractalParams, Range: (0, 4)</GuiHint>*/
	uniform float foldingLimit = 1;
	

	/*<GuiHint>GuiType: slider, Name: Fog Darkness, Range: (1.098, 50), Parent: Color</GuiHint>*/
	uniform float fogDarkness = 50;

	/*<GuiHint>GuiType: colorPicker, Name: Color A, Parent: Color</GuiHint>*/
	uniform vec3 colorA = vec3(0, 0.707, 0.707);
	
	/*<GuiHint>GuiType: colorPicker, Name: Color B, Parent: Color</GuiHint>*/
	uniform vec3 colorB = vec3(0, 0.707, 0.707);
	
	/*<GuiHint>GuiType: colorPicker, Name: Color C, Parent: Color</GuiHint>*/
	uniform vec3 colorC = vec3(0, 0.707, 0.707);
	
	/*<GuiHint>GuiType: colorPicker, Name: Color D, Parent: Color</GuiHint>*/
	uniform vec3 colorD = vec3(0, 0.707, 0.707);
	
	/*<GuiHint>GuiType: colorPicker, Name: Edge Color, Parent: Color</GuiHint>*/
	uniform vec3 edgeColor = vec3(0, 0.4, 0.4);

	/*<GuiHint>GuiType: slider, Name: Color Period, Parent: Color, Range: (0, 100)</GuiHint>*/
	uniform float colorFrequency = 10;
</uniforms>

<include>
	boxFold, sphereFold, triplexPow, icosaFold, sierpinskiFold, mengerFold
</include>

<operations>
	<sinSinhX>w.x = sin(w.x); w.x = sinh(w.x)*sin(w.x);</sinSinhX>,
	<cosZsinY>w.z = cos(w.z)*sin(w.z); w.y = cos(w.y)*sin(w.y);</cosZsinY>,
	<mengerIterate>
		w=abs(w);
		if(w.x-w.y<0) w.xy = w.yx;
		if(w.x-w.z<0) w.xz = w.zx;
		if(w.y-w.z<0) w.yz = w.zy;
		w = w*2.7-vec3(vec3(1).xy,0)*(2.7-1);
		if(w.z>0.5*vec3(1).z*(2.7-1)) w.z-=vec3(1).z*(2.7-1);
		dw*=2.7;
	</mengerIterate>,
	<icosaIterate>
		const float phi = 1.61803399; // golden ratio.

		const vec3 n1 = normalize(vec3(-phi,phi-1.0,1.0));
		const vec3 n2 = normalize(vec3(1.0,-phi,phi+1.0));

		w.yz=abs(w.yz);
		w-=2.0 * max(0,dot(w, n2)) * n2;

		w.xz = abs(w.xz);
		w-=2.0 * max(0,dot(w, n1)) * n1;
		w = w*2-vec3(0.850650808,0.525731112,0)*(2-1);
		dw*=2;
	</icosaIterate>
</operations>

<distanceTrap>
	<bulbForest>trap = mix(trap, vec4(w*w, m), 0.5);</bulbForest>
	<mandelBoxTrap>trap = min(trap, vec4(abs(w),m));</mandelBoxTrap>
</distanceTrap>

<coloring>
	<
		col = vec3(0.1);
		col = mix(col, colorA, clamp(pow(trap.w,6.0), 0., 1.));
		col += colorA * 0.1;
	>,


	<
		col.x += 1-cos(trap.y);
		col.y += 0.8*cos(trap.x);
		col.z += sqrt(sin(trap.z));
	>,

	<
		col = (cos(edgeColor + colorA * steps * colorFrequency) * -0.5 + 0.5);
	>,

	<
		col = (cos(edgeColor + colorA * iterations * colorFrequency) * -0.5 + 0.5);
	>,
	
	<
		col = colorA;
		col = mix(col, colorB, clamp(trap.x*trap.x, 0, 1));
		col = mix(col, colorC, clamp(trap.y*trap.y, 0, 1));
		col = mix(col, colorD, clamp(trap.z*trap.z, 0, 1));
	>,

	//col *= steps;
	//col *= 1 - length(uv); // Flashlight
	//col = sqrt(col);
</coloring>

<edgeGlow>
	<
		col = mix(col, edgeColor, pow(steps,fogDarkness));
	>,

	<
		col = edgeColor * pow(1-steps,pow(fogDarkness,1.1))*exp(-1/fogDarkness*t); /*Fog*/
		col = mix(col, vec3(sqrt(col)), clamp(pow(clamp(trap.w-steps*steps,0.,0.95),pow(fogDarkness,1.5)),0.,1.));
		col *= float(hitSurface);
	>,
	
	<
		col *= float(hitSurface);
	>,

	<
		col = (cos(edgeColor + colorA * steps * 10 * frequency) * -0.5 + 0.5);
	>,
</edgeGlow>
