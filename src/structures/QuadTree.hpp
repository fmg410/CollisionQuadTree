#pragma once
#include "QuadNode.hpp"
#include "../collision/pos.hpp"

#include <vector>
#include <algorithm>
#include <iterator> // For std::forward_iterator_tag
#include <cstddef>  // For std::ptrdiff_t
#include <functional>
#include <cmath>

/* template <typename T>
bool within(T firstBound, T secoundBound, T value)
{
    if(firstBound < secoundBound)
        return value > firstBound && value < secoundBound;
    return value < firstBound && value > secoundBound;
} */

bool within(float firstBound, float secoundBound, float value)
{
    if(firstBound < secoundBound)
        return value > firstBound && value < secoundBound;
    return value < firstBound && value > secoundBound;
}

template <typename T, unsigned int threshhold = 8>
class QuadTree{

public:
    using QuadNode = QuadNode<T, threshhold>;
    std::vector<QuadNode> nodes;
    QuadNode nullNode;
    QuadNode* freeList = nullptr;
    size_t allElements = 0;

    QuadTree(unsigned int maxNodes = 10000, float x = 0, float y = 0, float width = 100.f, float height = 100.f)
    {
        nodes.reserve(maxNodes);
        QuadNode root(x, y, width, height, nullNode);
        //QuadNode c = std::move(n);
        nodes.push_back(root);
    }

    /* float getDistance(QuadNode& node, float x, float y)
    {
        return std::sqrt(std::pow(node.x - x, 2) + std::pow(node.y - y, 2));
    } */

    float getRootX()
    {
        return nodes.at(0).x;
    }

    float getRootY()
    {
        return nodes.at(0).y;
    }

    float getRootWidth()
    {
        return nodes.at(0).width;
    }

    float getRootHeight()
    {
        return nodes.at(0).height;
    }

    QuadNode& getRoot()
    {
        return nodes.at(0);
    }

    QuadNode& getNullNode()
    {
        return nullNode;
    }

    struct Iterator{
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = QuadNode;
        using pointer           = QuadNode*;
        using reference         = QuadNode&;

