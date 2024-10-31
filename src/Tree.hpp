#ifndef TREE_HPP
#define TREE_HPP

#include "Settings.hpp"
#include "CollideFunctions.hpp"


void testTree() // TODO: update member (first find member...)
{
    using Figure = Figure<VERTICES_COUNT>;

    QuadTree<Figure, TREE_THRESHHOLD> tree(1000000, INITIAL_X, INITIAL_Y, AREA_WIDTH, AREA_HEIGHT);

    srand(0);
/*     Figure f{0.f, 0.f, 2.f, 2.f, 800.f};
        f.increaseAngle(5.f);
            tree.add(f); */

    std::default_random_engine generator;
    std::uniform_real_distribution<float> posGenerator(-AREA_WIDTH/2, AREA_WIDTH/2);
    std::uniform_real_distribution<float> speedGenerator(-3.f, 3.f);
    std::uniform_real_distribution<float> scaleGenerator(2.f, SCALE);
    std::uniform_real_distribution<float> angleGenerator(0.f, 359.f);

    for(int i = 0; i < CURRENT_ELEMENTS; i++)
    {
        Figure f{posGenerator(generator), posGenerator(generator), speedGenerator(generator), speedGenerator(generator), SCALE};
        f.increaseAngle(angleGenerator(generator));
        tree.add(f);
    }

// float scale = 0.001f;
//     Figure ttt{3000.f, 3000.f, 0.f, 0.f, scale};
// float r =ttt.getR();
// tree.add(ttt);

//     for (int i = 0; i < TREE_THRESHHOLD + 1; i++)
//     {
//         Figure f{AREA_WIDTH/2, AREA_HEIGHT/2, 0.f, 0.f, scale};
//         tree.add(f);
//     }
// std::cout <<"R: " << r << std::endl;
// //std::cout << "tree:" << tree.allElements << std::endl;
// //std::cout << "Node: " << sizeof(QuadNode<Figure, 8>) << std::endl;

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
                    (*c).data.at(0).y -= (*c).data.at(0).getR();
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    auto c = tree.begin();
                    while((*c).elements == 0)
                        c.operator++();
                    (*c).data.at(0).y += (*c).data.at(0).getR();
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    auto c = tree.begin();
                    while((*c).elements == 0)
                        c.operator++();
                    (*c).data.at(0).x -= (*c).data.at(0).getR();
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    auto c = tree.begin();
                    while((*c).elements == 0)
                        c.operator++();
                    (*c).data.at(0).x += (*c).data.at(0).getR();
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
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::M))
                {
std::cout << "size:" << tree.nodes.size() << std::endl;
std::cout << "capacity:" << tree.nodes.capacity() << std::endl;


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

        int num = 0;
        if(!pause)
        {
            time_point<Clock> start = Clock::now();
            collideTree<Figure, TREE_THRESHHOLD>(tree, collideAdv<Figure>, collisionCount); // collisions
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
                itr->update();
                DrawableFigure drawableFigure(*itr);
                window.draw(drawableFigure);
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

        sf::Vector2i mouseLocalPosition = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPosition = window.mapPixelToCoords(mouseLocalPosition);
        //std::cout << (mouseLocalPosition.x + 300)*1000.f/300.f << " " << mouseLocalPosition.y*1000.f/600.f << " " << view.getCenter().x << " " << view.getCenter().y << " " << view.getSize().x << " " << view.getSize().y << '\n';
        //auto& currentHoveredNode = tree.locateNodeByPosition(view.getCenter().x - view.getSize().x/2 + mouseLocalPosition.x*1000.f/600.f, view.getCenter().y - view.getSize().y/2 + mouseLocalPosition.y*1000.f/600.f);
        auto& currentHoveredNode = tree.locateNodeByPosition(mouseWorldPosition.x, mouseWorldPosition.y);
        if(&currentHoveredNode != &tree.getRoot())
        {
            sf::Text text;
            text.setFont(font);
            text.setString(std::string("X: ").append(std::to_string(currentHoveredNode.x)).append("\nY: ").append(std::to_string(currentHoveredNode.y)).append("\nWidth: ").append(std::to_string(currentHoveredNode.width)).append("\nHeight: ").append(std::to_string(currentHoveredNode.height).append("\nElement count: ").append(std::to_string(currentHoveredNode.elements)).append("\nLevel: ").append(std::to_string(tree.levelDifference(tree.getRoot(), currentHoveredNode)))));
            text.setCharacterSize(30);
            text.setScale(sf::Vector2f(view.getSize().x / 1000.f, view.getSize().y / 1000.f));
            text.setFillColor(sf::Color::White);
            text.setPosition(sf::Vector2f(view.getCenter().x - view.getSize().x/2, view.getCenter().y - view.getSize().y/2));
            window.draw(text);
        }

        // end the current frame
        window.display();
    }
    #endif

}

#endif