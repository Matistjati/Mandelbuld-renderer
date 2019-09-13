<flags>
	<mainLoopOff>
</flags>

<buffers>
/*<bufferType>renderInput</bufferType>*/
layout(std430, binding=0) buffer renderInput
{
    vec4 points[];
};
#define IndexPoints(X,Y) uint(X+(Y-step(3,Y)*3)*screenSize.x+screenSize.x)
</buffers>

<mainAA>
	vec4 col = points[IndexPoints(gl_FragCoord.x, gl_FragCoord.y)];


#if 0
	// Buddhabrot Wx,Wy
	vec4 brightness = points[IndexPoints(0.35875 * screenSize.x, 0.5 * screenSize.y)];

	color = vec4((col.xyz/brightness.xyz)*0.1, 1);
#else
	// An approximation used for buddhagrams. May need manual tweaking for optimal results (it depends on alot of paramters, such as screenSize, miniterations and maxiterations)
	//color = vec4((col.xyz/(float(frame)*4000))*vec3(20, 5, 1), 1);
	color = vec4(col.xyz/float(frame), 1);
#endif
</mainAA>