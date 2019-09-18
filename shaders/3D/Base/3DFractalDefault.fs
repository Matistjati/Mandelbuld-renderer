<maxIterations>4</maxIterations>
<maxSteps>60</maxSteps>
<shadowSoftness>4</shadowSoftness> // Higher = harder shadow
<maxIterationsRelease>16</maxIterationsRelease>
<maxStepsRelease>1000</maxStepsRelease>
<antiAliasing>2<antiAliasing>
<zoomDetailRatio>.001</zoomDetailRatio>
<escapeRadius>4</escapeRadius>

<sceneDistance>
	float sceneDistance(vec3 position, out vec4 resColor)
	{
		return DistanceEstimator(position, resColor, power);
	}
</sceneDistance>

<sky>
	col += vec3(0.8, 0.95, 1.0) * (0.6 + 0.4 * ray.dir.y);
</sky>

<sun>
	col += sunSize * vec3(0.8,0.7,0.5) * pow(clamp(dot(ray.dir, sun), 0.0, 1.0), sunTightness);
</sun>

<distanceSetup>
	<sinTanW>w = sin(w); w = tan(w);</sinTanW>,
	<sinW>w = sin(w);</sinW>,
	<tanW>w=tan(w);</tanW>,
	<powY>w.y=pow(w.y,0.9);</powY>,
	<sinWxz>w.xz=sin(w.xz);</sinWxz>,
	<addYcMulRot>w.y+=(c*rotation).x;</addYcMulRot>,
	<addWcMulRot>w+=(c*rotation);</addWcMulRot>,
	<subY9>w.y-=0.9;</subY9>,
	<addYsinX>w.y+=sin(w.x);</addYsinX>,
	<subWLength>w-=length(w);</subWLength>,
	<addPositionToW>w+=position;</addPositionToW>,
	<scaleInit>float Scale = <scale>;</scaleInit>,
	<defaultSetup>vec3 c = w; float m; vec4 trap = vec4(abs(w),m); float dw = 1.0;</defaultSetup>,
	<mandelBulbInit>vec3 c = w; float m = dot(w,w); vec4 trap = vec4(abs(w),m); float dw = 1.0;</mandelBulbInit>,
</distanceSetup>

<operations>
	<boxFold>boxFold(w, <foldingLimit>);</boxFold>,
	<sphereFold>m = dot(w, w);	sphereFold(w, dw, m, <innerRadius>, <outerRadius>);</sphereFold>,
	<triplexPow>w = triplexPow(w, power, dw, m);</triplexPow>,
	<scaleAndTranslate>w=Scale*w + c;</scaleAndTranslate>,
	<translate>w+=c;</translate>,
	<mandelBoxDerivative>dw = dw*abs(Scale)+1.0;</mandelBoxDerivative>,
	<sinY>w.y = sin(w.y);</sinY>,
	<addLength>w+=length(w);</addLength>,
	<mulW>w*=parameter;</mulW>,
	<addW>w+=parameter;</addW>,
	<addWY>w.y+=parameter;</addWY>,
	<setY>w.y=parameter;</setY>,
	<setXY>w.xy=parameter;</setXY>,
	<planeFold>w-=2 * min(0, dot(w, parameter)) * parameter;</planeFold>,
	<setW>w=parameter;</setW>,
	<mandelbulbModded>float r = length(w); float theta = 2 * atan(w.x, w.z); float phi = 1 * acos(w.y / r); parameter; w = r * vec3(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi));</mandelbulbModded>,
</operations>

<distanceReturn>
	<mandelBoxDist>length(w)/abs(dw);</mandelBoxDist>,
	<mandelBulbDist>abs(0.25* log(m)*sqrt(m)/dw);</mandelBulbDist>,
</distanceReturn>

<distanceTrap>
	<defaultTrap>m = dot(w,w);trap = min(trap, vec4(abs(w),m));</defaultTrap>,
</distanceTrap>

<distanceTrapReturn>
	<defaultTrapReturn>trap;</defaultTrapReturn>,
</distanceTrapReturn>

<distanceBreakCondition>
	<none></none>,
	<defaultBreak>if (m > <escapeRadius>) break;</defaultBreak>,
</distanceBreakCondition>

<distanceEstimator>
	float DistanceEstimator(vec3 w, out vec4 resColor, float Power)
	{
		<distanceSetup>

		for(int i = 0; i < <maxIterations>; i++)
		{
			<distanceBody>

			<distanceExtraOperations>

			<distanceTrap>

			<distanceBreakCondition>
		}
	
		resColor = <distanceTrapReturn>

		return <distanceReturn>
	}
</distanceEstimator>


<trace>
	float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
	{
		float res = -1.0;

		vec4 trap;

		float t = 0;
		int i = 0;
		for(; i<maxSteps; i++)
		{ 
			vec3 pos = ray.origin + ray.dir * t;
			float h = sceneDistance(pos, trap);
			float th = 0.25 * px * t;

			if(h<th || h > <maxDist>)
			{
				break;
			}
			t += h;
		}

		percentSteps = float(i)/float(maxSteps);

		if (t < <maxDist>)
		{
			trapOut = trap;
			res = t;
		}

		return res;
	}
