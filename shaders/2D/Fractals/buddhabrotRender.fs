<flags>
	<mainLoopOff>
</flags>

<buffers>
/*<bufferType>renderInput</bufferType>*/
layout(std430, binding=1) buffer renderInput
{
    uvec4 points[];
};
</buffers>


<mainAA>
	vec4 brightness = points[int((0.35875) * screenSize.x * screenSize.y + 0.5 * screenSize.y)];

																	// No idea why this term is necessary, but if you remove it the image will shift
	vec4 colA = points[int(gl_FragCoord.y*screenSize.x+gl_FragCoord.x+screenSize.x*0.525)];
	vec4 colB = points[int((screenSize.y - gl_FragCoord.y)*screenSize.x+gl_FragCoord.x+screenSize.x*0.525)];
	brightness = max(brightness, vec4(1.f));
	color = vec4(((colA.xyz+colB.xyz)/brightness.xyz)*0.25, 1);
</mainAA>