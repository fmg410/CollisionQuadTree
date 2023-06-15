#pragma once
#include "pos.hpp"
#include <limits>
#include <algorithm>
#include <cmath>

template <typename T>
bool collide(T& figure1, T& figure2)
	{
		T *poly1 = &figure1;
		T *poly2 = &figure2;

		float overlap = std::numeric_limits<float>::max();

		for (int shape = 0; shape < 2; shape++)
		{
			if (shape == 1)
			{
				poly1 = &figure2;
				poly2 = &figure1;
			}

			for (int a = 0; a < poly1->size(); a++)
			{
				int b = (a + 1) % poly1->size();
				pos axisProj = { -((poly1->begin() + b)->y - (poly1->begin() + a)->y), (poly1->begin() + b)->x - (poly1->begin() + a)->x };

				float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
				axisProj = { axisProj.x / d, axisProj.y / d };

				float min_r1 = std::numeric_limits<float>::max(), max_r1 = std::numeric_limits<float>::min();
				for (int p = 0; p < poly1->size(); p++)
				{
					float q = ((poly1->begin() + p)->x * axisProj.x + (poly1->begin() + p)->y * axisProj.y);
					min_r1 = std::min(min_r1, q);
					max_r1 = std::max(max_r1, q);
				}

				float min_r2 = std::numeric_limits<float>::max(), max_r2 = std::numeric_limits<float>::min();
				for (int p = 0; p < poly2->size(); p++)
				{
					float q = ((poly2->begin() + p)->x * axisProj.x + (poly2->begin() + p)->y * axisProj.y);
					min_r2 = std::min(min_r2, q);
					max_r2 = std::max(max_r2, q);
				}

				overlap = std::min(std::min(max_r1, max_r2) - std::max(min_r1, min_r2), overlap);

				if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
					return false;
			}
		}

		pos d = { figure2.x - figure1.x, figure2.y - figure1.y };
		float s = sqrtf(d.x*d.x + d.y*d.y);
		figure1.x -= overlap * d.x / s;
		figure1.y -= overlap * d.y / s;
		std::swap(figure1.velX, figure1.velY);
		std::swap(figure1.velX, figure2.velY);
		figure1.velX *= -1;
		figure1.velY *= -1;
		figure2.velX *= -1;
		figure2.velY *= -1;
		return false;
	}