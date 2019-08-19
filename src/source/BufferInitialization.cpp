#include "headers/BufferInitialization.h"

bool notInMainCardioid(float zx, float zy)
{
	float cx = zx - 0.25;

	float q = cx*cx+zy*zy;
	return q * (q + (cx)) > 0.25f * zy * zy;
}

bool notInMainBulb(float zx, float zy)
{
	zx += 1;
	return zx*zx+zy*zy > 0.062499999f;
}

void buddhaBrotImportanceMap(std::vector<glm::vec4> &data, glm::ivec2 screenSize, std::vector<float> params)
{
	for (size_t x = 0; x < screenSize.x; x++)
	{
		for (size_t y = 0; y < screenSize.y; y++)
		{
			float maxIter = params[0];
			// Map into fractal space
			float wx = float(x) / screenSize.x;
			float wy = float(y) / screenSize.y;
			wx = wx * (params[4] - params[2]) + params[2];
			wy = wy* (params[5] - params[3]) + params[3];

			if (notInMainBulb(wx, wy) && notInMainCardioid(wx, wy))
			{
				float cx = wx;
				float cy = wy;

				float i = 0;
				for (; i < maxIter; i++)
				{
					float tempX = wx * wx - wy * wy;
					wy = 2 * wy * wx + cy;
					wx = tempX + cx;
					
					if (wx * wx + wy * wy > 4)
					{
						break;
					}
				}
				float good = i > params[1] && i != params[0];
				
				data[x * screenSize.y + y] = glm::vec4(glm::vec2(cx, cy), i / params[0], good);
			}
		}
	}
}

std::map<std::string, InitFunction> BufferInitialization::functions = { {"buddhaBrotImportanceMap", buddhaBrotImportanceMap} };