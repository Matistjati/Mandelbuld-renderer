<sceneDistance>
	float sceneDistance(vec3 w, out vec4 resColor)
	{
		return DistanceEstimator(w, resColor);
	}
	
	float sceneDistance(vec3 w)
	{
		vec4 temp;
		return DistanceEstimator(w, temp);
	}
</sceneDistance>

<sky>
	col += skyColor * (0.6 + 0.4 * direction.y);
</sky>

<sun>
	col += sunSize * sunColor * pow(clamp(dot(direction, Sun), 0.0, 1.0), sunSpread);
</sun>

<buffers>
	/*<bufferType>imageBuffer</bufferType>*/
	/*<shouldBeCleared>button, resetFrame, onUniformChange: [position, rotation, zoom, pathTrace, maxWaterDist, cloudAmount, surfaceColor, bumpFactor, waterHeight, brightness]</shouldBeCleared>*/
	layout(std430, binding = 4) buffer PathTracedImage
	{
		vec4 image[];
	};
</buffers>

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
		for(; i < maxSteps; i++)
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

		hitSurface = h < th && i < maxSteps;

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
	
		for(int i = 0; i < maxSteps; i++)
		{
			float h = sceneDistance(origin + direction * t);
			result = min(result, k * h / t);

			if(result < 0.001) break;

			t += clamp(h, 0.01, 32.);
		}
		return clamp(result, 0.0, 1.0);
	}
</lightingFunctions>

