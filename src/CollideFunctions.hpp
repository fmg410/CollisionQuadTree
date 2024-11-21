#ifndef COLLIDE_FUNCTIONS_HPP
#define COLLIDE_FUNCTIONS_HPP

#include "Settings.hpp"

template<typename T, unsigned int treshhold>
void collideTree(QuadTree<T, treshhold>& tree, std::function<bool(T&, T&, float)> collider, uint64_t& collisionCount, unsigned int steps = 1)
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