#include "Tree.hpp"
#include "Vector.hpp"

int main()
{
    if(SIMULATE_BOTH)
    {
        testTree();
        testVector();
    }
    else
    {
        if(SIMULATE_TREE)
            testTree();
        else
            testVector();
    }

}