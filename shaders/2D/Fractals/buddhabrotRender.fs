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
	float brightnessMagnitude = length(brightness);
																	// No idea why this term is necessary, but if you remove it the image will shift
	vec4 col = points[int(gl_FragCoord.y*screenSize.x+gl_FragCoord.x+screenSize.x*0.525)];

	color = vec4((col.xyz/brightnessMagnitude), 1);
</mainAA>