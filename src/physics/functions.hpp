#pragma once




template <typename T>
void applyGravity(T& object, float g = 10.f)
{
    object.velocity.y += g;
}

template <typename T>
void updateSpeed(T& object)
{
    object.x += object.velX;
    object.y += object.velY;
}

template <typename T>
void applyBoundaries(T& object, float xMin, float xMax, float yMin, float yMax)
{
    if(object.x < xMin)
    {
        object.x = xMin;
        object.velX *= -1;
    }
    else if(object.x > xMax)
    {
        object.x = xMax;
        object.velX *= -1;
    }
    if(object.y < yMin)
    {
        object.y = yMin;
        object.velY *= -1;
    }
    else if(object.y > yMax)
    {
        object.y = yMax;
        object.velY *= -1;
    }
}

template <typename T>
void applyBoundariesNode(T& object, float x, float y, float width, float height)
{
    applyBoundaries(object, x - width / 2, x + width / 2, y - height / 2, y + height / 2);
}