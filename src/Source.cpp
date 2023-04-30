#include "structures/QuadNode.hpp"
#include <array>
#include <iostream>
#include "structures/QuadTree.hpp"

struct figure{
    float x = 0;
    float y = 0;
};

int main()
{
    //QuadNode<figure, 8> node;
    std::array<int, 5> a;
    a.at(0) = 3;

    //std::cout << a.size() << " " << a.max_size();
    QuadTree<figure> tree;
    /* figure f{12, 3};
    tree.add(f); */
    for(int i = 0; i < 8; i++) // breaks starting at 10
    {
        tree.add(figure{float(i), float(i+5)});
    }
    tree.add(figure{float(8), float(8+5)});
    auto node = tree.locateNodeByPosition(tree.nodes.at(0), 10, 5);
    std::cout << node.toString();

}