#include "structures/QuadNode.hpp"
#include <array>
#include <iostream>
#include "structures/QuadTree.hpp"
#include "physics/functions.hpp"
#include <cmath>
#include <chrono>
#include "collision/Figure.hpp"
#include "collision/DrawableFigure.hpp"
#include "collision/Collision.hpp"
#include "utils/RainbowGenerator.hpp"

//#define NO_UI

#ifndef NO_UI
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#endif

const bool SIMULATE_BOTH = true;
const bool SIMULATE_TREE = true;
const bool START_PAUSED = false;
constexpr unsigned int VERTICES_COUNT = 4;
const int CURRENT_ELEMENTS = 1000;
const unsigned int MAX_ITERATIONS = 1000;

static const float VIEW_HEIGHT = 600.f;

#ifndef NO_UI
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
#endif

void testTree() // TODO: update member (first find member...)
{
    using Figure = Figure<VERTICES_COUNT>;
    //QuadNode<Figure, 8> node;
    std::array<int, 5> a;
    a.at(0) = 3;

    //std::cout << a.size() << " " << a.max_size();
    QuadTree<Figure, 100000> tree(0, 0, 1000.f, 1000.f);
    /* Figure f{12, 3};
    tree.add(f); */
    /* for(int i = 0; i < 8; i++) // breaks starting at 10
    {
        tree.add(Figure{float(i), float(i+5)});
    }
    tree.add(Figure{float(8), float(8+5)});
    tree.add(Figure{float(-8), float(-8+5)}); */
    //srand(time(NULL));
    srand(0);
    for(int i = 0; i < CURRENT_ELEMENTS; i++)
    {
        Figure f{float(rand() % 1000 - 500), float(rand() % 1000) - 500, (rand() % 30) / 10.f - 0.5f, (rand() % 30) / 10.f - 0.5f, 10};
        f.increaseAngle(rand() % 360);
        //if(!tree.contains(f))
            tree.add(f);
        //else
        //    i--;
    }

    /* for(int j = 1000; j < CURRENT_ELEMENTS; j += 100)
    {
        std::cout << "MAX: " << j << std::endl;
        for(int i = 0; i < j; i++)
        {
            Figure f{float(rand() % 35 - 19), float(rand() % 35) - 19, (rand() % 21) / 10.f - 1.f, (rand() % 21) / 10.f - 1.f};
            //if(!tree.contains(f))
                tree.add(f);
            //else
            //    i--;
        }
        tree = QuadTree<Figure, 100000>();
    } */
    //auto sss = tree.locateNodeByPosition(tree.nodes.at(0), 10, 5);
    /* for(int i = 0; i < CURRENT_ELEMENTS; i++)
        tree.add(Figure{float(-80.f + i), float(-80.f + i), 0.1f, 0.2f}); */
    //std::cout << node.toString();

    /* int tab[5] = {1, 2, 3, 4, 5};
    int* jed = tab;
    int* dwa = tab + 3;
    std::cout << "   " << dwa - jed; */

    #ifndef NO_UI

    bool pause = START_PAUSED;
    unsigned int iterations = 0;


    using Clock = std::chrono::steady_clock;
    using std::chrono::time_point;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::microseconds;
    using std::chrono::nanoseconds;
    using namespace std::literals::chrono_literals;
    std::vector<int64_t> timings;

    sf::RenderWindow window(sf::VideoMode(600, 600), "My window");
    //sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(40.f, 40.f));
    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(1000.f, 1000.f));
    window.setView(view);


    uint64_t collisionCount = 0;
    sf::Clock clock;
    float lastTime = 0;
    sf::Font font;
    if (!font.loadFromFile("Arialn.ttf"))
    {
        return;
    }

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
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::N))
                {
                    Figure f{float(rand() % 35 - 19), float(rand() % 35) - 19, (rand() % 30) / 10.f - 0.5f, (rand() % 30) / 10.f - 0.5f, 10};
                    tree.add(f);
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                {
                    auto c = tree.begin();
                    while((*c).elements == 0)
                        c.operator++();
                    (*c).data.at(0).y -= 1.f;
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    auto c = tree.begin();
                    while((*c).elements == 0)
                        c.operator++();
                    (*c).data.at(0).y += 1.f;
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    auto c = tree.begin();
                    while((*c).elements == 0)
                        c.operator++();
                    (*c).data.at(0).x -= 1.f;
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    auto c = tree.begin();
                    while((*c).elements == 0)
                        c.operator++();
                    (*c).data.at(0).x += 1.f;
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                {
                    auto c = tree.begin();
                    while(c != tree.end() && (*c).elements == 0)
                        c.operator++();
                    tree.deleteData(*c, 0);
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                {
                    pause = !pause;
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::C))
                {
                    auto c = tree.begin();
                    while((*c).elements == 0)
                        c.operator++();
                    std::cout << "Node: " << (*c).data.at(0).x << " " << (*c).data.at(0).y << '\n';
                }
            }

            if (event.type == sf::Event::Resized)
				resizeView(window, view);

        }
        // fps control !!!!!!!!!!!!!!!!!!
        if(clock.getElapsedTime().asMilliseconds() < 16)
            continue;

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

        int num = 0;
        if(!pause)
        {
            time_point<Clock> start = Clock::now();
            for(int m = 0; m < 1; m++)
            {
                for(auto& node : tree)
                {
                    for(int i = 0; i < node.elements; i++)
                    {
                        updateSpeed(node.data.at(i), 1.f);
                        applyBoundariesNode(node.data.at(i), tree.getRootX(), tree.getRootY(), tree.getRootWidth(), tree.getRootHeight());
                        //std::cout << "Node " << num << ": " << node.data.at(i).x << " " << node.data.at(i).y << '\n';
                    }
                    /* for(int i = 0; i < node.elements; i++)
                        for(int j = i + 1; j < node.elements - 1; j++)
                                collide(node.data.at(i), node.data.at(j)); */
                    for(int i = 0; i < node.elements; i++)
                    {
                        auto list = tree.getNodesInArea(node, node.data.at(i));
                        for(auto& otherNodes : list)
                        {
                            for(int j = 0; j < otherNodes->elements; j++)
                                if(node.data.at(i) != otherNodes->data.at(j))
                                {
                                    if(collideAdv(node.data.at(i), otherNodes->data.at(j), 1.f))
                                        collisionCount++;
                                    applyBoundariesNode(node.data.at(i), tree.getRootX(), tree.getRootY(), tree.getRootWidth(), tree.getRootHeight());
                                    applyBoundariesNode(node.data.at(j), tree.getRootX(), tree.getRootY(), tree.getRootWidth(), tree.getRootHeight());
                                }
                        }
                    }
                    for(int i = 0; i < node.elements; i++)
                    {
                        if(tree.correctDataPosition(node, i))
                            i--;
                    }
                    num++;
                }
            }
            tree.mergeTree();
            time_point<Clock> end = Clock::now();
            nanoseconds diff = duration_cast<nanoseconds>(end - start);
            timings.push_back(diff.count());
            if(iterations >= MAX_ITERATIONS)
            {
                int64_t average = 0;
                std::for_each(timings.begin(), timings.end(), [&](auto& t){ average += t; });
                std::cout << "Collision count : " << collisionCount << std::endl;
                std::cout << "CollisionQuadTree : Average collision detection time for " << MAX_ITERATIONS << " iterations for " << CURRENT_ELEMENTS << " elements with " << Figure::size() << " vertices is : " << average * 1.f / timings.size() << " nanoseconds" << std::endl;
                break;
            }
            if(MAX_ITERATIONS != -1)
                iterations++;
        }


        unsigned int which = 0;
        num = 0;
        RainbowGenerator color;
        for(auto& node : tree)
        {
            sf::RectangleShape background;
            background.setSize(sf::Vector2f(node.width, node.height));
            //background.setOrigin(sf::Vector2f(node.width, node.height));
            background.setPosition(node.x - node.width / 2, node.y - node.height / 2);
            background.setFillColor(sf::Color(color.r, color.g, color.b));
            color.operator++();
            window.draw(background);
        }
        for(auto& node : tree)
        {
            for(auto itr = node.data.begin(); itr != node.data.begin() + node.elements; itr++)
            {
                /* sf::RectangleShape rect(sf::Vector2f(1.f, 1.f));
                rect.setPosition(sf::Vector2f(itr->x * 2, itr->y * 2));
                rect.setFillColor(sf::Color::Red);
                //rect.setOrigin(sf::Vector2f(0.5f, 0.5f));
                window.draw(rect); */
                itr->update();
                DrawableFigure drawableFigure(*itr);
                window.draw(drawableFigure);
                //if(abs(itr->x) > 19 || abs(itr->y) > 19)
                    //std::cout << "Node " << num << ": " << itr->x << " " << itr->y << '\n';
            }
            num++;
        }

        float currentTime = clock.restart().asSeconds();
        float fps = 1.f / (currentTime - lastTime);
        lastTime = currentTime;

        {sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(fps));
        text.setCharacterSize(12);
        text.setFillColor(sf::Color::White);
        text.setPosition(sf::Vector2f(650.f, 650.f));
        window.draw(text);}

        {sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(tree.nodes.size()));
        text.setCharacterSize(12);
        text.setFillColor(sf::Color::White);
        text.setPosition(sf::Vector2f(-650.f, -650.f));
        window.draw(text);}

        // end the current frame
        window.display();
    }
    #endif

}


