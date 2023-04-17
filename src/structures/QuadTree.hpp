#include "QuadNode.hpp"

#include <vector>


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

    void add(const T& data, QuadNode<T>& node)
    {
        if((node.end - node.begin) < threshhold)
            elements.insert(node.end)
    }

public:
    int threshhold = 8;
    int height = 200;
    int width = 200;
    std::vector<QuadNode<T>> nodes;
    std::vector<T> elements;



};