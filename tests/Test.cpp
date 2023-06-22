#include "../src/Settings.hpp"
#include "../src/CollideFunctions.hpp"
#include "csv2.hpp"

#define SETUP

using namespace csv2;

constexpr unsigned int DEFAULT_VERTICES_COUNT = 8;
const unsigned int DEFAULT_TREE_THRESHHOLD = 80;

template<unsigned int vertices, unsigned int treshhold>
float testTree(std::string filename, unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY);

template<unsigned int vertices>
float testVector(std::string filename, unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY);

//for ending recursion
template<std::size_t min, std::size_t max, bool vertices = true>
typename std::enable_if<min==max+1>::type treeSwitcher(std::string filename, unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{

}

//version that will do the real work
template<std::size_t min, std::size_t max, bool vertices = true>
typename std::enable_if<min!=max+1>::type treeSwitcher(std::string filename, unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{
    float time = 0;
    if constexpr (vertices)
        time = testTree<min, DEFAULT_TREE_THRESHHOLD>(filename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    else
        time = testTree<DEFAULT_VERTICES_COUNT, min>(filename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    std::ofstream file(filename, std::ios::app);
    Writer<delimiter<','>> writer(file);
    std::vector<std::string> row ={std::to_string(vertices ? min : DEFAULT_VERTICES_COUNT), std::to_string(!vertices ? min : DEFAULT_TREE_THRESHHOLD), std::to_string(elements), std::to_string(maxIterations), std::to_string(scale), std::to_string(areaWidth), std::to_string(areaHeight), std::to_string(initialX), std::to_string(initialY), std::to_string(time)};
    writer.write_row(row);
    file.close();
    std::cout << "Current min: " << min << std::endl;
    treeSwitcher<min+1, max, vertices>(filename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
}

//for ending recursion
template<std::size_t min, std::size_t max>
typename std::enable_if<min==max+1>::type vectorSwitcher(std::string filename, unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{

}

//version that will do the real work
template<std::size_t min, std::size_t max>
typename std::enable_if<min!=max+1>::type vectorSwitcher(std::string filename, unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{
    testVector<min>(filename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    vectorSwitcher<min+1, max>(filename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
}

template<unsigned int vertices, unsigned int treshhold>
float testTree(std::string filename, unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY)
{
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
            /* std::cout << "Collision count : " << collisionCount << std::endl;
            std::cout << "CollisionQuadTree : Average collision detection time for " << maxIterations << " iterations for " << elements << " elements with " << Figure::size() << " vertices and treshhold " << treshhold << " is : " << average * 1.f / timings.size() << " nanoseconds" << std::endl; */
            return average * 1.f / timings.size();
        }
        if(maxIterations != -1)
            iterations++;
    }
    return 0;
}

template<unsigned int vertices>
float testVector(std::string filename, unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY)
{
    using Figure = Figure<vertices>;

    std::vector<Figure> vec;
    vec.reserve(elements + 1);

    std::default_random_engine generator;
    std::uniform_real_distribution<float> posGenerator(-areaWidth/2, areaWidth/2);
    std::uniform_real_distribution<float> speedGenerator(-3.f, 3.f);
    std::uniform_real_distribution<float> scaleGenerator(2.f, scale);
    std::uniform_real_distribution<float> angleGenerator(0.f, 359.f);

    for(int i = 0; i < elements; i++)
    {
        Figure f{posGenerator(generator), posGenerator(generator), speedGenerator(generator), speedGenerator(generator), /* scaleGenerator(generator) */ 2.f};
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
        if(iterations >= maxIterations)
        {
            int64_t average = 0;
            std::for_each(timings.begin(), timings.end(), [&](auto& t){ average += t; });
            /* std::cout << "Collision count : " << collisionCount << std::endl;
            std::cout << "Vector : Average collision detection time for " << maxIterations << " iterations for " << elements << " elements with " << Figure::size() << " vertices and treshhold " << treshhold << " is : " << average * 1.f / timings.size() << " nanoseconds" << std::endl; */
            return average * 1.f / timings.size();
        }
        if(maxIterations != -1)
            iterations++;
    }
    return 0;
}

int main()
{
    std::string filename = "tree.csv";
    std::ofstream file(filename, std::ios::app);
    Writer<delimiter<','>> writer(file);
    std::vector<std::string> row ={"Vertices", "Treshhold", "Elements", "Max iterations", "Figure scale", "Area width", "Area height", "Root X", "Root Y", "Average all collision time"};
    writer.write_row(row);
    file.close();
    treeSwitcher<3, 5, true>(filename); //vertices
    treeSwitcher<3, 5, false>(filename); //treshhold
    //all<int>();
    //testTree();
    //testVector();
    return 0;
}