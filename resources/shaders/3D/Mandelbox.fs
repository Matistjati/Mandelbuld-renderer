%uniforms%
uniform float power = 1;
uniform float genericParameter = 1;
uniform vec3 sun;
%/uniforms%

%constants%
const float antiAliasing = 2;
%/constants%

%DE%
void sphereFold(inout vec3 z, inout float dz)
{
	float minRadius2 = power - genericParameter;
	float fixedRadius2 = power + genericParameter;
	float r2 = dot(z,z);
	if (r2<minRadius2) { 
		// linear inner scaling
		float temp = (fixedRadius2/minRadius2);
		z *= temp;
		dz*= temp;
	} else if (r2<fixedRadius2) { 
		// this is the actual sphere inversion
		float temp =(fixedRadius2/r2);
		z *= temp;
		dz*= temp;
	}
}

void boxFold(inout vec3 z, inout float dz) {
	float foldingLimit = power;
	z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
}
float DistanceEstimator(vec3 z, out vec4 resColor, float _)
{
	float Scale = 2;
	vec3 c = z;
	float dr = 1.0;
	float m;
	vec4 trap = vec4(0,0,0,0);
	for (int n = 0; n < 8; n++) 
	{
		boxFold(z,dr);     
		sphereFold(z,dr);
		
		//boxFold(z,dr);     
		//sphereFold(z,dr);  
 		
        z=Scale*z + c;  // Scale & Translate
		
		//z.x *= cos(z.x);
		
		//z.z = cos(z.z)*sin(z.z);
		//z.y = cos(z.y)*sin(z.y);

        dr = dr*abs(Scale)+1.0;

		trap = max(trap, vec4(abs(z), m));

		m = dot(z,z);
	}
	resColor = trap;
	float r = length(z);
	return r/abs(dr);
}
%/DE%

%Color%
col = vec3(0.01);
col = mix(col, vec3(0.54,0.3,0.07), clamp(trap.y,0.0,1.0)); // Inner
col = mix(col, vec3(0.02,0.4,0.30), clamp(trap.z*trap.z,0.0,1.0));
col = mix(col, vec3(0.78, 0.5, 0.13), clamp(pow(trap.w,6.0),0.0,1.0)); // Stripes
col *= 0.5;
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