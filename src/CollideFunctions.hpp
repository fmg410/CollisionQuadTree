#ifndef COLLIDE_FUNCTIONS_HPP
#define COLLIDE_FUNCTIONS_HPP

#include "Settings.hpp"

template<typename T, unsigned int treshhold>
void collideTreeOld(QuadTree<T, treshhold>& tree, std::function<bool(T&, T&, float)> collider, uint64_t& collisionCount, unsigned int steps = 1)
{
    for(int m = 0; m < steps; m++)
            {
                for(auto& node : tree)
                    for(int i = 0; i < node.elements; i++)
                    {
                        updateSpeed(node.data.at(i), 1.f / float(steps));
                        applyBoundariesNode(node.data.at(i), tree.getRootX(), tree.getRootY(), tree.getRootWidth(), tree.getRootHeight());
                    }
                for(auto& node : tree)
                {
                    for(int i = 0; i < node.elements; i++)
                    {
                        if(node.data.at(i).collisionChecked)
                            continue;
                        else
                            node.data.at(i).collisionChecked = true;

                        auto list = tree.getNodesInArea(node, node.data.at(i));
                        for(auto& otherNodes : list)
                        {
                            for(int j = 0; j < otherNodes->elements; j++)
                                if(node.data.at(i) != otherNodes->data.at(j))
                                {
                                    if(std::invoke(collider, node.data.at(i), otherNodes->data.at(j), 1.f / float(steps)))
                                    {
                                        collisionCount++;
                                        applyBoundariesNode(node.data.at(i), tree.getRootX(), tree.getRootY(), tree.getRootWidth(), tree.getRootHeight());
                                        applyBoundariesNode(otherNodes->data.at(j), tree.getRootX(), tree.getRootY(), tree.getRootWidth(), tree.getRootHeight());
                                        tree.correctDataPosition(*otherNodes, j);
                                    }
                                }
                        }
                    }
                    for(int i = 0; i < node.elements; i++)
                    {
                        if(tree.correctDataPosition(node, i))
                            i--;
                    }
                }
            }
            tree.mergeTree();

            for(auto& node : tree)
                for(int i = 0; i < node.elements; i++)
                {
                    node.data.at(i).collisionChecked = false;
                    node.data.at(i).displace();
                }
}

template<typename T, unsigned int treshhold>
void collideTree(QuadTree<T, treshhold>& tree, std::function<bool(T&, T&, float)> collider, uint64_t& collisionCount, unsigned int steps = 1)
{
    std::vector<T*> threeFigs;
    for(int m = 0; m < steps; m++)
    {
        for(auto& node : tree)
            for(int i = 0; i < node.elements; i++) // do optymalizacji przejściem przez vector TODO
            {
                updateSpeed(node.data.at(i), 1.f / float(steps));
                applyBoundariesNode(node.data.at(i), tree.getRootX(), tree.getRootY(), tree.getRootWidth(), tree.getRootHeight());
                tree.correctDataPosition(node, i);
            }
        for(auto& node : tree)
        {
            for(int i = 0; i < node.elements; i++)
            {
                auto& fig = node.data.at(i);
                if(fig.x + fig.R < node.x + node.width / 2 && fig.x - fig.R > node.x - node.width / 2 && fig.y + fig.R < node.y + node.height / 2 && fig.y - fig.R > node.y - node.height / 2) // within boundaries of node
                {
                    fig.collisionChecked = true;

                    for(int j = i + 1; j < node.elements; j++)
                    {
                        if(std::invoke(collider, fig, node.data.at(j), 1.f / float(steps)))
                        {
                            collisionCount++;
                        }
                    }
                }

                auto list = tree.getNodesInArea(node, node.data.at(i)); // dla dwójek
                for(auto otherNodes : list)
                {
                    if(&node == otherNodes)
                        continue;
                    if(tree.before(tree.itrOfNode(node), tree.itrOfNode(*otherNodes))) // jak widzisz późniejszego - kolidujesz
                    {
                        for(int j = 0; j < otherNodes->elements; j++)
                            if(node.data.at(i) != otherNodes->data.at(j)) // po co ten check????
                            {
                                if(std::invoke(collider, node.data.at(i), otherNodes->data.at(j), 1.f / float(steps)))
                                {
                                    collisionCount++;
                                }
                            }
                    }
                    if(!tree.before(tree.itrOfNode(node), tree.itrOfNode(*otherNodes))) // jak widzisz wcześniejszego - sprawdzasz czy cię widzi:
                    {
                        for(int j = 0; j < otherNodes->elements; j++)
                        {
                            auto othersList = tree.getNodesInArea(*otherNodes, otherNodes->data.at(j));
                            if(std::find(othersList.begin(), othersList.end(), &node) == othersList.end()) // jak nie - kolidujesz
                            {
                                if(std::invoke(collider, node.data.at(i), otherNodes->data.at(j), 1.f / float(steps)))
                                {
                                    collisionCount++;
                                }
                            }
                        }
                    }
                }

                if(list.size() > 2) // dla trójek
                {
                    threeFigs.push_back(&fig);
                }

                // for(int i = 0; i < node.elements; i++)
                // {
                //     if(tree.correctDataPosition(node, i))
                //         i--;
                // }
            }
        }

    }

    for(int i = 0; i < threeFigs.size() - 1; i++)
    {
        for(int j = i + 1; j < threeFigs.size(); j++)
        {
            if(std::invoke(collider, *(threeFigs.at(i)), *(threeFigs.at(j)), 1.f / float(steps)))
            {
                collisionCount++;
            }
        }
    }

    for(auto& node : tree)
        for(int i = 0; i < node.elements; i++)
        {
            node.data.at(i).collisionChecked = false;
            node.data.at(i).displace();
            applyBoundariesNode(node.data.at(i), tree.getRootX(), tree.getRootY(), tree.getRootWidth(), tree.getRootHeight());
            tree.correctDataPosition(node, i);
        }
    tree.mergeTree();
}

template<typename T>
void collideVector(std::vector<T>& vec, std::function<bool(T&, T&, float)> collider, uint64_t& collisionCount, unsigned int steps = 1)
{
    for(int m = 0; m < steps; m++)
            {
                for(int i = 0; i < vec.size(); i++)
                {
                    updateSpeed(vec.at(i), 1.f / steps);
                    applyBoundariesNode(vec.at(i), INITIAL_X, INITIAL_Y, AREA_WIDTH, AREA_HEIGHT);
                }
                for(int i = 0; i < vec.size(); i++)
                {
                    for(int j = i + 1; j < vec.size(); j++)
                    {
                        /* if((vec.at(i).x - vec.at(j).x) * (vec.at(i).x - vec.at(j).x) + (vec.at(i).y - vec.at(j).y) * (vec.at(i).y - vec.at(j).y) > 800000.f)
                            continue; */
                        if(std::invoke(collider, vec.at(i), vec.at(j), 1.f / steps))
                        {
                            collisionCount++;
                            applyBoundariesNode(vec.at(i), INITIAL_X, INITIAL_Y, AREA_WIDTH, AREA_HEIGHT);
                            applyBoundariesNode(vec.at(j), INITIAL_X, INITIAL_Y, AREA_WIDTH, AREA_HEIGHT);
                        }
                    }
                }
            }
}

#endif // COLLIDE_FUNCTIONS_HPP