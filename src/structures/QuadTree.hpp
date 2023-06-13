#pragma once
#include "QuadNode.hpp"

#include <vector>
#include <algorithm>
#include <iterator> // For std::forward_iterator_tag
#include <cstddef>  // For std::ptrdiff_t


template <typename T>
class QuadTree{

public:

    static const int threshhold = 8;
    using QuadNode = QuadNode<T, threshhold>;
    std::vector<QuadNode> nodes;
    QuadNode nullNode;
    QuadNode* freeList = nullptr;
    int test = 0;

    QuadTree()
    {
        nodes.reserve(10000);
        QuadNode n(0.f, 0.f, 100.f, 100.f, nullNode);
        //QuadNode c = std::move(n);
        nodes.push_back(n);
    }

    struct Iterator{
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = QuadNode;
        using pointer           = QuadNode*;  // or also value_type*
        using reference         = QuadNode&;  // or also value_type&

        Iterator(pointer ptr) : m_ptr(ptr) {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        // Prefix increment
        Iterator& operator++()
        {
            if(m_ptr->parent == m_ptr->parent->parent)
            {
                m_ptr == m_ptr->parent;
                return *this;
            }

            if(std::find(m_ptr->parent->children, m_ptr->parent->children + 4, m_ptr) != m_ptr->parent->children + 3)
            {
                m_ptr++;
                while(m_ptr->children[0] != m_ptr->children[1])
                    m_ptr = m_ptr->children[0];
                return *this;
            }

            while(std::find(m_ptr->parent->children, m_ptr->parent->children + 4, m_ptr) == m_ptr->parent->children + 3) // TODO replace children with std::array !!!!
            {
                if(m_ptr->parent == m_ptr->parent->parent)
                {
                    m_ptr == m_ptr->parent;
                    return *this;
                }
                m_ptr = m_ptr->parent;
            }

            //return this->operator++();

            if(m_ptr->parent == m_ptr->parent->parent)
            {
                m_ptr = m_ptr->parent;
                return *this;
            }

            if(std::find(m_ptr->parent->children, m_ptr->parent->children + 4, m_ptr) != m_ptr->parent->children + 3) // well this one was missing
            {
                m_ptr++;
                while(m_ptr->children[0] != m_ptr->children[1])
                    m_ptr = m_ptr->children[0];
                return *this;
            }

            m_ptr++;
            return *this;

        }

        // Postfix increment
        //Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

    private:

        pointer m_ptr;
    };

    //Iterator begin() { return Iterator(&(*nodes.begin())); }
    Iterator begin()
    {
        QuadNode* n = &(*nodes.begin());
        while(n->children[0] != n->children[1])
            n = n->children[0];
        return Iterator(n);
    }
    Iterator end()   { return Iterator(&nullNode); } // 200 is out of bounds

    void add(const T& elem)
    {
        add(elem, nodes.at(0));
    }

    QuadNode& locateNodeByPosition(QuadNode& node, float x, float y)
    {
        if(node.x + node.width/2 < x || node.x - node.width/2 > x || node.y + node.height/2 < y || node.y - node.height/2 > y)
            return locateNodeByPosition(*(node.parent), x, y);
        if(node.children[0] == node.children[1])
            return node;
        /* int child = (x < node.x) + (y < node.y) * 3;
        child -= (child / 4) * 2;
        return locateNodeByPosition(*(node.children[child]), x, y); */
        return locateNodeByPosition(*(node.children[QuadrantByPoint(node, x, y)]), x, y);
    }

    bool contains(QuadNode& node, const T& val)
    {
        QuadNode& n = locateNodeByPosition(node, val.x, val.y);
        if(std::find(n.data.begin(), n.data.begin() + n.elements, val) != n.data.begin() + n.elements)
            return true;
        return false;
    }

    bool contains(const T& val)
    {
        return contains(nodes.at(0), val);
    }

    /* bool containsAt(QuadNode& node, float x, float y)
    {
        QuadNode& n = locateNodeByPosition(node, x, y);
        if(std::find_if(n.data.begin(), n.data.begin() + n.elements, [x, y](const T& d){ return d.x == x && d.y == y; }) != n.data.begin() + n.elements)
            return true;
        return false;
    }
    {
        QuadNode& n = locateNodeByPosition(node, val.x, val.y);
        if(std::find(n.data.begin(), n.data.begin() + n.elements, val) != n.data.begin() + n.elements)
            return true;
        return false;
    } */



    void add(const T& data, QuadNode& node)
    {
        QuadNode& n = locateNodeByPosition(nodes.at(0), data.x, data.y);
        if(n.elements < threshhold)
        {
            *(n.lastElement()) = data;
            n.elements++;
            test++;
        }
        else
        {
            divide(n);
            add(data, n);
        }

    }

