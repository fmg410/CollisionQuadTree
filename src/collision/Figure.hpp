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

const float MAX_VEL = 6.f;

unsigned long getNextId()
{
    static unsigned long currentId = 1;
    return currentId++;
}

float figDistance(pos a, pos b)
{
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool broadCheck(const pos& a, const pos& b, float R1, float R2)
{
    return figDistance(a, b) < R1 + R2;
}

template <unsigned int N>
struct Figure{
    float x = 0.f;
    float y = 0.f;
    float velX = 0.1f;
    float velY = 0.1f;
    bool collisionChecked = false;
    float displacementX = 0.0f;
    float displacementY = 0.0f;
    float changeVelocityX = 0.0f;
    float changeVelocityY = 0.0f;
    float R = 0.0f;


    // DEBUG...
    static constexpr unsigned int maxIds = 7;
    long lastCollisionIds[maxIds] = {0, };
    unsigned int lastCollisionIndex = 0;

    void addCollisionId(unsigned long id)
    {
        lastCollisionIds[lastCollisionIndex] = id;
        lastCollisionIndex = (lastCollisionIndex + 1) % maxIds;
    }

    void resetCollisionIds()
    {
        for(int i = 0; i < maxIds; i++)
            lastCollisionIds[i] = 0;
        lastCollisionIndex = 0;
    }

    // ...DEBUG

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
        calculateRadious();
    }

    void displace()
    {
        displacementX = std::max(-R, std::min(R, displacementX));
        displacementY = std::max(-R, std::min(R, displacementY));
        x += displacementX;
        y += displacementY;
        // resetDisplacement();
    }

    void resetDisplacement()
    {
        displacementX = 0.f;
        displacementY = 0.f;
    }

    void changeVelocity()
    {
        velX += changeVelocityX;
        velY += changeVelocityY;
        velX = std::max(-MAX_VEL, std::min(MAX_VEL, velX));
        velY = std::max(-MAX_VEL, std::min(MAX_VEL, velY));
        //resetVelocityChange();
    }

    void resetVelocityChange()
    {
        changeVelocityX = 0.f;
        changeVelocityY = 0.f;
    }

    pos getPosition() const
    {
        return {x, y};
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

    float msq(float x, float y)
    {
        return x * x + y * y;
    }

    void calculateRadious()
    {
        pos center; 
        for (int i = 0; i < N; i++)
        {
            center.x = center.x + calculatedPoints[i].x;
            center.y = center.y + calculatedPoints[i].y;
        }
        center.x = center.x * (1.0f / (float)N);
        center.y = center.y * (1.0f / (float)N);
        float radius = msq(center.x - calculatedPoints[0].x, center.y - calculatedPoints[0].y);
        for (int i = 1; i < N; i++)
        {
            float distance = msq(center.x - calculatedPoints[i].x, center.y - calculatedPoints[i].y);
            if (distance > radius)
                radius = distance;
        }
        R = sqrtf(radius) * 1.1f;
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

public:
    pos calculatedPoints[N];
    pos model[N];
    float angle = 0.0f;
    unsigned long id = 0;

};


template <unsigned int N>
bool broadCheck(const Figure<N>& a, const Figure<N>& b)
{
    return broadCheck({a.x, a.y}, {b.x, b.y}, a.R, b.R);
}