        Iterator(pointer ptr) : m_ptr(ptr) {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        // Prefix increment
        Iterator& operator++()
        {
            if(m_ptr->parent == m_ptr->parent->parent)
            {
                m_ptr = m_ptr->parent;
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
                /* if(m_ptr->parent == m_ptr->parent->parent)
                {
                    //m_ptr == m_ptr->parent;
                    return *this;
                } */
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

    Iterator advance(Iterator& it, int steps = 1)
    {
        for(int i = 0; i < steps; i++)
            ++it;
        return it;
    }

    //Iterator begin() { return Iterator(&(*nodes.begin())); }
    Iterator begin()
    {
        QuadNode* n = &(*nodes.begin());
        while(n->children[0] != n->children[1])
            n = n->children[0];
        return Iterator(n);
    }
    Iterator end()   { return Iterator(&nullNode);} // 200 is out of bounds

    void add(const T& elem)
    {
        add(elem, nodes.at(0));
    }

    QuadNode& locateNodeByPosition(QuadNode& node, float x, float y, int depth)
    {
        if(depth > 7)
            volatile int a = 2;
        if(depth > 1000)
            return nullNode;
        if(x > getRootX() + getRootWidth()/2 || x < getRootX() - getRootWidth()/2 || y > getRootY() + getRootHeight()/2 || y < getRootY() - getRootHeight()/2) // outside boundaries
            return nodes.at(0);
        if(node.x + node.width/2 < x || node.x - node.width/2 > x || node.y + node.height/2 < y || node.y - node.height/2 > y)
            return locateNodeByPosition(*(node.parent), x, y, depth + 1);
        if(node.children[0] == node.children[1])
            return node;
        if(&node == &nullNode)
            return node;
        return locateNodeByPosition(*(node.children[QuadrantByPoint(node, x, y)]), x, y, depth + 1);
    }

    QuadNode& locateNodeByPosition(float x, float y)
    {
        return locateNodeByPosition(nodes.at(0), x, y, 0);
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
        QuadNode& n = locateNodeByPosition(nodes.at(0), data.x, data.y, 0);
        if(n.elements < threshhold)
        {
            *(n.lastElement()) = data;
            n.elements++;
            allElements++;
        }
        else
        {
            // if(nodes.size() > 9000)
            //     return;
            divide(n);
            add(data, n);
        }

    }

    bool generateChildren(QuadNode& node)
    {
        if(freeList)
        {
            node.children[0] = freeList;
            node.children[1] = freeList + 1;
            node.children[2] = freeList + 2;
            node.children[3] = freeList + 3;
            freeList = freeList->parent;
            *node.children[0] = QuadNode(node.x + node.width/4, node.y + node.height/4, node.width/2, node.height/2, nullNode);
            node.children[0]->parent = &node;
            *node.children[1] = QuadNode(node.x - node.width/4, node.y + node.height/4, node.width/2, node.height/2, nullNode);
            node.children[1]->parent = &node;
            *node.children[2] = QuadNode(node.x - node.width/4, node.y - node.height/4, node.width/2, node.height/2, nullNode);
            node.children[2]->parent = &node;
            *node.children[3] = QuadNode(node.x + node.width/4, node.y - node.height/4, node.width/2, node.height/2, nullNode);
            node.children[3]->parent = &node;
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
        generateChildren(node);

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
            if(!checkIfLeaf(*(node.children[i])))
                return -1;
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

    bool withinThresholdAll(QuadNode& node)
    {
        if(node.parent == node.parent->parent)
            return false;
        if(!checkIfLeaf(node))
            for(int i = 0; i < 4; i++)
                return withinThresholdAll(*(node.children[i]));
        if(calculateElementsOfNeighbours(node) <= threshhold)
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
        if(node.parent == node.parent->parent)
            return -1;
        if(checkIfLeaf(node))
            for(int i = 0; i < 4; i++)
            {
                sum += (&node - whichChild(node) + i)->elements;
            }
        else
            return -1;
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

    /* void merge(QuadNode& node)
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
    } */

    void merge(QuadNode& node)
    {
        for(int i = 0; i < 4; i++)
            if(!checkIfLeaf(*(node.children[i])))
                merge(*(node.children[i]));

        if(!withinThreshold(node))
            return;

        for(int i = 0; i < 4; i++)
        {
            std::copy_n(node.children[i]->data.begin(), node.children[i]->elements, node.data.begin() + node.elements);
            node.elements += node.children[i]->elements;
        }
        deleteChildren(node);
    }

    bool deleteChildren(QuadNode& node)
    {
        *node.children[0] = QuadNode();
        *node.children[1] = QuadNode();
        *node.children[2] = QuadNode();
        *node.children[3] = QuadNode();
        node.children[0]->parent = freeList;
        freeList = node.children[0];
        node.children[0] = &nullNode;
        node.children[1] = &nullNode;
        node.children[2] = &nullNode;
        node.children[3] = &nullNode;
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
            allElements--;
            /* if(withinThreshold(*node.parent))
                merge(*node.parent); */
        }
        return data;
    }

    bool deleteData(QuadNode& node, size_t index)
    {
        if(index < node.elements)
        {
            std::copy(node.data.begin() + index + 1, node.data.begin() + node.elements, node.data.begin() + index);
            node.elements--;
            /* if(withinThresholdAll(*node.parent))
                merge(*node.parent); */
            allElements--;
            return true;
        }
        return false;
    }

    bool deleteDataFast(QuadNode& node, size_t index)
    {
        if(index < node.elements)
        {
            std::copy(node.data.begin() + index + 1, node.data.begin() + node.elements, node.data.begin() + index);
            node.elements--;
            /* if(withinThresholdNeighbours(*node.parent))
                merge(*node.parent); */
            allElements--;
            return true;
        }
        return false;
    }

    void mergeTree()
    {
        merge(nodes.at(0));
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

    bool verifyDataPosition(QuadNode& node, size_t index)
    {
        if(index < node.elements)
        {
            return !(node.data.at(index).x < node.x - node.width/2 || node.data.at(index).x > node.x + node.width/2 || node.data.at(index).y < node.y - node.height/2 || node.data.at(index).y > node.y + node.height/2);

            /* if(QuadrantByPoint(node, node.data.at(index)) == QuadrantByPoint(node, node.data.at(index), node.data.at(index).x, node.data.at(index).y))
                return true; */
        }
        return false;
    }

    bool correctDataPosition(QuadNode& node, size_t index)
    {
        if(index < node.elements)
        {
            if(!verifyDataPosition(node, index))
            {
                add(getData(node, index));
                deleteData(node, index);
                return true;
            }
        }
        return false;
    }

    bool withinNode(QuadNode& area, QuadNode& check)
    {
        return within(area.x - area.width/2, area.x + area.width/2, check.x) && within(area.y - area.height/2, area.y + area.height/2, check.y);
    }

    Iterator getAfterChildrenIterator(QuadNode& node)
    {
        Iterator itr(&node);
        while(itr != end())
        {
            if(!withinNode(node, *itr))
                return itr;
            itr.operator++();
        }
        return itr;
    }

    bool isSameLevel(QuadNode& first, QuadNode& second)
    {
        return fabs(first.width) == fabs(second.width);
    }

    bool isNullNode(QuadNode& node)
    {
        return &node == &nullNode;
    }

    bool isNeighbour(QuadNode& first, QuadNode& second)
    {
        if(isNullNode(first) || isNullNode(second))
            return false;
        return first.parent == second.parent;
    }

    bool isChild(QuadNode& parent, QuadNode& potentialChild)
    {
        return withinNode(parent, potentialChild);
    }

    bool isNextTo(QuadNode& first, QuadNode& second)
    {
        if(isNullNode(first) || isNullNode(second))
            return false;
        if(first.x + first.width/2 + second.width/2 == second.x || second.x + second.width/2 + first.width/2 == first.x)
            return true;
        if(first.y + first.height/2 + second.height/2 == second.y || second.y + second.height/2 + first.height/2 == first.y)
            return true;
        return false;
    }

    bool checkFunctionInNode(QuadNode& node, float a, float b)
    {
        if(within(node.y - node.height/2, node.y + node.height/2, a * node.x + b))
            return true;
        if(within(node.y - node.height/2, node.y + node.height/2, a * (node.x - node.width/2) + b))
            return true;
        if(within(node.y - node.height/2, node.y + node.height/2, a * (node.x + node.width/2) + b))
            return true;
        if(within(node.x - node.height/2, node.x + node.height/2, (node.y - b) / 2))
            return true;
        if(within(node.x - node.height/2, node.x + node.height/2, (node.y - node.height/2 - b) / 2))
            return true;
        if(within(node.x - node.height/2, node.x + node.height/2, (node.y + node.height/2 - b) / 2))
            return true;

        return false;
    }

    void updateElement(QuadNode& node, size_t index, std::function<void(QuadNode&)> func)
    {
        if(index < node.elements)
        {
            func(node.data.at(index));
        }
    }

    void updateElements(QuadNode& node, std::function<void(QuadNode&)> func)
    {
        for(int i = 0; i < node.elements; i++)
        {
            func(node.data.at(i));
        }
    }

    bool moveElement(QuadNode& node, size_t index, QuadNode& target)
    {
        if(index < node.elements)
        {
            *(target.lastElement()) = std::move(node.data.at(index));
            target.elements++;
            std::copy(node.data.begin() + index + 1, node.data.begin() + node.elements, node.data.begin() + index);
            node.elements--;
            if(withinThreshold(*node.parent))
                merge(*node.parent);
            return true;
        }
        return false;
    }

    bool moveElementFast(QuadNode& node, size_t index, QuadNode& target)
    {
        if(index < node.elements)
        {
            *(target.lastElement()) = std::move(node.data.at(index));
            target.elements++;
            std::copy(node.data.begin() + index + 1, node.data.begin() + node.elements, node.data.begin() + index);
            node.elements--;
            if(withinThresholdNeighbours(getFirstOfChildren(node)))
                merge(*node.parent);
            return true;
        }
        return false;
    }

    bool moveElement(QuadNode& node, size_t index, QuadNode& target, size_t targetIndex)
    {
        if(index < node.elements && targetIndex < target.elements)
        {
            *(target.lastElement()) = std::move(node.data.at(index));
            target.elements++;
            std::copy(node.data.begin() + index + 1, node.data.begin() + node.elements, node.data.begin() + index);
            node.elements--;
            if(withinThreshold(*node.parent))
                merge(*node.parent);
            return true;
        }
        return false;
    }

    std::vector<QuadNode*> getNodesInArea(QuadNode& node, float x, float y, float width, float height) // ?
    {
        /* std::vector<QuadNode*> nodes;
        if(node.x + node.width/2 < x || node.x - node.width/2 > x + width || node.y + node.height/2 < y || node.y - node.height/2 > y + height)
            return nodes;
        if(node.children[0] == node.children[1])
        {
            nodes.push_back(&node);
            return nodes;
        }
        for(int i = 0; i < 4; i++)
        {
            std::vector<QuadNode*> temp = getNodesInArea(*(node.children[i]), x, y, width, height);
            nodes.insert(nodes.end(), temp.begin(), temp.end());
        }
        return nodes; */
        std::vector<QuadNode*> nodes;

        if(x + width/2 < node.x + node.width/2 && x - width/2 > node.x - node.width/2 && y + height/2 < node.y + node.height/2 && y - height/2 > node.y - node.height/2)
        {
            nodes.push_back(&node);
            return nodes;
        }

        // rozrysowac i policzyc, mozna lokalizowac narozniki, a glebokosc liczyc arytmetycznie, a nie gonic wskazniki
        return nodes;
    }

    /* std::vector<QuadNode*> getNodesInArea(QuadNode& node, const T& data) // ?
    {
        std::vector<QuadNode*> nodesReturn;

        for(int i = 0; i < T::size(); i++)
        {
            QuadNode& n = locateNodeByPosition(node, (data.cbegin() + i)->x, (data.cbegin() + i)->y, 0);
            if(std::find(nodesReturn.begin(), nodesReturn.end(), &n) == nodesReturn.end())
                nodesReturn.push_back(&n);
        }

        // rozrysowac i policzyc, mozna lokalizowac narozniki, a glebokosc liczyc arytmetycznie, a nie gonic wskazniki
        return nodesReturn;
    } */

    int levelDifference(QuadNode& first, QuadNode& second)
    {
        float multiplayer;
        if (second.width < first.width)
            return lround(log2(first.width / second.width));
        else if(first.width < second.width)
            return -lround(log2(second.width / first.width));
        else
            return 0;
    }

    std::vector<QuadNode*> getNodesInArea(QuadNode& node, const T& data) // dorzucic sprawdzenie dla srodkowego punktu czy sasiaduje z wierzcholkami
    {
        std::vector<QuadNode*> nodesReturn;

        for(int i = 0; i < T::size(); i++)
        {
            pos v1 = *(data.cbegin() + i);
            QuadNode* n1 = &(locateNodeByPosition(node, (data.cbegin() + i)->x, (data.cbegin() + i)->y, 0));
            QuadNode* n2 = &(i != T::size() - 1 ? locateNodeByPosition(node, (data.cbegin() + i + 1)->x, (data.cbegin() + i + 1)->y, 0) : locateNodeByPosition(node, (data.cbegin())->x, (data.cbegin())->y, 0));
            if(isSameLevel(*n1, *n2) && (n1 == n2 || isNextTo(*n1, *n2)))
            {
                if(std::find(nodesReturn.begin(), nodesReturn.end(), n1) == nodesReturn.end())
                    nodesReturn.push_back(n1);
                continue;
            }

            pos v2 = *(i != T::size() - 1 ? (data.cbegin() + i + 1) : (data.cbegin()));
            float a = (v2.y - v1.y) / (v2.x - v1.x);
            float b = v1.y - a * v1.x;
            int levelDiff = levelDifference(*n1, *n2);
            while(levelDiff < 0)
            {
                n1 = n1->parent;
                levelDiff++;
            }
            while(levelDiff > 0)
            {
                n2 = n2->parent;
                levelDiff--;
            }

            while(n1 != n2 && !isNextTo(*n1, *n2))
            {
                n1 = n1->parent;
                n2 = n2->parent;
            }

            for(Iterator itr(n1); itr != getAfterChildrenIterator(*n1); itr.operator++())
            {
                if(checkFunctionInNode(*itr, a, b))
                {
                    if(std::find(nodesReturn.begin(), nodesReturn.end(), &(*itr)) == nodesReturn.end())
                        nodesReturn.push_back(&(*itr));
                }
            }

            for(Iterator itr(n2); itr != getAfterChildrenIterator(*n2); itr.operator++())
            {
                if(checkFunctionInNode(*itr, a, b))
                {
                    if(std::find(nodesReturn.begin(), nodesReturn.end(), &(*itr)) == nodesReturn.end())
                        nodesReturn.push_back(&(*itr));
                }
            }

        }

        // rozrysowac i policzyc, mozna lokalizowac narozniki, a glebokosc liczyc arytmetycznie, a nie gonic wskazniki
        return nodesReturn;
    }

    std::vector<QuadNode*> getNodesInArea(float x, float y, float width, float height)
    {
        return getNodesInArea(nodes.at(0), x, y, width, height);
    }

    std::vector<T> getElementsInArea(QuadNode& node, float x, float y, float width, float height)
    {
        std::vector<T> elements;
        if(node.x + node.width/2 < x || node.x - node.width/2 > x + width || node.y + node.height/2 < y || node.y - node.height/2 > y + height)
            return elements;
        if(node.children[0] == node.children[1])
        {
            for(int i = 0; i < node.elements; i++)
            {
                if(node.data.at(i).x >= x && node.data.at(i).x <= x + width && node.data.at(i).y >= y && node.data.at(i).y <= y + height)
                    elements.push_back(node.data.at(i));
            }
            return elements;
        }
        for(int i = 0; i < 4; i++)
        {
            std::vector<T> temp = getElementsInArea(*(node.children[i]), x, y, width, height);
            elements.insert(elements.end(), temp.begin(), temp.end());
        }
        return elements;
    }

    std::vector<T> getElementsInArea(float x, float y, float width, float height) //getElementsInNode
    {
        return getElementsInArea(nodes.at(0), x, y, width, height);
    }

    std::vector<T> getElementsInArea(QuadNode& node, QuadNode& area)
    {
        return getElementsInArea(node, area.x, area.y, area.width, area.height);
    }

    std::vector<T> getElementsInArea(QuadNode& area)
    {
        return getElementsInArea(nodes.at(0), area.x, area.y, area.width, area.height);
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

    bool before(const Iterator& a, const Iterator& b)
    {
        auto& node1 = *a;
        auto& node2 = *b;
        // if(node1.x < node2.x)
        //     return true;
        float currentX = getRootX();
        float currentY = getRootY();
        float currentWidth = getRootWidth();
        float currentHeight = getRootHeight();

        if(a == b)
            return false;

        while(true)
        {
            int quadrant1 = QuadrantByPoint(currentX, currentY, node1);
            int quadrant2 = QuadrantByPoint(currentX, currentY, node2);
            if(quadrant1 < quadrant2)
                return true;
            else if(quadrant1 > quadrant2)
                return false;
            else
            {
                if(quadrant1 == 0)
                {
                    currentX += currentWidth/4;
                    currentY += currentHeight/4;
                }
                else if(quadrant1 == 1)
                {
                    currentX -= currentWidth/4;
                    currentY += currentHeight/4;
                }
                else if(quadrant1 == 2)
                {
                    currentX -= currentWidth/4;
                    currentY -= currentHeight/4;
                }
                else if(quadrant1 == 3)
                {
                    currentX += currentWidth/4;
                    currentY -= currentHeight/4;
                }
                currentWidth /= 2;
                currentHeight /= 2;
            }
        }


        std::cout << "Error in before function" << std::endl; // should never happen REMOVE_LATER
        return false;
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