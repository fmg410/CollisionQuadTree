#pragma once
#include <cmath>



template <typename T>
void applyGravity(T& object, float g = 0.001f, float modifier = 1.f)
{
    object.velY += g * modifier;
}

template <typename T>
void updateSpeed(T& object, float modifier = 1.f)
{
    //applyGravity(object, modifier);
    object.x += object.velX * modifier;
    object.y += object.velY * modifier;
    #ifdef ROTATE_FIGURES
    object.increaseAngle((rand() % 11 / 50.f) * modifier);
    #endif
}

template <typename T>
void applyBoundaries(T& object, float xMin, float xMax, float yMin, float yMax)
{

    for(auto& vertex : object)
    {
        object.update();
        if(vertex.x < xMin)
        {
            object.x += xMin - vertex.x;
            object.velX = fabs(object.velX);
        }
        else if(vertex.x > xMax)
        {
            object.x -= vertex.x - xMax;
            object.velX = fabs(object.velX) * -1;
        }
        if(vertex.y < yMin)
        {
            object.y += yMin - vertex.y;
            object.velY = fabs(object.velY);
        }
        else if(vertex.y > yMax)
        {
            object.y -= vertex.y - yMax;
            object.velY = fabs(object.velY) * -1;
            /* object.velY = 0; // przy grawitacji */
        }
    }
    object.update();
    /* if(object.x < xMin)
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
    } */
}

template <typename T>
void applyBoundariesNode(T& object, float x, float y, float width, float height)
{
    applyBoundaries(object, x - width / 2, x + width / 2, y - height / 2, y + height / 2);
}