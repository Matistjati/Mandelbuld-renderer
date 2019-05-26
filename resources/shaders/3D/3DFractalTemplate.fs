%uniforms%

%/uniforms%

%constants%
const float antiAliasing = 2;
%/constants%

%DE%

float DistanceEstimator(vec3 start, out vec4 resColor, float Power)
{

}
%/DE%

%Color%
col = vec3(0.01);

%/Color%

%main%
	vec2 uv = gl_FragCoord.xy / screenSize;
	uv = uv * 2.0 - 1.0;

	uv.x *= float(screenSize.x) / float(screenSize.y);

	vec3 direction = normalize(vec3(uv.xy, 1));

	direction.zy *= pitchMatrix;

	direction.xz *= yawMatrix;
	direction.xy *= rollMatrix;
	direction.y *= worldFlip;
	
	
	vec3 col = render(Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz));

    color = vec4(col.xyz, 1.0);
%/main%

%mainAA%

	vec3 col = vec3(0.0);
	for (int i = 0; i < antiAliasing; i++)
	{
		for (int j = 0; j < antiAliasing; j++)
		{
			vec2 frag = gl_FragCoord.xy;
			frag.x += float(i)/antiAliasing;
			frag.y += float(j)/antiAliasing;
			vec2 uv = frag / screenSize;
			uv = uv * 2.0 - 1.0;
			uv.x *= float(screenSize.x) / float(screenSize.y);

			vec3 direction = normalize(vec3(uv.xy, 1));

			direction.zy *= pitchMatrix;

			direction.xz *= yawMatrix;
			direction.xy *= rollMatrix;
			direction.y *= worldFlip;
	

			Ray	ray = Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz);
			col += render(ray);
	
			//vec3 col = render(Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz));
		}
	}
	col /= float(antiAliasing*antiAliasing);

    color = vec4(col.xyz, 1.0);
%/mainAA%