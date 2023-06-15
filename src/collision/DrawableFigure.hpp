#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>

/* struct figure{
    float x = 0;
    float y = 0;
    float velX = 0.1f;
    float velY = 0.1f;
    bool operator==(const figure& f)
    {
        return x == f.x && y == f.y;
    }
}; */


template <typename T>
struct DrawableFigure : sf::Drawable, sf::Transformable
{
    float x = 0;
    float y = 0;
    float velX = 0.1f;
    float velY = 0.1f;
    bool operator==(const DrawableFigure& f)
    {
        return x == f.x && y == f.y;
    }
    public:

    DrawableFigure(const T& figure, sf::Color color = sf::Color::Red)
    {
        m_vertices[0].position = sf::Vector2f(figure.x, figure.y);
        m_vertices[0].color = color;
        for(int i = 0; i < T::size() + 1; i++)
        {
            m_vertices[i].position = sf::Vector2f((figure.cbegin() + i)->x, (figure.cbegin() + i)->y);
            m_vertices[i + 1].color = color;
        }
    }
    // add functions to play with the entity's geometry / colors / texturing...

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the entity's transform -- combine it with the one that was passed by the caller
        states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

        // apply the texture
        states.texture = nullptr;

        // you may also override states.shader or states.blendMode if you want

        // draw the vertex array
        target.draw(m_vertices, T::size(), sf::TriangleFan, states);
    }

    sf::Vertex m_vertices[T::size() + 1];
    sf::Texture m_texture;
};