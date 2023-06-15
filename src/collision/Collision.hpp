#pragma once
#include "pos.hpp"
#include <limits>
#include <algorithm>

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
				pos axisProj = { -(poly1->calculatedPoints[b].y - poly1->calculatedPoints[a].y), poly1->calculatedPoints[b].x - poly1->calculatedPoints[a].x };

				// Optional normalisation of projection axis enhances stability slightly
				//float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
				//axisProj = { axisProj.x / d, axisProj.y / d };

				// Work out min and max 1D points for r1
				float min_r1 = std::numeric_limits<float>::max(), max_r1 = -std::numeric_limits<float>::max();
				for (int p = 0; p < poly1->size(); p++)
				{
					float q = (poly1->calculatedPoints[p].x * axisProj.x + poly1->calculatedPoints[p].y * axisProj.y);
					min_r1 = std::min(min_r1, q);
					max_r1 = std::max(max_r1, q);
				}

				// Work out min and max 1D points for r2
				float min_r2 = std::numeric_limits<float>::max(), max_r2 = -std::numeric_limits<float>::max();
				for (int p = 0; p < poly2->size(); p++)
				{
					float q = (poly2->calculatedPoints[p].x * axisProj.x + poly2->calculatedPoints[p].y * axisProj.y);
					min_r2 = std::min(min_r2, q);
					max_r2 = std::max(max_r2, q);
				}

				// Calculate actual overlap along projected axis, and store the minimum
				overlap = std::min(std::min(max_r1, max_r2) - std::max(min_r1, min_r2), overlap);

				if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
					return false;
			}
		}

		// If we got here, the objects have collided, we will displace r1
		// by overlap along the vector between the two object centers
		pos d = { figure2.x - figure1.x, figure2.y - figure1.y };
		float s = sqrtf(d.x*d.x + d.y*d.y);
		figure1.x -= overlap * d.x / s;
		figure1.y -= overlap * d.y / s;
		return false;
	}