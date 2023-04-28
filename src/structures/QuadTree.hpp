#include "QuadNode.hpp"

#include <vector>
#include <algorithm>


template <typename T>
class QuadTree{

public:
    void add(const T& elem)
    {

    }

    QuadNode& locateNodeBasedOn(float x, float y, float centerX, float centerY, int modifier = 1)
    {
        QuadNode<T>* node = nodes.at(0);
        float h = 0;
        float w = 0;
        while(true)
        {
            int child = (x < w) + (y < h) * 3;
            child -= (child / 4) * 2;
            if(!node->children[0])
                return *node;
            node = node->children[child];
            if(x < w)
                w -= width / modifier;
            else
                w += width / modifier;
            if(y < h)
                h -= height / modifier;
            else
                h += height / modifier;
            modifier *= 2;
        }
    }

    QuadNode<T>& ZlocateNodeBasedOn(float x, float y, int modifier = 1)
    {
        QuadNode<T>* node = nodes.at(0);
        float h = 0;
        float w = 0;
        while(true)
        {
            int child = (x < w) + (y < h) * 3;
            child -= (child / 4) * 2;
            if(!node->children[0])
                return *node;
            node = node->children[child];
            if(x < w)
                w -= width / modifier;
            else
                w += width / modifier;
            if(y < h)
                h -= height / modifier;
            else
                h += height / modifier;
            modifier *= 2;
        }
    }

    QuadNode<T>& locateNodeByPosition(QuadNode<T>& node, float x, float y)
    {
        if(node.x + width < x || node.x - width > x || node.y + height < y || node.y - height > y)
            return locateNodeByPosition(node.parent, x, y);
        if(node.children[0] == node.children[1])
            return node;
        int child = (x < node.x) + (y < node.y) * 3;
        child -= (child / 4) * 2;
        return locateNodeByPosition(node.children[child], x, y);
    }

    void add(const T& data, QuadNode<T>& node)
    {
        if(elements.size() + 1 >= maxElements)
            return;

        elements.insert(++(node.end), data);

        if((node.end - node.begin) >= threshhold)


    }

    bool divide(QuadNode<T>& node)
    {
        node.children[0] = &nodes.emplace_back(node.x + node.width/2, node.y + node.height/2, node.width/2, node.height/2, nullNode);
        node.children[1] = &nodes.emplace_back(node.x - node.width/2, node.y + node.height/2, node.width/2, node.height/2, nullNode);
        node.children[2] = &nodes.emplace_back(node.x - node.width/2, node.y - node.height/2, node.width/2, node.height/2, nullNode);
        node.children[3] = &nodes.emplace_back(node.x + node.width/2, node.y - node.height/2, node.width/2, node.height/2, nullNode);

        std::sort(node.begin, node.end, [&](auto& first, auto& second){
            return QuadrantByPoint(node, first) > QuadrantByPoint(node, second);
        });

        int currentNode = -1;
        auto itr1 = std::find_if(node.begin, node.end, [&](auto first){
            return currentNode != QuadrantByPoint(node, first);
        });
        node.children[currentNode];

        for(int i = 0; i < 3; i++)
        {
            node.children[i].begin = std::find_if(node.begin, node.end, [&](auto first){
            return currentNode == QuadrantByPoint(node, first);
            });
            node.children[i].end = std::find_if(node.children[i].begin, node.end, [&](auto first){
                return currentNode != QuadrantByPoint(node, first);
            });
            if(node.children[i].begin == node.children[i].end)
            /* if(node.children[i].end == node.end + 1)
                node.children[i].end--; */
        }

        /* for(auto itr = node.begin; itr != node.end + 1; itr++)
        {
            if(QuadrantByPoint(node, itr) != currentNode)
            {

                currentNode = QuadrantByPoint(node, itr);
                (*(node.children[currentNode])).begin = itr;
            }
        } */
    }

    // return range [0, 3]
    int QuadrantByPoint(float centerX, float centerY, float posX, float posY)
    {
        int quadrant = (centerX < posX) + (centerY < posY) * 3;
        quadrant -= (quadrant / 4) * 2;
        return quadrant;
    }

    int QuadrantByPoint(float centerX, float centerY, QuadNode<T>& node)
    {
        return QuadrantByPoint(centerX, centerY, node.x, node.y);
    }

    int QuadrantByPoint(QuadNode<T>& reference, QuadNode<T>& node)
    {
        return QuadrantByPoint(reference.x, reference.y, node.x, node.y);
    }

    QuadTree()
    : elements(maxElements) // set constant size (possibly in template) so no iterators are invalidated // change later
    {

    }

    // test for scenario where threshold+1 elements are in a corner so that the map divides infinitely, tree hight should be log[figure_radious](map_size)
public:
    int threshhold = 8;
/*     int height = 200;
    int width = 200; */
    std::vector<QuadNode<T>> nodes;
    std::vector<T> elements;
    const int maxElements = 10000;
    QuadNode<T> nullNode{};


    //void add(QuadNode<T>& node, const T& data);

};