#include "DrawableFigure.hpp"
#include "Figure.hpp"
#include <iostream>

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

int main()
{
    Figure<9> f;
    f.x = 100;
    f.y = 100;
    f.update();
    for(auto&& pos : f)
    {
        std::cout << "x: " << pos.x << " y: " << pos.y << "\n";
    }

     sf::RenderWindow window(sf::VideoMode(200, 200), "My window");
    sf::View view(sf::Vector2f(100.f, 100.f), sf::Vector2f(400.f, 400.f));
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
                    f.increaseAngle(10.f);
                    f.update();
                }
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        DrawableFigure dra(f);
        window.draw(dra);
        // draw everything here...
        // window.draw(...);

        // end the current frame
        window.display();
    }
}