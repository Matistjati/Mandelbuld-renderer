<uniforms>
	/*<GuiHint>GuiType: slider, Name: Fog Darkness, Range: (0, 10)</GuiHint>*/
	uniform float fogDarkness = 5;
	
	/*<GuiHint>GuiType: slider, Name: Distance Diff, Range: (0, 4)</GuiHint>*/
	uniform float distDiff = 2;
	
	/*<GuiHint>GuiType: slider, Name: Scale, Range: (-2, 4)</GuiHint>*/
	uniform float scale = 2;

	/*<GuiHint>GuiType: colorPicker, Name: First rotation</GuiHint>*/
	uniform vec3 rot1 = vec3(0);
	
	/*<GuiHint>GuiType: colorPicker, Name: Second rotation</GuiHint>*/
	uniform vec3 rot2 = vec3(0);
	
	/*<GuiHint>GuiType: colorPicker, Name: Center</GuiHint>*/
	uniform vec3 offset = vec3(1);

	/*<GuiHint>GuiType: colorPicker, Name: Color A</GuiHint>*/
	uniform vec3 colorA = vec3(0, 0.707, 0.707);
	


	/*<GuiHint>GuiType: colorPicker, Name: Edge Color</GuiHint>*/
	uniform vec3 edgeColor = vec3(0, 0.4, 0.4);
</uniforms>

<include>
	sierpinskiFold, mengerFold, icosaFold
</include>

<distanceSetup>
	<mandelFoldSetup>
		float m = dot(w,w);
		vec4 trap = vec4(abs(w),m);

		vec3 phase1 = vec3(0,0,0);
		vec3 phase2 = vec3(0,0,0);

		vec3 sinrot1 = vec3(sin((rot1+phase1)*6.28318530718));
		vec3 cosrot1 = vec3(cos((rot1+phase1)*6.28318530718));
		mat2 rotMat1 = mat2(cosrot1.x,-sinrot1.x,sinrot1.x,cosrot1.x);
		
		vec3 sinrot2 = vec3(sin((rot2+phase2)*6.28318530718));
		vec3 cosrot2 = vec3(cos((rot2+phase2)*6.28318530718));
		mat2 rotMat2 = mat2(cosrot2.x,-sinrot2.x,sinrot2.x,cosrot2.x);
	</mandelFoldSetup>,
</distanceSetup>

<distanceBreakCondition>
	<foldBreak>if (length(w)>escapeRadius) {i++; break;}</foldBreak>,
</distanceBreakCondition>,

<operations>
	<scaleTranslate>w = w*scale-offset*(scale-1);</scaleTranslate>,
	<mengerScale>
		w = w*scale-vec3(offset.xy,0)*(scale-1);
		if(w.z>0.5*offset.z*(scale-1)) w.z-=offset.z*(scale-1);
	</mengerScale>,
	<rotate>
		w.xy*=parameter;
		w.xz*=parameter;
		w.zy*=parameter;
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
	col = (cos(edgeColor + colorA * steps * 10) * -0.5 + 0.5);
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
