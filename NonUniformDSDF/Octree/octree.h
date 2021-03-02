#ifndef OCTREE_H
#define OCTREE_H

#include <algorithm>
#include <cassert>
#include <istream>
#include <ostream>
#include "../vector3d.h"

template< typename T, int P = 2 >
class Octree
{

public:
    // Octree node types
    enum NodeType { BranchNode, LeafNode };

    class Node
    {
    public:
        NodeType type() const;

        int layer() const { return layer_; }
        const Node* parent() const { return parent_; }
        void parent(Node* p) { this->parent_ = p; }
        int indexInParent = -1;

    protected:
        Node(NodeType type, int layer);
        Node(NodeType type);
        ~Node() {};

        int layer_ = -1;
        Node* parent_ = nullptr;

        NodeType type_ = NodeType::LeafNode;
    };


    class Branch : public Node
    {
    public:
        Branch(int layer);
        Branch(const Branch& b);
        ~Branch();

        const Node* child(int x, int y, int z) const;
        const Node* child(int index) const;
        Node* child(int x, int y, int z);
        Node* child(int index);
        void child(int x, int y, int z, Node* newChild);
        void child(int index, Node* newChild);

        friend void Octree<T, P>::deleteNode(Node*& node);

    private:
        Branch& operator= (Branch b);

    private:
        Node* children[2][2][2];
    };


    class Leaf : public Node
    {
    public:
        Leaf(int layer);
        Leaf(const T& v, int layer);

        const T& value() const;
        T& value();
        void setValue(const T& v);
        void subdivide(vector3d<T>& init, vector3d<Leaf*>& newLeaves);

        friend void Octree<T, P>::deleteNode(Node*& node);

    private:
        ~Leaf() {};

    private:
        T value_;
    };

    Octree() { }
    Octree(int size, vector3d<T>& init, vector3d<Leaf*>& generatedLeaves);
    Octree(const Octree<T, P>& o);
    ~Octree();

    Octree<T, P>& operator= (Octree<T, P> o);

    Node*& root();
    const Node* root() const;

    static void deleteNode(Node*& node);

    void packForGPU(std::vector<unsigned int>& branchGPU, std::vector<unsigned int>& leavesGPU, int& branchCount);
    void print(glm::vec3 bboxMin, glm::vec3 bboxMax);

private:
    // Recursive helper functions
    void initLeavesRecursive(Node* currNode, vector3d<T>& init, vector3d<Leaf*>& generatedLeaves, int x, int y, int z);

    Node* root_;
    int size_;
};

#include "octree.hpp"
#include "octreeNode.hpp"
#include "octreeBranch.hpp"
#include "octreeLeaf.hpp"
#include "octreePack.hpp"
#include "octreePrint.hpp"

#endif
