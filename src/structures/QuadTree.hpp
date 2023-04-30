#pragma once
#include "QuadNode.hpp"

#include <vector>
#include <algorithm>


template <typename T>
class QuadTree{

public:

    static const int threshhold = 8;
    using QuadNode = QuadNode<T, threshhold>;
    std::vector<QuadNode> nodes;
    QuadNode nullNode;

    QuadTree()
    {
        nodes.reserve(10000);
        QuadNode n(0.f, 0.f, 100.f, 100.f, nullNode);
        //QuadNode c = std::move(n);
        nodes.push_back(n);
    }

    void add(const T& elem)
    {
        add(elem, nodes.at(0));
    }

    /* QuadNode& locateNodeBasedOn(float x, float y, float centerX, float centerY, int modifier = 1)
    {
        QuadNode* node = nodes.at(0);
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

    QuadNode& ZlocateNodeBasedOn(float x, float y, int modifier = 1)
    {
        QuadNode* node = nodes.at(0);
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
    } */

    QuadNode& locateNodeByPosition(QuadNode& node, float x, float y)
    {
        if(node.x + node.width/2 < x || node.x - node.width/2 > x || node.y + node.height/2 < y || node.y - node.height/2 > y)
            return locateNodeByPosition(*(node.parent), x, y);
        if(node.children[0] == node.children[1])
            return node;
        int child = (x < node.x) + (y < node.y) * 3;
        child -= (child / 4) * 2;
        return locateNodeByPosition(*(node.children[child]), x, y);
    }

    void add(const T& data, QuadNode& node)
    {
        QuadNode& n = locateNodeByPosition(nodes.at(0), data.x, data.y);
        if(n.elements < threshhold)
        {
            *(n.lastElement()) = data;
            n.elements++;
        }
        else
        {
            divide(n);
            add(data, n);
        }
        /* if(elements.size() + 1 >= maxElements)
            return; */

        //elements.insert(++(node.end), data);

        //if((node.end - node.begin) >= threshhold)


    }

    bool divide(QuadNode& node)
    {
        node.children[0] = &(nodes.emplace_back(node.x + node.width/4, node.y + node.height/4, node.width/2, node.height/2, nullNode));
        node.children[1] = &(nodes.emplace_back(node.x - node.width/4, node.y + node.height/4, node.width/2, node.height/2, nullNode));
        node.children[2] = &(nodes.emplace_back(node.x - node.width/4, node.y - node.height/4, node.width/2, node.height/2, nullNode));
        node.children[3] = &(nodes.emplace_back(node.x + node.width/4, node.y - node.height/4, node.width/2, node.height/2, nullNode));
        node.children[0]->parent = &node;
        node.children[1]->parent = &node;
        node.children[2]->parent = &node;
        node.children[3]->parent = &node;

        std::sort(node.data.begin(), node.data.end(), [&](auto& first, auto& second){
            return QuadrantByPoint(node, first) > QuadrantByPoint(node, second);
        });

        int currentNode = -1;
        auto itr1 = std::find_if(node.data.begin(), node.data.end(), [&](auto first){
            return currentNode != QuadrantByPoint(node, first);
        });
        node.children[currentNode];
        for(auto itr : node.data)
        {
            auto n = node.children[QuadrantByPoint(node, itr)];
            *(n->lastElement()) = std::move(itr);
            n->elements++;
        }

        return true;
    }
        /* for(int i = 0; i < 3; i++)
        {
            node.children[i].begin = std::find_if(node.begin, node.end, [&](auto first){
            return currentNode == QuadrantByPoint(node, first);
            });
            node.children[i].end = std::find_if(node.children[i].begin, node.end, [&](auto first){
                return currentNode != QuadrantByPoint(node, first);
            });
            //if(node.children[i].begin == node.children[i].end)
            /* if(node.children[i].end == node.end + 1)
                node.children[i].end--;
        } */

        /* for(auto itr = node.begin; itr != node.end + 1; itr++)
        {
            if(QuadrantByPoint(node, itr) != currentNode)
            {

                currentNode = QuadrantByPoint(node, itr);
                (*(node.children[currentNode])).begin = itr;
            }
        } */

    // return range [0, 3]
    int QuadrantByPoint(float centerX, float centerY, float posX, float posY)
    {
        int quadrant = (centerX < posX) + (centerY < posY) * 3;
        quadrant -= (quadrant / 4) * 2;
        return quadrant;
    }

    int QuadrantByPoint(float centerX, float centerY, QuadNode& node)
    {
        return QuadrantByPoint(centerX, centerY, node.x, node.y);
    }

    int QuadrantByPoint(QuadNode& reference, QuadNode& node)
    {
        return QuadrantByPoint(reference.x, reference.y, node.x, node.y);
    }

    int QuadrantByPoint(QuadNode& reference, T& data)
    {
        return QuadrantByPoint(reference.x, reference.y, data.x, data.y);
    }

   /*  QuadTree()
    //: elements(maxElements) // set constant size (possibly in template) so no iterators are invalidated // change later
    {

    } */

    // test for scenario where threshold+1 elements are in a corner so that the map divides infinitely, tree hight should be log[figure_radious](map_size)
public:

/*     int height = 200;
    int width = 200; */
    //std::vector<T> elements;
    //const int maxElements = 10000;


    //void add(QuadNode& node, const T& data);

};