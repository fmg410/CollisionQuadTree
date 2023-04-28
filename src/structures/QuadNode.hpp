#include <vector>


template <typename T>
struct QuadNode{
    float x{};
    float y{};
    float width{};
    float height{};
    QuadNode<T>* children[4] = {this, this, this, this};
    QuadNode<T>* parent = this;
    std::vector<T>::iterator begin;
    std::vector<T>::iterator end;
    /* T* begin{};
    T* end{}; */
    QuadNode() = default;
    QuadNode(QuadNode<T>& nullNode)
    : children{&nullNode, &nullNode, &nullNode, &nullNode}
    , parent{&nullNode}
    {}
    QuadNode(float _x, float _y, float _width, float _height, QuadNode<T>& nullNode)
    : QuadNode(nullNode)
    , x(_x)
    , y(_y)
    , width(_width)
    , height(_height)
    {}
};