<flags>
	<mainLoopOff>
</flags>

<buffers>
layout(std430, binding=0) buffer renderInput
{
    vec4 points[];
};
</buffers>


<mainAA>
																	// No idea why this term is necessary, but if you remove it the image will shift
	vec4 col =  points[int(gl_FragCoord.y*screenSize.x+gl_FragCoord.x+screenSize.x/2)];
	vec4 brightness = points[int(0.38046875 * screenSize.x * screenSize.y + 0.5 * screenSize.y)];
	brightness = max(brightness, vec4(1.f));
	color = vec4(col.xyz/brightness.xyz, 1);
</mainAA>