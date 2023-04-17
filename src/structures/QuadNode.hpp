


template <typename T>
struct QuadNode{

    QuadNode* children[4];
    QuadNode* parent;
    T* begin;
    T* end;
};