    bool generateChildren(QuadNode& node)
    {
        if(freeList)
        {
            node.children[0] = freeList;
            node.children[0]->parent = &node;
            node.children[1] = freeList + 1;
            node.children[1]->parent = &node;
            node.children[2] = freeList + 2;
            node.children[2]->parent = &node;
            node.children[3] = freeList + 3;
            node.children[3]->parent = &node;
            freeList = freeList->parent;
            return true;
        }
        node.children[0] = &(nodes.emplace_back(node.x + node.width/4, node.y + node.height/4, node.width/2, node.height/2, nullNode));
        node.children[1] = &(nodes.emplace_back(node.x - node.width/4, node.y + node.height/4, node.width/2, node.height/2, nullNode));
        node.children[2] = &(nodes.emplace_back(node.x - node.width/4, node.y - node.height/4, node.width/2, node.height/2, nullNode));
        node.children[3] = &(nodes.emplace_back(node.x + node.width/4, node.y - node.height/4, node.width/2, node.height/2, nullNode));
        node.children[0]->parent = &node;
        node.children[1]->parent = &node;
        node.children[2]->parent = &node;
        node.children[3]->parent = &node;
        return true;
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

        node.elements = 0;

        return true;
    }

    size_t calculateElementsOfChildren(QuadNode& node)
    {
        size_t sum = 0;
        if(node.children[0] == node.children[1])
            return 0;
        for(int i = 0; i < 4; i++)
        {
            sum += node.children[i]->elements;
        }
        return sum;
    }

    bool withinThreshold(QuadNode& node)
    {
        if(node.children[0] == node.children[1])
            return false;
        if(calculateElementsOfChildren(node) <= threshhold)
            return true;
        return false;
    }

    int whichChild(QuadNode& node)
    {
        if(node.children[0] == node.children[1])
            return -1;
        unsigned int difference = &node - &nodes.at(0) - 1;
        difference %= 4;
        return difference;
    }

    QuadNode& getFirstOfChildren(QuadNode& node)
    {
        if(node.children[0] == node.children[1])
            return node;
        unsigned int difference = &node - &nodes.at(0) - 1;
        difference %= 4;
        return *(&node - difference);
    }

    size_t calculateElementsOfNeighbours(QuadNode& node)
    {
        size_t sum = 0;
        if(node.children[0] == node.children[1])
            return 0;
        for(int i = 0; i < 4; i++)
        {
            sum += (&node + i)->elements;
        }
        return sum;
    }

    bool withinThresholdNeighbours(QuadNode& node)
    {
        if(node.children[0] == node.children[1])
            return false;
        if(calculateElementsOfNeighbours(node) <= threshhold)
            return true;
        return false;
    }

    bool checkIfLeaf(QuadNode& node)
    {
        if(node.children[0] == node.children[1])
            return true;
        return false;
    }

    void merge(QuadNode& node)
    {
        if(!checkIfLeaf(node))
        {
            for(int i = 0; i < 4; i++)
            {
                merge(*(node.children[i]));
            }
        }
        for(int i = 0; i < 4; i++)
        {
            std::copy_n(node.children[i]->data.begin(), node.children[i]->elements, node.data.begin() + node.elements);
            node.elements += node.children[i]->elements;
            deleteChildren(node);
        }
    }

    bool deleteChildren(QuadNode& node)
    {
        node.children[0] = QuadNode();
        node.children[1] = QuadNode();
        node.children[2] = QuadNode();
        node.children[3] = QuadNode();
        node.children[0]->parent = freeList;
        freeList = node.children[0];
        return true;
    }

    T getData(QuadNode& node, size_t index)
    {
        if(index < node.elements)
            return node.data.at(index);
        return T();
    }

    T removeData(QuadNode& node, size_t index)
    {
        T data;
        if(index < node.elements)
        {
            data = node.data.at(index);
            std::copy(node.data.begin() + index + 1, node.data.begin() + node.elements, node.data.begin() + index);
            node.elements--;
            if(withinThreshold(*node.parent))
                merge(*node.parent);
        }
        return data;
    }

    T removeDataFast(QuadNode& node, size_t index)
    {
        T data;
        if(index < node.elements)
        {
            data = node.data.at(index);
            std::copy(node.data.begin() + index + 1, node.data.begin() + node.elements, node.data.begin() + index);
            node.elements--;
            if(withinThresholdNeighbours(getFirstOfChildren(node)))
                merge(*node.parent);
        }
        return data;
    }

    /* bool deleteChildren(QuadNode& node)
    {
        for(auto&& child : node.children)
            if(!checkIfLeaf(child))
            {
                for(int i = 0; i < 4; i++)
                {
                    deleteChildren(*(node.children[i]));
                }
            }
        node.children[0] = QuadNode();
        node.children[1] = QuadNode();
        node.children[2] = QuadNode();
        node.children[3] = QuadNode();
        node.children[0]->parent = freeList;
        freeList = node.children[0];
        return true;
    } */

    // return range [0, 3]
    int QuadrantByPoint(float centerX, float centerY, float posX, float posY)
    {
        /* int quadrant = (centerX < posX) + (centerY < posY) * 3;
        quadrant -= (quadrant / 4) * 2;
        return quadrant; */
        if(posX >= centerX && posY > centerY)
            return 0;
        else if(posX < centerX && posY >= centerY)
            return 1;
        else if(posX <= centerX && posY < centerY)
            return 2;
        return 3;
    }

    int QuadrantByPoint(float centerX, float centerY, QuadNode& node)
    {
        return QuadrantByPoint(centerX, centerY, node.x, node.y);
    }

    int QuadrantByPoint(QuadNode& center, float posX, float posY)
    {
        return QuadrantByPoint(center.x, center.y, posX, posY);
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