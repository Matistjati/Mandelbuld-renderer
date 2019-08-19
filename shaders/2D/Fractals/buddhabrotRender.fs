<flags>
	<mainLoopOff>
</flags>

<buffers>
/*<bufferType>renderInput</bufferType>*/
layout(std430, binding=1) buffer renderInput
{
    uvec4 points[];
};
#define IndexPoints(X,Y) uint(X+Y*screenSize.x+screenSize.x)
</buffers>


<mainAA>
	vec4 brightness = points[IndexPoints(0.35875 * screenSize.x, 0.5 * screenSize.y)];
	float brightnessMagnitude = length(brightness.xyz);

	vec4 col = points[IndexPoints(gl_FragCoord.x, gl_FragCoord.y)];

	//col = pow(col, vec4(0.75, 0.94, 0.97, 1));
	color = vec4((col.xyz/brightnessMagnitude)*0.3, 1);
	//color = col / 256;
</mainAA>