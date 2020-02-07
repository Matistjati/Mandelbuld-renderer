<sceneDistance>
	float sceneDistance(vec3 w, out vec4 resColor)
	{
		return DistanceEstimator(w, resColor);
	}
</sceneDistance>

<sky>
	col += skyColor * (0.6 + 0.4 * direction.y);
</sky>

<sun>
	col += sunSize * sunColor * pow(clamp(dot(direction, sun), 0.0, 1.0), sunSpread);
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
	<defaultSetup>vec3 c = w; float m; vec4 trap = vec4(abs(w),m); float dw = 1.0;</defaultSetup>,
	<mandelBulbInit>vec3 c = w; float m = dot(w,w); vec4 trap = vec4(abs(w),m); float dw = 1.0;</mandelBulbInit>,
	<rotationSetup>
		vec3 phase1 = vec3(0,0,0);
		vec3 phase2 = vec3(0,0,0);

		vec3 sinrot1 = vec3(sin((rot1+phase1)*6.28318530718));
		vec3 cosrot1 = vec3(cos((rot1+phase1)*6.28318530718));
		mat3 rotMat1 = mat3(1,0,0,0,cosrot1.x,-sinrot1.x,0,sinrot1.x,cosrot1.x)*mat3(cosrot1.y,0,sinrot1.y,0,1,0,-sinrot1.y,0,cosrot1.y)*mat3(cosrot1.z,-sinrot1.z,0,sinrot1.z,cosrot1.z,0,0,0,1);
		
		vec3 sinrot2 = vec3(sin((rot2+phase2)*6.28318530718));
		vec3 cosrot2 = vec3(cos((rot2+phase2)*6.28318530718));
		mat3 rotMat2 = mat3(1,0,0,0,cosrot2.x,-sinrot2.x,0,sinrot2.x,cosrot2.x)*mat3(cosrot2.y,0,sinrot2.y,0,1,0,-sinrot2.y,0,cosrot2.y)*mat3(cosrot2.z,-sinrot2.z,0,sinrot2.z,cosrot2.z,0,0,0,1);
	</rotationSetup>,
</distanceSetup>

<operations>
	<boxFold>boxFold(w, foldingLimit);</boxFold>,
	<sphereFold>m = dot(w, w);	sphereFold(w, dw, m, innerRadius, outerRadius);</sphereFold>,
	<triplexPow>w = triplexPow(w, power, dw, m);</triplexPow>,
	<scaleAndTranslate>w=scale*w + c;</scaleAndTranslate>,
	<translate>w+=c;</translate>,
	<mandelBoxDerivative>dw = dw*abs(scale)+1.0;</mandelBoxDerivative>,
	<sinY>w.y = sin(w.y);</sinY>,
	<addLength>w+=length(w);</addLength>,
	<mulW>w*=parameter;</mulW>,
	<addW>w+=parameter;</addW>,
	<addWY>w.y+=parameter;</addWY>,
	<setY>w.y=parameter;</setY>,
	<setXY>w.xy=parameter;</setXY>,
	<planeFold>w-=2 * min(0, dot(w, parameter)) * parameter;</planeFold>,
	<setW>w=parameter;</setW>,
	<SwapXZ>w.xz=w.zx;</SwapXZ>,
	<icosaFold>icosaFold(parameter);</icosaFold>,
	<mengerFold>mengerFold(parameter);</mengerFold>,
	<sierpinskiFold>sierpinskiFold(parameter);</sierpinskiFold>,
	<mandelbulbModded>float r = length(w); float theta = 2 * atan(w.x, w.z); float phi = 1 * acos(w.y / r); parameter; w = r * vec3(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi));</mandelbulbModded>,
</operations>

<distanceReturn>
	<mandelBoxDist>abs(length(w)/abs(dw))</mandelBoxDist>,
	<mandelBulbDist>abs(0.25* log(m)*sqrt(m)/dw)</mandelBulbDist>,
	<mandelFoldDist>(length(w)-distDiff)*pow(scale,-i)</mandelFoldDist>,
	<mandelFoldDistParam>(length(w)-distDiff)*pow(parameter,-i)</mandelFoldDistParam>,
</distanceReturn>

<distanceTrap>
	<defaultTrap>m = dot(w,w);trap = min(trap, vec4(abs(w),m));</defaultTrap>,
</distanceTrap>

<distanceTrapReturn>
	<defaultTrapReturn>trap;</defaultTrapReturn>,
</distanceTrapReturn>

<distanceBreakCondition>
	<none></none>,
	<defaultBreak>if (m > escapeRadius) break;</defaultBreak>,
</distanceBreakCondition>

<distanceEstimator>
float DistanceEstimator(vec3 w, out vec4 resColor)
{
	<distanceSetup>

	float i;
	for(i = 0; i < maxIterations; i++)
	{
		<distanceBody>

		<distanceExtraOperations>

		<distanceTrap>

		<distanceBreakCondition>
	}

	resColor = <distanceTrapReturn>

	return <distanceReturn>;
}
</distanceEstimator>


