#pragma once
#include "pos.hpp"
#include <cmath>

/* struct Figure{
    float x = 0;
    float y = 0;
    float velX = 0.1f;
    float velY = 0.1f;
    bool operator==(const Figure& f)
    {
        return x == f.x && y == f.y;
    }
}; */

unsigned long getNextId()
{
    static unsigned long currentId = 1;
    return currentId++;
}

template <unsigned int N>
struct Figure{
    float x = 0;
    float y = 0;
    float velX = 0.1f;
    float velY = 0.1f;
    bool collisionChecked = false;

    Figure(float _x, float _y, float _velX, float _velY, float scale = 2.f)
    : x(_x)
    , y(_y)
    , velX(_velX)
    , velY(_velY)
    , id(getNextId())
    {
        float fTheta = 3.14159f * 2.0f / N;
        for (int i = 0; i < N; i++)
		{
			model[i] = { scale * std::cos(fTheta * i), scale * std::sin(fTheta * i) };
			calculatedPoints[i] = { scale * std::cos(fTheta * i), scale * std::sin(fTheta * i) };
		}
    }

    unsigned long getId() const
    {
        return id;
    }

    void update()
    {
        for (int i = 0; i < N; i++)
				calculatedPoints[i] =
				{
					(model[i].x * std::cos(angle)) - (model[i].y * std::sin(angle)) + x,
					(model[i].x * std::sin(angle)) + (model[i].y * std::cos(angle)) + y
				};
    }

    static constexpr size_t size()
    {
        return N;
    }

    size_t getSize() const
    {
        return N;
    }

    auto begin()
    {
        return calculatedPoints;
    }

    auto end()
    {
        return calculatedPoints + N;
    }

    auto cbegin() const
    {
        return calculatedPoints;
    }

    auto cend() const
    {
        return calculatedPoints + N;
    }

    void increaseAngle(float angle)
    {
        this->angle += angle;
        while(angle >= 360.f)
            angle -= 360.f;
        while(angle < 0.f)
            angle += 360.f;
    }

    Figure()
    : Figure(0, 0, 0, 0)
    {}

    bool operator==(const Figure& f)
    {
        return x == f.x && y == f.y;
    }
    bool operator!=(const Figure& f)
    {
        return !(*this == f);
    }

    float getR()
    {
        pos p1 = calculatedPoints[0];
        pos p2 = calculatedPoints[1];
        float x = p1.x - p2.x;
        float y = p1.y - p2.y;
        float r = sqrtf(x * x + y * y);
        return r;
    }

private:
    pos calculatedPoints[N];
    pos model[N];
    float angle = 0.0f;
    unsigned long id = 0;

};
