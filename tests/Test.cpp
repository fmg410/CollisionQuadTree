#include "../src/Settings.hpp"
#include "../src/CollideFunctions.hpp"
#include "csv2.hpp"
#include <utility>

using namespace csv2;

constexpr unsigned int DEFAULT_VERTICES_COUNT = 7;
const unsigned int DEFAULT_TREE_THRESHHOLD = 8;

template<unsigned int vertices, unsigned int treshhold>
std::pair<long, long> testTree(std::string treeFilename, unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY);

template<unsigned int vertices>
std::pair<long, long> testVector(std::string treeFilename, unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY);

//for ending recursion
template<std::size_t min, std::size_t max, bool vertices = true>
typename std::enable_if<min==max+1>::type treeSwitcher(std::string treeFilename, unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{

}

//version that will do the real work
template<std::size_t min, std::size_t max, bool vertices = true>
typename std::enable_if<min!=max+1>::type treeSwitcher(std::string treeFilename, unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{
    std::pair<long, long> pair;
    if constexpr (vertices)
        pair = testTree<min, DEFAULT_TREE_THRESHHOLD>(treeFilename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    else
        pair = testTree<DEFAULT_VERTICES_COUNT, min>(treeFilename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    std::ofstream file(treeFilename, std::ios::app);
    Writer<delimiter<';'>> writer(file);
    std::vector<std::string> row ={std::to_string(vertices ? min : DEFAULT_VERTICES_COUNT), std::to_string(!vertices ? min : DEFAULT_TREE_THRESHHOLD), std::to_string(elements), std::to_string(maxIterations), std::to_string(scale), std::to_string(areaWidth), std::to_string(areaHeight), std::to_string(initialX), std::to_string(initialY), std::to_string(pair.first), std::to_string(pair.second), std::to_string(pair.first)};
    writer.write_row(row);
    file.close();
    std::cout << "Current t min: " << min << std::endl;
    treeSwitcher<min+1, max, vertices>(treeFilename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
}

//for ending recursion
template<std::size_t min, std::size_t max>
typename std::enable_if<min==max+1>::type vectorSwitcher(std::string treeFilename, unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{

}

//version that will do the real work
template<std::size_t min, std::size_t max>
typename std::enable_if<min!=max+1>::type vectorSwitcher(std::string treeFilename, unsigned int elements = 1000, unsigned int maxIterations = 1000, float scale = 15.f, float areaWidth = 1000.f, float areaHeight = 1000.f, float initialX = 0.f, float initialY = 0.f)
{
    std::pair<long, long> pair = testVector<min>(treeFilename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    std::ofstream file(treeFilename, std::ios::app);
    Writer<delimiter<';'>> writer(file);
    std::vector<std::string> row ={std::to_string(min), std::to_string(elements), std::to_string(maxIterations), std::to_string(scale), std::to_string(areaWidth), std::to_string(areaHeight), std::to_string(initialX), std::to_string(initialY), std::to_string(pair.second), std::to_string(pair.first)};
    writer.write_row(row);
    file.close();
    std::cout << "Current v min: " << min << std::endl;
    vectorSwitcher<min+1, max>(treeFilename, elements, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
}

template<unsigned int vertices, unsigned int treshhold>
std::pair<long, long> testTree(std::string treeFilename, unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY)
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
        Figure f{posGenerator(generator), posGenerator(generator), speedGenerator(generator), speedGenerator(generator), /* scaleGenerator(generator) */ scale};
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
            return std::make_pair(lround(average * 1.f / timings.size()), collisionCount);
        }
        if(maxIterations != -1)
            iterations++;
    }
    return std::make_pair(0, 0);
}

template<unsigned int vertices>
std::pair<long, long> testVector(std::string treeFilename, unsigned int elements, unsigned int maxIterations, float scale, float areaWidth, float areaHeight, float initialX, float initialY)
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
        Figure f{posGenerator(generator), posGenerator(generator), speedGenerator(generator), speedGenerator(generator), /* scaleGenerator(generator) */ scale};
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
            return std::make_pair(lround(average * 1.f / timings.size()), collisionCount);
        }
        if(maxIterations != -1)
            iterations++;
    }
    return std::make_pair(0, 0);
}

void emptyRow(std::string filename)
{
    std::ofstream file(filename, std::ios::app);
    Writer<delimiter<';'>> writer(file);
    std::vector<std::string> row ={""};
    writer.write_row(row);
    file.close();
}

void standardTreeTests(int argc, char** argv)
{
    unsigned int elements = 1000;
    unsigned int maxIterations = 1000;
    float scale = 15.f;
    float areaWidth = 1000.f;
    float areaHeight = 1000.f;
    float initialX = 0.f;
    float initialY = 0.f;

    if(argc == 8)
    {
        elements = std::stoi(argv[1]);
        maxIterations = std::stoi(argv[2]);
        scale = std::stof(argv[3]);
        areaWidth = std::stof(argv[4]);
        areaHeight = std::stof(argv[5]);
        initialX = std::stof(argv[6]);
        initialY = std::stof(argv[7]);
    }

    std::string treeFilename = __func__;
    treeFilename.append(".csv");
    {
        std::ofstream fileT(treeFilename, std::ios::app);
        Writer<delimiter<';'>> writer(fileT);
        std::vector<std::string> row ={"Vertices", "Treshhold", "Elements", "Max iterations", "Figure scale", "Area width", "Area height", "Root X", "Root Y", "Collision count", "Average all collision time"};
        writer.write_row(row);
        fileT.close();
    }

    treeSwitcher<3, 30, true>(treeFilename); //vertices
    emptyRow(treeFilename);
    treeSwitcher<3, 30, false>(treeFilename); //treshhold
    emptyRow(treeFilename);

    for(int i = 1; i < 20; i++)
        treeSwitcher<DEFAULT_VERTICES_COUNT, DEFAULT_VERTICES_COUNT>(treeFilename, elements, maxIterations, scale, areaWidth*i, areaHeight*i, initialX, initialY);
    emptyRow(treeFilename);
    for(int i = 1; i < 20; i++)
        treeSwitcher<DEFAULT_VERTICES_COUNT, DEFAULT_VERTICES_COUNT>(treeFilename, elements*i, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    emptyRow(treeFilename);
    for(int i = 1; i < 20; i++)
        treeSwitcher<DEFAULT_VERTICES_COUNT, DEFAULT_VERTICES_COUNT>(treeFilename, elements*i, maxIterations, scale, areaWidth*i, areaHeight*i, initialX, initialY);
    emptyRow(treeFilename);
    for(int i = 1; i < 60; i++)
        treeSwitcher<DEFAULT_VERTICES_COUNT, DEFAULT_VERTICES_COUNT>(treeFilename, elements, maxIterations, scale/15*i, areaWidth, areaHeight, initialX, initialY);
    emptyRow(treeFilename);
}

void standardVectorTests(int argc, char** argv)
{
    unsigned int elements = 1000;
    unsigned int maxIterations = 1000;
    float scale = 15.f;
    float areaWidth = 1000.f;
    float areaHeight = 1000.f;
    float initialX = 0.f;
    float initialY = 0.f;

    if(argc == 8)
    {
        elements = std::stoi(argv[1]);
        maxIterations = std::stoi(argv[2]);
        scale = std::stof(argv[3]);
        areaWidth = std::stof(argv[4]);
        areaHeight = std::stof(argv[5]);
        initialX = std::stof(argv[6]);
        initialY = std::stof(argv[7]);
    }

    std::string vectorFilename = __func__;
    vectorFilename.append(".csv");
    {
        std::ofstream fileV(vectorFilename, std::ios::app);
        Writer<delimiter<';'>> writer(fileV);
        std::vector<std::string> row ={"Vertices", "Elements", "Max iterations", "Figure scale", "Area width", "Area height", "Root X", "Root Y", "Collision count", "Average all collision time"};
        writer.write_row(row);
        fileV.close();
    }

    vectorSwitcher<3, 20>(vectorFilename); //vertices
    emptyRow(vectorFilename);

    for(int i = 1; i < 10; i++)
        vectorSwitcher<DEFAULT_VERTICES_COUNT, DEFAULT_VERTICES_COUNT>(vectorFilename, elements, maxIterations, scale, areaWidth*i, areaHeight*i, initialX, initialY);
    emptyRow(vectorFilename);
    for(int i = 1; i < 10; i++)
        vectorSwitcher<DEFAULT_VERTICES_COUNT, DEFAULT_VERTICES_COUNT>(vectorFilename, elements/10*i, maxIterations, scale, areaWidth, areaHeight, initialX, initialY);
    emptyRow(vectorFilename);
    for(int i = 1; i < 30; i++)
        vectorSwitcher<DEFAULT_VERTICES_COUNT, DEFAULT_VERTICES_COUNT>(vectorFilename, elements/10*i, maxIterations / i, scale, areaWidth*i, areaHeight*i, initialX, initialY);
    emptyRow(vectorFilename);
    for(int i = 1; i < 60; i++)
        vectorSwitcher<DEFAULT_VERTICES_COUNT, DEFAULT_VERTICES_COUNT>(vectorFilename, elements, maxIterations, scale/15*i, areaWidth, areaHeight, initialX, initialY);
    emptyRow(vectorFilename);
}

int main(int argc, char** argv)
{
    standardTreeTests(argc, argv);
    standardVectorTests(argc, argv);

    return 0;
}