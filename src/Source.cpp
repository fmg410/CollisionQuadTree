#include "structures/QuadNode.hpp"
#include <array>
#include <iostream>
#include "structures/QuadTree.hpp"

//#define NO_UI

#ifndef NO_UI
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#endif

static const float VIEW_HEIGHT = 600.f;

void resizeView(const sf::RenderWindow& window, sf::View& view)
{
	float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
	view.setSize(VIEW_HEIGHT * aspectRatio, VIEW_HEIGHT);
}

void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, float zoom, sf::View& view, int delta)
{
	// delta < 0 -> zoom out
	// zoom > 0 -> zoom out
	if (delta > 0)
	{
		const sf::Vector2f beforeCoord{ window.mapPixelToCoords(pixel) };
		view.zoom(1.0f / zoom);
		window.setView(view);
		const sf::Vector2f afterCoord{ window.mapPixelToCoords(pixel) };
		const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
		view.move(offsetCoords);
		window.setView(view);
	}
	else if (delta < 0)
	{
		view.zoom(zoom);
	}
}
struct figure{
    float x = 0;
    float y = 0;
    bool operator==(const figure& f)
    {
        return x == f.x && y == f.y;
    }
};

int main() // TODO: update member (first find member...)
{
    //QuadNode<figure, 8> node;
    std::array<int, 5> a;
    a.at(0) = 3;

    //std::cout << a.size() << " " << a.max_size();
    QuadTree<figure> tree;
    /* figure f{12, 3};
    tree.add(f); */
    /* for(int i = 0; i < 8; i++) // breaks starting at 10
    {
        tree.add(figure{float(i), float(i+5)});
    }
    tree.add(figure{float(8), float(8+5)});
    tree.add(figure{float(-8), float(-8+5)}); */
    //srand(time(NULL));
    srand(0);
    for(int i = 0; i < 300; i++)
    {
        figure f{float(rand() % 35 - 19), float(rand() % 35) - 19};
        //if(!tree.contains(f))
            tree.add(f);
        //else
        //    i--;
    }
    auto sss = tree.locateNodeByPosition(tree.nodes.at(0), 10, 5);
    //std::cout << node.toString();

    /* int tab[5] = {1, 2, 3, 4, 5};
    int* jed = tab;
    int* dwa = tab + 3;
    std::cout << "   " << dwa - jed; */

    int count = 0;
    int num = 0;
    for(auto& node : tree)
    {
        for(auto itr = node.data.begin(); itr != node.data.begin() + node.elements; itr++)
        {
            //std::cout << "Node " << num << ": " << itr->x << " " << itr->y << '\n';
            count++;
        }
        num++;
        //std::cout << node.data.at(0).x << " " << node.data.at(0).y << '\n';
    }

    std::cout << "Count: " << count << " Test: " << tree.test << '\n';

    #ifndef NO_UI

    sf::RenderWindow window(sf::VideoMode(600, 600), "My window");
    //sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(40.f, 40.f));
    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(100.f, 100.f));
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

            if (event.type == sf::Event::MouseWheelScrolled)
			{
				zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, 1.1f, view, event.mouseWheelScroll.delta);
			}

            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
                {
                    sf::Texture texture;
                    texture.create(window.getSize().x, window.getSize().y);
                    texture.update(window);
                    if (texture.copyToImage().saveToFile("ss.jpg"))
                    {
                        std::cout << "screenshot saved to ss.jpg" << std::endl;
                    }
                }
            }

            if (event.type == sf::Event::Resized)
				resizeView(window, view);

        }

        // clear the window with black color
        window.setView(view);
        window.clear(sf::Color::Black);

        // draw everything here...
        // window.draw(...);
        /* sf::RectangleShape center(sf::Vector2f(1.f, 1.f));
        center.setFillColor(sf::Color::Green);
        center.setPosition(sf::Vector2f(0.f, 0.f));
        center.setOrigin(sf::Vector2f(0.5f, 0.5f));
        window.draw(center); */

        unsigned int color = 0;
        unsigned int which = 0;
        for(auto& node : tree)
        {
            sf::RectangleShape background;
            background.setSize(sf::Vector2f(node.width * 2, node.height * 2));
            //background.setOrigin(sf::Vector2f(node.width, node.height));
            background.setPosition(node.x * 2 - node.width, node.y * 2 - node.height);
            background.setFillColor(sf::Color(color + (which == 0 ? 10 : 0), color + (which == 1 ? 10 : 0), color + (which == 2 ? 10 : 0)));
            which++;
            which %= 3;
            if(which == 0)
            {
                color += 20;
                color %= 250;
            }
            window.draw(background);
            for(auto itr = node.data.begin(); itr != node.data.begin() + node.elements; itr++)
            {
                sf::RectangleShape rect(sf::Vector2f(1.f, 1.f));
                rect.setPosition(sf::Vector2f(itr->x * 2, itr->y * 2));
                rect.setFillColor(sf::Color::Red);
                //rect.setOrigin(sf::Vector2f(0.5f, 0.5f));
                window.draw(rect);
                if(abs(itr->x) > 19 || abs(itr->y) > 19)
                    std::cout << "Node " << num << ": " << itr->x << " " << itr->y << '\n';
            }
        }

        // end the current frame
        window.display();
    }
    #endif

}