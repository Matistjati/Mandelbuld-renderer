<antiAliasing>2</antiAliasing>

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

<distanceSetup>
	<mandelFoldSetup>float m = dot(w,w); vec4 trap = vec4(abs(w),m);</mandelFoldSetup>,
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
		vec3 sinparameter = vec3(sin(parameter*6.28318530718));
		vec3 cosparameter = vec3(cos(parameter*6.28318530718));
		w.xy*=mat2(cosparameter.x,-sinparameter.x,sinparameter.x,cosparameter.x);
		w.xz*=mat2(cosparameter.x,-sinparameter.x,sinparameter.x,cosparameter.x);
		w.zy*=mat2(cosparameter.x,-sinparameter.x,sinparameter.x,cosparameter.x);
	</rotate>,
	<mushroomFold>
		w=abs(w);
		if(w.x-w.y<0) w.xy = w.yx;
		if(w.x-w.z<0) w.xz = w.zx;
		if(w.y-w.z<0) w.yz = w.zy;
	</mushroomFold>,
	<sierpinskiFold>
		if(w.x + w.y < 0) w.xy = -w.yx;
		if(w.x + w.z < 0) w.xz = -w.zx;
		if(w.y + w.z < 0) w.yz = -w.zy;
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
		w=abs(w);
		if(w.x-w.y<0) w.xy = w.yx;
		if(w.x-w.z<0) w.xz = w.zx;
		if(w.y-w.z<0) w.yz = w.zy;
	</mengerFold>,
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

	//col *= steps;
	//col *= 1 - length(uv); // Flashlight
	//col = sqrt(col);
</coloring>

<edgeGlow>
	<
	col = mix(col, edgeColor, pow(steps,fogDarkness));
	>,
</edgeGlow>
