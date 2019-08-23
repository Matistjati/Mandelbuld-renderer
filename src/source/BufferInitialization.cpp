#include "headers/BufferInitialization.h"
#include <iostream>
#include <random>
#include <chrono>
#include <functional>

bool notInMainCardioid(float zx, float zy)
{
	float cx = zx - 0.25f;

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
	const float dxy = 0.01f;
	std::vector<glm::vec4> goodPoints;
	for (int x = 0; x < screenSize.x; x++)
	{
		for (int y = 0; y < screenSize.y; y++)
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
				if (good)
				{
					goodPoints.push_back(glm::vec4(cx, cy, i, good));
				}
			}
		}
	}

	std::mt19937::result_type seed = (std::mt19937::result_type)std::chrono::high_resolution_clock::now().time_since_epoch().count();
	auto image_rand_index = std::bind(std::uniform_int_distribution<int>(0, goodPoints.size()-1),
		std::mt19937(seed));
	for (size_t i = 0; i < data.size(); i++)
	{
		int index = image_rand_index();
		data[i] = goodPoints[index];
	}
}

std::map<std::string, InitFunction> BufferInitialization::functions = { {"buddhaBrotImportanceMap", buddhaBrotImportanceMap} };