#version 430

uniform vec2 screenSize = vec2(1920, 1080);

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(std430, binding = 0) buffer destBuffer
{
	vec4 data[];
};


void main()
{
	uint fragCoord = gl_GlobalInvocationID.y*1920+gl_GlobalInvocationID.x;

	vec2 c = (2*gl_GlobalInvocationID.xy-screenSize)/screenSize.y;

	vec2 z = c;
	float i = 0;
	for	(int f = 0; i < 200.; i++)
	{
        z = mat2(z, -z.y, z.x) * z + c;
		if (dot(z,z)>4) break;
	}
    i = i / float(200.);
	vec3 col = vec3(i);
	data[fragCoord] = vec4(col,1);
}