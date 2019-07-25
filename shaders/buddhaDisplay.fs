#version 430

layout(location = 0) out vec4 color;


uniform vec2 screenSize = vec2(1280,720);

layout(std430, binding=0) buffer densityPlot
{
    vec4 points[];
};


void main()
{
																	// No idea why this term is necessary, but if you remove it the image will shift
	vec4 col =  points[int(gl_FragCoord.y*screenSize.x+gl_FragCoord.x+screenSize.x/2)];
	vec4 brightness = points[int(0.38046875 * screenSize.x * screenSize.y + 0.5 * screenSize.y)];
	brightness = max(brightness, vec4(1.f));
	color = vec4(col.xyz/brightness.xyz, 1);
}