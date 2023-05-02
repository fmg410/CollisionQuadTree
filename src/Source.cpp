#include "structures/QuadNode.hpp"
#include <array>
#include <iostream>
#include "structures/QuadTree.hpp"

//#define NO_UI

#ifndef NO_UI
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#endif

struct figure{
    float x = 0;
    float y = 0;
};

int main()
{
    //QuadNode<figure, 8> node;
    std::array<int, 5> a;
    a.at(0) = 3;

    //std::cout << a.size() << " " << a.max_size();
    QuadTree<figure> tree;
    /* figure f{12, 3};
    tree.add(f); */
    for(int i = 0; i < 8; i++) // breaks starting at 10
    {
        tree.add(figure{float(i), float(i+5)});
    }
    tree.add(figure{float(8), float(8+5)});
    tree.add(figure{float(-8), float(-8+5)});
    auto sss = tree.locateNodeByPosition(tree.nodes.at(0), 10, 5);
    //std::cout << node.toString();

    /* int tab[5] = {1, 2, 3, 4, 5};
    int* jed = tab;
    int* dwa = tab + 3;
    std::cout << "   " << dwa - jed; */

    int num = 0;
    for(auto& node : tree)
    {
        for(auto itr = node.data.begin(); itr != node.data.begin() + node.elements; itr++)
            std::cout << "Node " << num << ": " << itr->x << " " << itr->y << '\n';
        num++;
        //std::cout << node.data.at(0).x << " " << node.data.at(0).y << '\n';
    }

    #ifndef NO_UI

    sf::RenderWindow window(sf::VideoMode(600, 600), "My window");
    //sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(40.f, 40.f));
    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(30.f, 30.f));
    window.setView(view);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        // window.draw(...);
        sf::RectangleShape center(sf::Vector2f(1.f, 1.f));
        center.setFillColor(sf::Color::Green);
        center.setPosition(sf::Vector2f(0.f, 0.f));
        center.setOrigin(sf::Vector2f(0.5f, 0.5f));
        window.draw(center);

        for(auto& node : tree)
        {
            for(auto itr = node.data.begin(); itr != node.data.begin() + node.elements; itr++)
            {
                sf::RectangleShape rect(sf::Vector2f(1.f, 1.f));
                rect.setPosition(sf::Vector2f(itr->x, itr->y));
                rect.setFillColor(sf::Color::Red);
                rect.setOrigin(sf::Vector2f(0.5f, 0.5f));
                window.draw(rect);
            }
        }

        // end the current frame
        window.display();
    }
    #endif

}