</trace>

<lightingFunctions>
	vec3 calculateNormal(vec3 p)
	{
		const vec3 small_step = vec3(0.001, 0.0, 0.0);

		vec3 gradient;
		vec4 temp;

		gradient.x = sceneDistance(p + small_step.xyy, temp) - sceneDistance(p - small_step.xyy, temp);
		gradient.y = sceneDistance(p + small_step.yxy, temp) - sceneDistance(p - small_step.yxy, temp);
		gradient.z = sceneDistance(p + small_step.yyx, temp) - sceneDistance(p - small_step.yyx, temp);

		return normalize(gradient);
	}

	float SoftShadow(Ray ray, float k)
	{
		float result = 1.0;
		float t = 0.0;
	
		vec4 temp;

		for(int i = 0; i < maxSteps; i++)
		{
			float h = sceneDistance(ray.origin + ray.dir * t, temp);
			result = min(result, k * h / t);

			if(result < 0.001) break;

			t += clamp(h, 0.01, 32);
		}
		return clamp(result, 0.0, 1.0);
	}
</lightingFunctions>

<render>
	vec3 render(Ray ray, vec2 uv)
	{
		float px = (100/screenSize.y) * zoom * zoomDetailRatio;
		vec4 trap;
		float steps;

		float t = trace(ray, trap, px, steps);

		vec3 col = vec3(0);

		// Color the sky if we don't hit the fractal
		if(t < 0.0)
		{
			// Sky gradient
			<sky>
     	

			// Sun
			<sun>

			<edgeGlow>
		}
		else
		{
			<coloring>

			// Lighting

			// The end position of the ray
			vec3 pos = (ray.origin + ray.dir * t);
			vec3 normal = calculateNormal(pos);
			Ray fractalToSun = Ray(pos + 0.001 * normal, sun);
			vec3 fractalToSunDir = normalize(sun - ray.dir);
			float occlusion = clamp(0.05*log(trap.x),0.0,1.0);
			float fakeSSS = clamp(1.0+dot(ray.dir,normal),0.0,1.0);

			// Sun
			float shadow = SoftShadow(fractalToSun, <shadowSoftness>);
			float diffuse = clamp(dot(sun, normal), 0.0, 1.0 ) * shadow;
			float specular = pow(clamp(dot(normal,fractalToSunDir),0.0,1.0), 32.0 )*diffuse*(0.04+0.96*pow(clamp(1.0-dot(fractalToSunDir,sun),0.0,1.0),5.0));

			// Bounce
			float diffuse2 = clamp( 0.5 + 0.5*dot(light, normal), 0.0, 1.0 )*occlusion;

			// Sky
			float diffuse3 = (0.7+0.3*normal.y)*(0.2+0.8*occlusion);

			vec3 light = vec3(0.0); 
			light += 7.0*vec3(1.50,1.10,0.70)*diffuse;
			light += 4.0*vec3(0.25,0.20,0.15)*diffuse2;
			light += 1.5*vec3(0.10,0.20,0.30)*diffuse3;
			light += 2.5*vec3(0.35,0.30,0.25)*(0.05+0.95*occlusion); // ambient
			light += 4*fakeSSS*occlusion;                          // fake SSS


			col *= light;
			col = pow( col, vec3(0.7,0.9,1.0) );                  // fake SSS
			col += specular * 15.;

			// Reflection (?)
			//vec3 reflection = reflect( ray.dir, normal );
			//col += 8.0*vec3(0.8,0.9,1.0)*(0.2+0.8*occlusion)*(0.03+0.97*pow(fakeSSS,5.0))*smoothstep(0.0,0.1,reflection.y )*SoftShadow( Ray(pos+0.01*normal, reflection), 2.0 );
		}

		return col;
	}
</render>

<main>
	vec2 uv = gl_FragCoord.xy / screenSize * 2.0 - 1.0;

	uv.x *= float(screenSize.x) / float(screenSize.y);
	uv *= zoom;

	vec3 direction = normalize(vec3(uv.xy, 1));

	direction *= rotation;
	direction.y *= worldFlip;
	
	
	vec3 col = render(Ray(vec3(position.z, position.y * worldFlip, position.x), direction), uv);

    color = vec4(col.xyz, 1.0);
</main>

<mainAA>
	vec3 col = vec3(0.0);
	for (int i = 0; i < antiAliasing; i++)
	{
		for (int j = 0; j < antiAliasing; j++)
		{
			vec2 frag = gl_FragCoord.xy;
			frag.x += float(i)/antiAliasing;
			frag.y += float(j)/antiAliasing;
			vec2 uv = frag / screenSize * 2.0 - 1.0;
			uv.x *= float(screenSize.x) / float(screenSize.y);
			uv *= zoom;

			vec3 direction = normalize(vec3(uv.xy, 1));

			direction *= rotation;
			direction.y *= worldFlip;
	

			Ray	ray = Ray(vec3(position.z, position.y * worldFlip, position.x), direction);
			col += render(ray, uv);
		}
	}
	col /= float(antiAliasing*antiAliasing);

    color = vec4(col.xyz, 1.0);
</mainAA>