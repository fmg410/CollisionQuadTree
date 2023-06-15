#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>

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
        for(int i = 0; i < T::size(); i++)
        {
            m_vertices[i].position = sf::Vector2f((figure.cbegin() + i)->x, (figure.cbegin() + i)->y);
            m_vertices[i].color = color;
        }
        /* m_vertices[T::size() + 1].position = sf::Vector2f((figure.cbegin())->x, (figure.cbegin())->y);
        m_vertices[T::size() + 1].color = color; */
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        states.texture = nullptr;

        target.draw(m_vertices, T::size(), sf::TriangleFan, states);
    }

    sf::Vertex m_vertices[T::size()];
    sf::Texture m_texture;
};