void testVector() // TODO: update member (first find member...)
{
    using Figure = Figure<VERTICES_COUNT>;
    //QuadNode<Figure, 8> node;
    std::array<int, 5> a;
    a.at(0) = 3;

    //std::cout << a.size() << " " << a.max_size();
    QuadTree<Figure, 100000> tree(0, 0, 1000.f, 1000.f);
    std::vector<Figure> vec;
    /* Figure f{12, 3};
    tree.add(f); */
    /* for(int i = 0; i < 8; i++) // breaks starting at 10
    {
        tree.add(Figure{float(i), float(i+5)});
    }
    tree.add(Figure{float(8), float(8+5)});
    tree.add(Figure{float(-8), float(-8+5)}); */
    //srand(time(NULL));
    srand(0);
    for(int i = 0; i < CURRENT_ELEMENTS; i++)
    {
        Figure f{float(rand() % 1000 - 500), float(rand() % 1000) - 500, (rand() % 30) / 10.f - 0.5f, (rand() % 30) / 10.f - 0.5f, 10};
        //if(!tree.contains(f))
            vec.push_back(f);
        //else
        //    i--;
    }

    /* for(int j = 1000; j < CURRENT_ELEMENTS; j += 100)
    {
        std::cout << "MAX: " << j << std::endl;
        for(int i = 0; i < j; i++)
        {
            Figure f{float(rand() % 35 - 19), float(rand() % 35) - 19, (rand() % 21) / 10.f - 1.f, (rand() % 21) / 10.f - 1.f};
            //if(!tree.contains(f))
                tree.add(f);
            //else
            //    i--;
        }
        tree = QuadTree<Figure, 100000>();
    } */
    //auto sss = tree.locateNodeByPosition(tree.nodes.at(0), 10, 5);
    /* for(int i = 0; i < CURRENT_ELEMENTS; i++)
        tree.add(Figure{float(-80.f + i), float(-80.f + i), 0.1f, 0.2f}); */
    //std::cout << node.toString();

    /* int tab[5] = {1, 2, 3, 4, 5};
    int* jed = tab;
    int* dwa = tab + 3;
    std::cout << "   " << dwa - jed; */

    #ifndef NO_UI

    bool pause = START_PAUSED;
    unsigned int iterations = 0;


    using Clock = std::chrono::steady_clock;
    using std::chrono::time_point;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::microseconds;
    using std::chrono::nanoseconds;
    using namespace std::literals::chrono_literals;
    std::vector<int64_t> timings;

    sf::RenderWindow window(sf::VideoMode(600, 600), "My window");
    //sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(40.f, 40.f));
    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(1000.f, 1000.f));
    window.setView(view);

    uint64_t collisionCount = 0;
    sf::Clock clock;
    float lastTime = 0;
    sf::Font font;
    if (!font.loadFromFile("Arialn.ttf"))
    {
        return;
    }

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
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                {
                    pause = !pause;
                }
            }

            if (event.type == sf::Event::Resized)
				resizeView(window, view);

        }
        // fps control !!!!!!!!!!!!!!!!!!
        if(clock.getElapsedTime().asMilliseconds() < 16)
            continue;

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

        int num = 0;
        if(!pause)
        {
            time_point<Clock> start = Clock::now();
            for(int m = 0; m < 1; m++)
            {
                for(int i = 0; i < vec.size(); i++)
                {
                    updateSpeed(vec.at(i), 1.f);
                    applyBoundariesNode(vec.at(i), 0.f, 0.f, 1000.f, 1000.f);
                    for(int j = i + 1; j < vec.size(); j++)
                    {
                        if(collideAdv(vec.at(i), vec.at(j), 1.f))
                            collisionCount++;
                        applyBoundariesNode(vec.at(i), 0.f, 0.f, 1000.f, 1000.f);
                        applyBoundariesNode(vec.at(j), 0.f, 0.f, 1000.f, 1000.f);
                    }
                }
            }
            time_point<Clock> end = Clock::now();
            nanoseconds diff = duration_cast<nanoseconds>(end - start);
            timings.push_back(diff.count());
            if(iterations >= MAX_ITERATIONS)
            {
                int64_t average = 0;
                std::for_each(timings.begin(), timings.end(), [&](auto& t){ average += t; });
                std::cout << "Collision count : " << collisionCount << std::endl;
                std::cout << "std::vector: Average collision detection time for " << MAX_ITERATIONS << " iterations for " << CURRENT_ELEMENTS << " elements with " << Figure::size() << " vertices is : " << average * 1.f / timings.size() << " nanoseconds" << std::endl;
                break;
            }
            if(MAX_ITERATIONS != -1)
                iterations++;
        }


        unsigned int which = 0;
        num = 0;
        RainbowGenerator color;
        for(auto& node : tree)
        {
            for(auto& itr : vec)
            {
                /* sf::RectangleShape rect(sf::Vector2f(1.f, 1.f));
                rect.setPosition(sf::Vector2f(itr->x * 2, itr->y * 2));
                rect.setFillColor(sf::Color::Red);
                //rect.setOrigin(sf::Vector2f(0.5f, 0.5f));
                window.draw(rect); */
                itr.update();
                DrawableFigure drawableFigure(itr);
                window.draw(drawableFigure);
                //if(abs(itr->x) > 19 || abs(itr->y) > 19)
                    //std::cout << "Node " << num << ": " << itr->x << " " << itr->y << '\n';
            }
            num++;
        }

        float currentTime = clock.restart().asSeconds();
        float fps = 1.f / (currentTime - lastTime);
        lastTime = currentTime;

        {sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(fps));
        text.setCharacterSize(12);
        text.setFillColor(sf::Color::White);
        text.setPosition(sf::Vector2f(650.f, 650.f));
        window.draw(text);}

        {sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(tree.nodes.size()));
        text.setCharacterSize(12);
        text.setFillColor(sf::Color::White);
        text.setPosition(sf::Vector2f(-650.f, -650.f));
        window.draw(text);}

        // end the current frame
        window.display();
    }
    #endif

}

int main()
{
    if(SIMULATE_BOTH)
    {
        testTree();
        testVector();
    }
    else
    {
        if(SIMULATE_TREE)
            testTree();
        else
            testVector();
    }

}