#pragma once
#include <vector>
#include <array>
#include <cstddef>
#include <string>

template <typename T, int size>
struct QuadNode{
    float x{};
    float y{};
    float width{};
    float height{};
    QuadNode<T, size>* children[4] = {this, this, this, this}; // non-owning pointers
    QuadNode<T, size>* parent = this; // non-owning pointer
    std::array<T, size> data;
    size_t elements = 0;
    T* lastElement() { return data.begin() + elements; }
    QuadNode() = default;
    QuadNode(QuadNode<T, size>& nullNode)
    : children{&nullNode, &nullNode, &nullNode, &nullNode}
    , parent{&nullNode}
    {}
    QuadNode(float _x, float _y, float _width, float _height, QuadNode<T, size>& nullNode)
    : x(_x)
    , y(_y)
    , width(_width)
    , height(_height)
    , children{&nullNode, &nullNode, &nullNode, &nullNode}
    , parent{&nullNode}
    {}
    QuadNode(const QuadNode<T, size>& ref) = default;
    QuadNode(QuadNode<T, size>&& ref) = default;
    QuadNode<T, size>& operator=(const QuadNode<T, size>& node) = default;
    QuadNode<T, size>& operator=(QuadNode<T, size>&& node) = default;
    std::string toString() { return std::string("Pos: ").append(std::to_string(x)).append(std::to_string(y)); }
};