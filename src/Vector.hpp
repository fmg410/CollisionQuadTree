#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "Settings.hpp"
#include "CollideFunctions.hpp"

void testVector() // TODO: update member (first find member...)
{
    using Figure = Figure<VERTICES_COUNT>;
    //QuadNode<Figure, 8> node;
    std::array<int, 5> a;
    a.at(0) = 3;

    //std::cout << a.size() << " " << a.max_size();
    //QuadTree<Figure, 8> tree(10000, 0, 0, 1000.f, 1000.f);
    std::vector<Figure> vec;
    vec.reserve(CURRENT_ELEMENTS + 1);
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
    /* Figure f{float(rand() % 1000 - 500), float(rand() % 1000) - 500, (rand() % 30) / 10.f - 0.5f, (rand() % 30) / 10.f - 0.5f, SCALE * 10};
        f.increaseAngle(rand() % 360);
        //if(!tree.contains(f))
        vec.push_back(f); */
    for(int i = 0; i < CURRENT_ELEMENTS; i++)
    {
        Figure f{float(rand() % int(AREA_WIDTH) - AREA_WIDTH/2), float(rand() % int(AREA_HEIGHT) - AREA_HEIGHT/2), (rand() % 30) / 10.f - 0.5f, (rand() % 30) / 10.f - 0.5f, /* 2.f */ rand() % 20 + 5.f};
        f.increaseAngle(rand() % 360);
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
    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(AREA_WIDTH, AREA_HEIGHT));
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
            collideVector<Figure>(vec, collideAdv<Figure>, collisionCount); // collisions
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

        // end the current frame
        window.display();
    }
    #endif

}


#endif