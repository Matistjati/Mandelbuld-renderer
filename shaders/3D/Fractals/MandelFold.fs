<uniforms>
	/*<GuiHint>GuiType: slider, Name: Fog Darkness, Parent: color, Range: (0, 10)</GuiHint>*/
	uniform float fogDarkness = 5;
	
	/*<GuiHint>GuiType: slider, Name: Distance Diff, Parent: render, Range: (0, 4)</GuiHint>*/
	uniform float distDiff = 2;
	
	/*<GuiHint>GuiType: slider, Name: Scale, Parent: fractalParams, Range: (-2, 4)</GuiHint>*/
	uniform float scale = 2;

	/*<GuiHint>GuiType: slider, Name: First rotation, Parent: fractalParams, Range: (0, 1)</GuiHint>*/
	uniform vec3 rot1 = vec3(0);
	
	/*<GuiHint>GuiType: slider, Name: Second rotation, Parent: fractalParams, Range: (0, 1)</GuiHint>*/
	uniform vec3 rot2 = vec3(0);
	
	/*<GuiHint>GuiType: slider, Name: Center, Parent: fractalParams, Range: (0, 5)</GuiHint>*/
	uniform vec3 offset = vec3(1);

	/*<GuiHint>GuiType: colorPicker, Name: Color A, Parent: color</GuiHint>*/
	uniform vec3 colorA = vec3(0, 0.707, 0.707);
	
	/*<GuiHint>GuiType: colorPicker, Name: Edge Color, Parent: color</GuiHint>*/
	uniform vec3 edgeColor = vec3(0, 0.4, 0.4);

	/*<GuiHint>GuiType: slider, Name: Color period, Parent: color, Range: (0, 200)</GuiHint>*/
	uniform float frequency = 10;
</uniforms>

<include>
	sierpinskiFold, mengerFold, icosaFold, sphereFold
</include>

<distanceSetup>
	<mandelFoldSetup>
		float m = dot(w,w);
		vec4 trap = vec4(abs(w),m);
	</mandelFoldSetup>,
</distanceSetup>

<distanceBreakCondition>
	<foldBreak>if (length(w)>escapeRadius) {i++; break;}</foldBreak>,
</distanceBreakCondition>,

<operations>
	<foldAbout>
		w-=2.0 * max(0,dot(w, parameter)) * parameter;
	</foldAbout>,
	<scaleTranslate>w = w*scale-offset*(scale-1);</scaleTranslate>,
	<scaleTranslateParam>w = w*parameter-parameter1*(parameter-1);</scaleTranslateParam>,
	<mengerScale>
		w = w*scale-vec3(offset.xy,0)*(scale-1);
		if(w.z>0.5*offset.z*(scale-1)) w.z-=offset.z*(scale-1);
	</mengerScale>,
	<mengerScaleParam>
		w = w*parameter-vec3(parameter1.xy,0)*(parameter-1);
		if(w.z>0.5*parameter1.z*(parameter-1)) w.z-=parameter1.z*(parameter-1);
	</mengerScaleParam>,
	<rotate>
		w*=parameter;
	</rotate>,
	<mushroomFold>
		w=abs(w);
		if(w.x-w.y<0) w.xy = w.yx;
		if(w.x-w.z<0) w.xz = w.zx;
		if(w.y-w.z<0) w.yz = w.zy;
	</mushroomFold>,

	<cubeFold>
		w = abs(w);
	</cubeFold>

	<sierpinskiFold>
		sierpinskiFold(w);
	</sierpinskiFold>,
	
	<octahedronFold>
		if(w.x-w.y<0) w.xy = w.yx;
		if(w.x+w.y<0) w.xy = -w.yx;
		if(w.x-w.z<0) w.xz = w.zx;
		if(w.x+w.z<0) w.xz = -w.zx;
		if(w.y-w.z<0) w.yz = w.zy;
		if(w.y+w.z<0) w.yz = -w.zy;
	</octahedronFold>,
	
	<mengerFold>
		mengerFold(w);
	</mengerFold>,
	
	<icosaFold>
		icosaFold(w);
	</icosaFold>,
</operations>

<include>

</include>

<distanceTrap>

</distanceTrap>

<coloring>
	<
	col = vec3(0.1);
	col = mix(col, colorA, clamp(pow(trap.w,6.0), 0, 1));
	col += colorA * 0.1;
	>,


	<
	col.x += 1-cos(trap.y);
	col.y += 0.8*cos(trap.x);
	col.z += sqrt(sin(trap.z));
	>,

	<
	col = (cos(edgeColor + colorA * steps * frequency) * -0.5 + 0.5);
	>,

	//col *= steps;
	//col *= 1 - length(uv); // Flashlight
	//col = sqrt(col);
</coloring>

<edgeGlow>
	<
	col = mix(col, edgeColor, pow(steps,fogDarkness));
	>,
</edgeGlow>
