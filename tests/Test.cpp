#include "../src/Settings.hpp"
#include "../src/CollideFunctions.hpp"
#include "csv2.hpp"

#define SETUP

using namespace csv2;

template<unsigned int vertices, unsigned int treshhold>
void testTree(unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY);

//for ending recursion
template<std::size_t min, std::size_t max>
typename std::enable_if<min==max+1>::type treeSwitcher(unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{

}

//version that will do the real work
template<std::size_t min, std::size_t max>
typename std::enable_if<min!=max+1>::type treeSwitcher(unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{
    testTree<min, 4>(elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    f<min+1, max>();
}

/* template<typename T>
void all()
{
    unsigned int elements = 5000;
    unsigned int maxIterations = 100;
    float scale = 15.f;
    float areaWidth = 1000.f;
    float areaHeight = 1000.f;
    float initialX = 0.f;
    float initialY = 0.f;

    for(constexpr int i = 3; i < 14; i++)
        testTree<i, 8>(elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
} */

template<unsigned int vertices, unsigned int treshhold>
void testTree(unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY)
{
    std::ofstream file("test.csv", std::ios::app);
    Writer<delimiter<','>> writer(file);

    std::vector<std::vector<std::string>> rows =
        {
            {"a", "b", "c"},
            {"1", "2", "3"},
            {"4", "5", "6"}
        };

    writer.write_rows(rows);
    file.close();
    using Figure = Figure<vertices>;
    using QuadTree = QuadTree<Figure, treshhold>;

    QuadTree tree(100000, initialX, initialY, areaWidth, areaHeight);

    std::default_random_engine generator;
    std::uniform_real_distribution<float> posGenerator(-areaWidth/2, areaWidth/2);
    std::uniform_real_distribution<float> speedGenerator(-3.f, 3.f);
    std::uniform_real_distribution<float> scaleGenerator(2.f, scale);
    std::uniform_real_distribution<float> angleGenerator(0.f, 359.f);

    for(int i = 0; i < elements; i++)
    {
        Figure f{posGenerator(generator), posGenerator(generator), speedGenerator(generator), speedGenerator(generator), /* scaleGenerator(generator) */ 2.f};
        f.increaseAngle(angleGenerator(generator));
        tree.add(f);
    }

    using Clock = std::chrono::steady_clock;
    using std::chrono::time_point;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::microseconds;
    using std::chrono::nanoseconds;
    using namespace std::literals::chrono_literals;
    std::vector<int64_t> timings;

    unsigned int iterations = 0;
    uint64_t collisionCount = 0;
    sf::Clock clock;
    float lastTime = 0;

    while(true)
    {
        time_point<Clock> start = Clock::now();
        collideTree<Figure, treshhold>(tree, collideAdv<Figure>, collisionCount); // collisions
        time_point<Clock> end = Clock::now();
        nanoseconds diff = duration_cast<nanoseconds>(end - start);
        timings.push_back(diff.count());
        if(iterations >= maxIterations)
        {
            int64_t average = 0;
            std::for_each(timings.begin(), timings.end(), [&](auto& t){ average += t; });
            std::cout << "Collision count : " << collisionCount << std::endl;
            std::cout << "CollisionQuadTree : Average collision detection time for " << maxIterations << " iterations for " << elements << " elements with " << Figure::size() << " vertices and treshhold " << treshhold << " is : " << average * 1.f / timings.size() << " nanoseconds" << std::endl;
            break;
        }
        if(maxIterations != -1)
            iterations++;
    }

}

void testVector()
{
    using Figure = Figure<VERTICES_COUNT>;
    using QuadTree = QuadTree<Figure, TREE_THRESHHOLD>;

    std::vector<Figure> vec;
    vec.reserve(CURRENT_ELEMENTS + 1);

    std::default_random_engine generator;
    std::uniform_real_distribution<float> posGenerator(-AREA_WIDTH/2, AREA_WIDTH/2);
    std::uniform_real_distribution<float> speedGenerator(-3.f, 3.f);
    std::uniform_real_distribution<float> scaleGenerator(2.f, SCALE);
    std::uniform_real_distribution<float> angleGenerator(0.f, 359.f);

    for(int i = 0; i < CURRENT_ELEMENTS; i++)
    {
        Figure f{posGenerator(generator), posGenerator(generator), speedGenerator(generator), speedGenerator(generator), scaleGenerator(generator)};
        f.increaseAngle(angleGenerator(generator));
        vec.push_back(f);
    }

    using Clock = std::chrono::steady_clock;
    using std::chrono::time_point;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::microseconds;
    using std::chrono::nanoseconds;
    using namespace std::literals::chrono_literals;
    std::vector<int64_t> timings;

    unsigned int iterations = 0;
    uint64_t collisionCount = 0;
    sf::Clock clock;
    float lastTime = 0;

    while(true)
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
            std::cout << "CollisionQuadTree : Average collision detection time for " << MAX_ITERATIONS << " iterations for " << CURRENT_ELEMENTS << " elements with " << Figure::size() << " vertices is : " << average * 1.f / timings.size() << " nanoseconds" << std::endl;
            break;
        }
        if(MAX_ITERATIONS != -1)
            iterations++;
    }

}

int main()
{
    treeSwitcher<3, 40>();
    //all<int>();
    //testTree();
    //testVector();
    return 0;
}