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

	uniform sampler2D testingTexture;
</uniforms>

<include>
	boxFold, sphereFold, triplexPow, icosaFold, sierpinskiFold, mengerFold
</include>

<textures>
	uniform sampler3D cloudDensity;
</textures>


<main>
	vec3 col = vec3(0);
	vec2 uv = (gl_FragCoord.xy-vec2(0.5)) / screenSize * 2.0 - 1.0;
	uv.x *= float(screenSize.x) / float(screenSize.y);
	uv *= zoom;

	vec3 direction = normalize(vec3(uv.xy, -1));

	direction *= rotation;

	col = vec3(0.2,0.4,0.6)*((direction.y+1)/1.7);

	//color = vec4(col, 1);
	
	color = vec4(texture(testingTexture, uv).xyz, 1);

	//image[int(gl_FragCoord.x*screenSize.y+gl_FragCoord.y)] = vec4(1,0,0,1);
</main>
