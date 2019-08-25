<flags>
	<mainLoopOff>
</flags>

<buffers>
/*<bufferType>renderInput</bufferType>*/
layout(std430, binding=1) buffer renderInput
{
    vec4 points[];
};
#define IndexPoints(X,Y) uint(X+Y*screenSize.x+screenSize.x)
</buffers>


<mainAA>
	vec4 brightness = points[IndexPoints(0.35875 * screenSize.x, 0.5 * screenSize.y)];
	float brightnessMagnitude = length(brightness.xyz);

	vec4 col = points[IndexPoints(gl_FragCoord.x, gl_FragCoord.y)];

	//color = vec4((col.xyz/brightness.xyz)*0.1, 1);

	// A poor approximation used for buddhagrams
	//color = vec4((col.xyz/(float(frame)*1000))*0.1, 1);

	//color = col / 512;
</mainAA>