<trace>
#define LinneaRetarded 0
	// Compute the intersection of the fractal and a given ray parameterised by a starting point and a direction
	float trace(vec3 origin, vec3 direction, out vec4 trapOut, float px, out float percentSteps, out bool hitSurface)
	{
		float res = -1;
		vec4 trap;

		float t = 0;
		float h = 0;
		int i = 0;
		float th = 0;
		for(; i<maxSteps; i++)
		{ 
			vec3 pos = origin + direction * t;
			h = fudgeFactor * sceneDistance(pos, trap);
			th = 0.25 * px * t;

			if (h < th || h > maxDist)
			{
				break;
			}
			t += h;
		}

		percentSteps = float(i)/float(maxSteps);

		trapOut = trap;

		hitSurface = h < th;

#if LinneaRetarded
		if (t < maxDist && !fogColoring)
#else
		if (h < maxDist && !fogColoring)
#endif
		{
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

	float SoftShadow(vec3 origin, vec3 direction, float k)
	{
		float result = 1.0;
		float t = 0.0;
	
		vec4 temp;

		for(int i = 0; i < maxSteps; i++)
		{
			float h = sceneDistance(origin + direction * t, temp);
			result = min(result, k * h / t);

			if(result < 0.001) break;

			t += clamp(h, 0.01, 32);
		}
		return clamp(result, 0.0, 1.0);
	}
</lightingFunctions>

<render>
	vec3 render(vec3 origin, vec3 direction, vec2 uv)
	{
		float px = (100/screenSize.y) * zoom * zoomDetailRatio;
		vec4 trap;
		float steps;
		bool hitSurface = false;

		float t = trace(origin, direction, trap, px, steps, hitSurface);

		vec3 col = vec3(0);

		
		vec3 sun = normalize(vec3(sin(time * 0.25),
							  abs(sin(time * 0.1)),
								  cos(time * 0.25)));

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
			vec3 pos = (origin + direction * t);

			vec3 normal = calculateNormal(pos);

			vec3 fractalToSunDir = normalize(sun - direction);
			float occlusion = clamp(0.05*log(trap.x),0.0,1.0);
			float fakeSSS = clamp(1.0+dot(direction, normal),0.0,1.0);

			// Sun
			float shadow = SoftShadow(pos + 0.001 * normal, fractalToSunDir, shadowSoftness);
			float diffuse = clamp(dot(sun, normal), 0.0, 1.0) * shadow * min(3,0.5/steps);
			float specular = pow(clamp(dot(normal,fractalToSunDir),0.0,1), 32.0 )*diffuse*(0.04+0.96*pow(clamp(1.0-dot(fractalToSunDir,sun),0.0,1.0),5.0));

			// Bounce
			float diffuse2 = clamp( 0.5 + 0.5*dot(light, normal), 0.0, 1.0 )*occlusion;

			// Sky
			float diffuse3 = (0.7+0.3*normal.y)*(0.2+0.8*occlusion);

			vec3 light = vec3(0.0); 
			diffuse *= specularStrength;
			light += 7.0*vec3(1.50,1.10,0.70)*diffuse;
			light += 4.0*vec3(0.25,0.20,0.15)*diffuse2;
			light += 1.5*vec3(0.10,0.20,0.30)*diffuse3;
			light += 2.5*vec3(0.35,0.30,0.25)*(0.05+0.95*occlusion); // ambient
			light += 4*fakeSSS*occlusion;                            // fake SSS
			light = pow(light, vec3((light.x<1) ? 1 : 1.5));

			light = pow(light, 1/vec3(shadowDarkness));
			col *= light;
			col = pow( col, vec3(0.7,0.9,1.0) );                  // fake SSS
			col += specular;

			// Reflection (?)
			//vec3 reflection = reflect( ray.dir, normal );
			//col += 8.0*vec3(0.8,0.9,1.0)*(0.2+0.8*occlusion)*(0.03+0.97*pow(fakeSSS,5.0))*smoothstep(0.0,0.1,reflection.y )*SoftShadow( Ray(pos+0.01*normal, reflection), 2.0 );
		}

		return pow(col, vec3(gamma));
	}
</render>

<main>
	int AA = int(antiAliasing);
	vec3 col = vec3(0.0);

	for (int i = 0; i < AA; i++)
	{
		for (int j = 0; j < AA; j++)
		{
			vec2 frag = gl_FragCoord.xy;
			frag += vec2(float(i),float(j))/float(AA);
			vec2 uv = frag / screenSize * 2.0 - 1.0;
			uv.x *= float(screenSize.x) / float(screenSize.y);
			uv *= zoom;

			vec3 direction = normalize(vec3(uv.xy, -1));

			direction *= rotation;
			
			col += render(position, direction, uv*zoom);
		}
	}
	col /= AA*AA;

    color = vec4(col.xyz, 1.0);
</main>
