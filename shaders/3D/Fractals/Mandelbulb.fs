<extraSections>
[colorB, false, "", true], [colorC, false, "", true]
</extraSections>

<maxIterations>8</maxIterations>
<maxSteps>60</maxSteps>
<shadowSoftness>4</shadowSoftness> // Higher = harder shadow
<maxIterationsRelease>16</maxIterationsRelease>
<maxStepsRelease>1000</maxStepsRelease>
<antiAliasing>2</antiAliasing>
<zoomDetailRatio>.001</zoomDetailRatio>

<maxDist>5</maxDist>
<maxDistRelease>5</maxDistRelease>

<uniforms>
	uniform float power = 8;
	uniform float genericParameter = 1;
</uniforms>

<include>
	boundingSphere, sphereFold, boxFold, triplexPow
</include>

<operations>
	<sinX>w.x = sin(w.x); w.x = sinh(w.x)*sin(w.x);</sinX>,
	<subXfromY>w.y-=w.x;</subXfromY>,
	<tanY>w.y = tan(w.y);</tanY>,
	<xAtanYX>w.x = atan(w.y, w.x);</xAtanYX>,
	<MulXY>w.y*=w.y;w.x*=w.y;</MulXY>,
	<xMulSinX>w.x *= sin(w.x);</xMulSinX>,
	<mulYYXYYX>w.y*=w.y;w.x*=w.y;w.y*=w.x;</mulYYXYYX>,
</operations>

<color>
	<vec3(0.54,0.3,0.07)>,
	<vec3(0.7,0.7,0.9)>,
</color>

<colorB>
	<vec3(0.02,0.4,0.30)>,
	<vec3(0.6,0.6,0.9)>,
</colorB>

<colorC>
	<vec3(0.15, 0.4, 0.04)>,
	<vec3(0.7,0.7,1)>,
</colorC>

<coloring>
	<col = vec3(0.01);
	col = mix(col, <color>, clamp(trap.y,0.0,1.0)); /*Inner*/
	col = mix(col, <colorB>, clamp(trap.z*trap.z,0.0,1.0));
	col = mix(col, <colorC>, clamp(pow(trap.w,6.0),0.0,1.0)); /*Stripes*/
	col *= 0.5;>,

	<col.x += 1-cos(trap.y*power);
	col.y += 0.8*cos(trap.x*power);
	col.z += sqrt(sin(trap.z*power));>,
</coloring>

<distanceTrapReturn>
	<mandelBulbTrapReturn>vec4(m,trap.yzw);</mandelBulbTrapReturn>
</distanceTrapReturn>

<trace>
	float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
	{
		float res = -1.0;

		//bounding sphere
		vec2 dis = boundingSphere(vec4(0.0,0.0,0.0,1.25), ray.origin, ray.dir);
		if(dis.y < 0.0)
			return -1.0;

		dis.x = max(dis.x, 0.0);
		dis.y = min(dis.y, 10.0);

		// raymarch fractal distance field
		vec4 trap;

		float t = dis.x;
		int i = 0;
		for(; i<maxSteps; i++  )
		{ 
			vec3 pos = ray.origin + ray.dir * t;
			float h = sceneDistance(pos, trap);
			float th = 0.25 * px * t;

			if(t>dis.y || h < th)
			{
				break;
			}
			t += h;
		}

		percentSteps = float(i) / maxSteps;

		if(t < dis.y)
		{
			trapOut = trap;
			res = t;
		}

		return res;
	}
</trace>

<edgeGlow>
	col += vec3(0.556, 0.843, 0.415) * steps * steps;
</edgeGlow>