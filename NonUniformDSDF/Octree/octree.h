#ifndef OCTREE_H
#define OCTREE_H

#include <algorithm>
#include <cassert>
#include <istream>
#include <ostream>
#include <memory>
#include <iostream>
#include <glm/glm.hpp>

#include "../vector3d.h"

template< typename T, int P = 2 >
class Octree
{

public:
    // Octree node types
    enum NodeType { BranchNode, LeafNode };

    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        NodeType type() const;

        int layer() const { return layer_; }
        const std::shared_ptr<Node> parent() const { return parent_; }
        void parent(std::shared_ptr<Node> p) { std::swap(p, parent_); }
        int indexInParent = -1;
        virtual ~Node() {}

    protected:
        Node(NodeType type, int layer);
        Node(NodeType type);

        Node(const Node& b);
        Node& operator= (const Node& b);
    	
        int layer_ = -1;
        std::shared_ptr<Node> parent_ = nullptr;

        NodeType type_ = NodeType::LeafNode;
    };


    class Branch : public Node
    {
    public:
        Branch(int layer);
        Branch(const Branch& b);
        Branch& operator= (const Branch& b);
        ~Branch() final = default;

        const std::shared_ptr<Node> child(int x, int y, int z) const;
        const std::shared_ptr<Node> child(int index) const;
        std::shared_ptr<Node> child(int x, int y, int z);
        std::shared_ptr<Node> child(int index);
        void child(int x, int y, int z, std::shared_ptr<Node> newChild);
        void child(int index, std::shared_ptr<Node> newChild);


    private:
        std::shared_ptr<Node> children[2][2][2];
    };


    class Leaf : public Node
    {
    public:
        Leaf(int layer);
        Leaf(const T& v, int layer);

        Leaf(const Leaf& b);
        Leaf& operator=(const Leaf& b);

        const T& value() const;
        T& value();
        void setValue(const T& v);
        void subdivide(vector3d<T>& init, vector3d<std::shared_ptr<Leaf>>& newLeaves);

        ~Leaf() final = default;

    private:
        T value_;
    };

    Octree() { }
    Octree(int size, vector3d<T>& init, vector3d<std::shared_ptr<Leaf>>& generatedLeaves);
    ~Octree();

    Octree(const Octree<T, P>& o);
    Octree<T, P>& operator= (const Octree<T, P>& o);

    std::shared_ptr<Node>& root();
    const std::shared_ptr<Node> root() const;

    void packForGPU(std::vector<unsigned int>& branchGPU, std::vector<unsigned int>& leavesGPU, int& branchCount, float& compressAmount);
    void print(glm::vec3 bboxMin, glm::vec3 bboxMax);

private:
    // Recursive helper functions
    void initLeavesRecursive(std::shared_ptr<Node>& currNode, vector3d<T>& init, vector3d<std::shared_ptr<Leaf>>& generatedLeaves, int x, int y, int z);

    std::shared_ptr<Node> root_;
};

#include "octree.hpp"
#include "octreeNode.hpp"
#include "octreeBranch.hpp"
#include "octreeLeaf.hpp"
#include "octreePack.hpp"
#include "octreePrint.hpp"

#endif