<render>
	vec2 rayBoxDst(vec3 boundsMin, vec3 boundsMax, vec3 rayOrigin, vec3 invRaydir)
	{
		// Adapted from: http://jcgt.org/published/0007/03/04/
		vec3 t0 = (boundsMin - rayOrigin) * invRaydir;
		vec3 t1 = (boundsMax - rayOrigin) * invRaydir;
		vec3 tmin = min(t0, t1);
		vec3 tmax = max(t0, t1);
				
		float dstA = max(max(tmin.x, tmin.y), tmin.z);
		float dstB = min(tmax.x, min(tmax.y, tmax.z));

		// CASE 1: ray intersects box from outside (0 <= dstA <= dstB)
		// dstA is dst to nearest intersection, dstB dst to far intersection

		// CASE 2: ray intersects box from inside (dstA < 0 < dstB)
		// dstA is the dst to intersection behind the ray, dstB is dst to forward intersection

		// CASE 3: ray misses box (dstA > dstB)

		float dstToBox = max(0, dstA);
		float dstInsideBox = max(0, dstB - dstToBox);
		return vec2(dstToBox, dstInsideBox);
	}

	float cloudNoise(vec3 pos)
	{
		float a;
		float b;
		float c;
		a = 1-cellular(pos, noiseScaleLarge.x * noiseScaleSmall.x);
		b = 1-cellular(pos, noiseScaleLarge.y * noiseScaleSmall.y);
		c = 1-cellular(pos, noiseScaleLarge.z * noiseScaleSmall.z);
		float f = a + (b * persistence) + (c * persistence * persistence);
		return max(0, f - densityThreshold) * densityLevel;
	}

	float SampleCloudDensity(vec3 ro, vec3 rd)
	{
		vec2 intersect = rayBoxDst(boxPos-boxWidth*0.5, boxPos+boxWidth*0.5, ro, 1/rd);
		float totalDensity = 0;
		for (float t = intersect.x; t < intersect.y; t+=stepSize)
		{
			totalDensity += cloudNoise(ro+rd*t) * stepSize;
		}
		float transmittance = exp(-totalDensity)*cloudBrightness;
		return transmittance;
	}

	float shadow(in vec3 ro, in vec3 rd)
	{
		float res = 0.0;
	
		float maxDist = 12.0;
	
		float t = 0.001;
		for (int i = 0; i < maxSteps; i++)
		{
			float h = sceneDistance(ro+rd*t);
			if(h < 0.0001 || t > maxDist) break;
			t += h;
		}

		if (t > maxDist) res = 1.0;// + pow(clamp(dot(ro, rd),0,1),5);
		


		return res*SampleCloudDensity(ro, rd);
	}

	float hash(float seed)
	{
		return fract(sin(seed)*43758.5453);
	}

	vec3 cosineDirection( in float seed, in vec3 nor)
	{
		float u = hash( 78.233 + seed);
		float v = hash( 10.873 + seed);

	
		// Method 1 and 2 first generate a frame of reference to use with an arbitrary
		// distribution, cosine in this case. Method 3 (invented by fizzer) specializes 
		// the whole math to the cosine distribution and simplfies the result to a more 
		// compact version that does not depend on a full frame of reference.

		#if 0
			// method 1 by http://orbit.dtu.dk/fedora/objects/orbit:113874/datastreams/file_75b66578-222e-4c7d-abdf-f7e255100209/content
			vec3 tc = vec3( 1.0+nor.z-nor.xy*nor.xy, -nor.x*nor.y)/(1.0+nor.z);
			vec3 uu = vec3( tc.x, tc.z, -nor.x );
			vec3 vv = vec3( tc.z, tc.y, -nor.y );

			float a = 6.2831853 * v;
			return sqrt(u)*(cos(a)*uu + sin(a)*vv) + sqrt(1.0-u)*nor;
		#endif
		#if 0
			// method 2 by pixar:  http://jcgt.org/published/0006/01/01/paper.pdf
			float ks = (nor.z>=0.0)?1.0:-1.0;     //do not use sign(nor.z), it can produce 0.0
			float ka = 1.0 / (1.0 + abs(nor.z));
			float kb = -ks * nor.x * nor.y * ka;
			vec3 uu = vec3(1.0 - nor.x * nor.x * ka, ks*kb, -ks*nor.x);
			vec3 vv = vec3(kb, ks - nor.y * nor.y * ka * ks, -nor.y);
	
			float a = 6.2831853 * v;
			return sqrt(u)*(cos(a)*uu + sin(a)*vv) + sqrt(1.0-u)*nor;
		#endif
		#if 1
			// method 3 by fizzer: http://www.amietia.com/lambertnotangent.html
			float a = 6.2831853 * v;
			u = 2.0*u - 1.0;
			return normalize( nor + vec3(sqrt(1.0-u*u) * vec2(cos(a), sin(a)), u) );
		#endif
	}

	
	vec3 uniformVector( in float seed)
	{
		float a = 3.141593*hash( 78.233 + seed);
		float b = 6.283185*hash( 10.873 + seed);
		return vec3( sin(b)*sin(a), cos(b)*sin(a), cos(a) );
	}

	bool intersectPlane(vec3 ro, vec3 rd, float height, out float dist)
	{	
		if (rd.y==0.0)
		{
			return false;
		}
		
		float d = -(ro.y - height)/rd.y;
		d = min(100000.0, d);
		if( d > 0. )
		{
			dist = d;
			return true;
		}
		return false;
	}

	float hash(vec3 p)  // replace this by something better
	{
		p  = fract( p*0.3183099+.1 );
		p *= 17.0;
		return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
	}

	vec3 calculateColor(vec3 ro, vec3 direction, float seed)
	{
		vec3 Sun = sun;
		vec3 sunCol = 3.0*sunColor;
		vec3 skyCol =  4.0*skyColor;
		float px = (100/screenSize.y) * zoom * zoomDetailRatio;
		const float epsilon = 0.0001;

		vec3 col;
		vec3 colorMask = vec3(1);
		vec3 accumulatedColor = vec3(0.0);

		float fdis = 0.0;

		vec4 trap;
		float steps;
		bool hitSurface;
		float t = trace(ro, direction, trap, px, steps, hitSurface);

		// Water intersection and reflection
		float dist;
		bool intersects = intersectPlane(ro, direction, waterHeight, dist);
		bool aboveWater = (ro + direction*t).y > waterHeight;

		// With unsifficient raymarching steps, we will we unable to make a perfect deduction about the state of the ray
		// The following approach suffers in form of incorrectly labelling some of the water around the fractal as part of the real fractal
		// This is the least punishing case, as the fractal will still be reflected on the water, thus we will most likely be able to get away with it
		// Case A: Hit the fractal but we are under water. Case B: Did not hit the fractal, still above water after marching but will intersect with water. 
		// Stored as variable for future info
		bool reflected = dist < maxWaterDist && (hitSurface && !aboveWater) || ((!hitSurface && aboveWater) && intersects);
		if (reflected)
		{
			col = vec3(0.,0,0.05);
			ro = ro + direction*dist;

			const float bumpDistance = 200;
			const float epsilon = 0.2;


			float bumpfactor = bumpFactor * (1. - smoothstep( 0., bumpDistance, dist));

			vec2 coord = ro.xz+time*0.1;
			vec2 step = vec2(epsilon, 0.);

			vec3 nor = vec3(0.,1.,0.);
			nor.x = -bumpfactor * (cellular(coord + step.xy) - cellular(coord - step.xy)) / (2. * epsilon);
			nor.z = -bumpfactor * (cellular(coord + step.yx) - cellular(coord - step.yx)) / (2. * epsilon);
			nor = normalize(nor);
			direction = reflect(direction, nor);
		}

		for(int bounce = 0; bounce < bounces; bounce++) // bounces of GI
		{
			//rd = normalize(rd);
	   
			//-----------------------
			// trace
			//-----------------------
			
			t = trace(ro, direction, trap, px, steps, hitSurface);

			if(!hitSurface)
			{
				// Clouds
				float transmittance = SampleCloudDensity(ro, direction);

				<sky>
			
				<sun>

				<edgeGlow>

				return col*transmittance;
			}

			if( bounce==0 ) fdis = t;

			vec3 pos = ro + direction * t;
			vec3 nor = calculateNormal(pos);

			//-----------------------
			// add direct lighitng
			//-----------------------
			colorMask *= surfaceColor;
			<coloring>
			col*=brightness;

			vec3 light = vec3(0.0);

			// light 1
			float sunDif =  max(0.0, dot(Sun, nor));
			float sunSha = 1.0; if( sunDif > 0.00001 ) sunSha = shadow( pos + nor*epsilon, Sun);
			light += sunCol * sunDif * sunSha;

			// light 2
			vec3 skyPoint = cosineDirection( seed + 7.1*float(frame) + 5681.123 + float(bounce)*92.13, nor);
			float skySha = shadow( pos + nor*epsilon, skyPoint);
			light += skyCol * skySha;


			accumulatedColor += colorMask * col * light;

			//-----------------------
			// calculate new ray
			//-----------------------
			float isDif = 0.8;
			if( hash(seed + 1.123 + 7.7*float(bounce)) < isDif )
			{
			   direction = cosineDirection(76.2 + 73.1*float(bounce) + seed + 17.7*float(frame), nor);
			}
			else
			{
				float glossiness = 0.2;
				direction = normalize(reflect(direction, nor)) + uniformVector(seed + 111.123 + 65.2*float(bounce)) * glossiness;
			}

			ro = pos;
	   }

	   return accumulatedColor;
	}

	vec3 render(vec3 origin, vec3 direction, vec2 uv)
	{
		float px = (100/screenSize.y) * zoom * zoomDetailRatio;
		vec4 trap;
		float steps;
		bool hitSurface = false;

		float t = trace(origin, direction, trap, px, steps, hitSurface);

		vec3 col = vec3(0);

		
		vec3 Sun = sun;

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

			vec3 fractalToSunDir = normalize(Sun - direction);
			float occlusion = clamp(0.05*log(trap.x),0.0,1.0);
			float fakeSSS = clamp(1.0+dot(direction, normal),0.0,1.0);

			// Sun
			float shadow = SoftShadow(pos + 0.001 * normal, fractalToSunDir, shadowSoftness);
			float diffuse = clamp(dot(Sun, normal), 0.0, 1.0) * shadow * min(3,0.5/steps);
			float specular = pow(clamp(dot(normal,fractalToSunDir),0.0,1.), 32.0 )*diffuse*(0.04+0.96*pow(clamp(1.0-dot(fractalToSunDir,Sun),0.0,1.0),5.0));

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

	if (pathTrace)
	{
		uint hash = uint(intHash(intHash(abs(int(frame))+intHash(int(gl_FragCoord.x)))*intHash(int(gl_FragCoord.y))));
		vec2 frag = gl_FragCoord.xy;
		// Anti aliasing
		frag += hash2(hash, hash)*2-1;
		vec2 uv = frag / screenSize * 2.0 - 1.0;
		uv.x *= float(screenSize.x) / float(screenSize.y);
		uv *= zoom;

		vec3 direction = normalize(vec3(uv.xy, -1));

		direction *= rotation;

		float seed = float(hash)/float(0xffffffffU);
		col = calculateColor(position, direction, seed);

		// gl_FragCoord is in the range [0.5, screenSize+0.5], so we subtract to get to [0, screenSize]
		vec2 pos = gl_FragCoord.xy-vec2(0.5);
		int index = int(pos.y*screenSize.x+pos.x); 
		image[index] += vec4(col, 1);
		col = pow(image[index].xyz / frame, vec3(gamma));
	}
	else
	{
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
	}
	
	color = vec4(col, 1);
</main>
