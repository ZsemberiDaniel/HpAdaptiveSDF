#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <cstring>
#include <queue>

/**
 * \param size Size of octree, in nodes. Should be a power of two. For example,
 * an octree with \a size = 256 will represent a cube divided into 256x256x256
 * nodes. <b>Must be a power of two.</b>
 * 
 * \param init a size x size x size 3D array with which the tree can be initialized.
 */
template< typename T, int P >
Octree<T, P>::Octree(int size, vector3d<T>& init, vector3d<std::shared_ptr<Leaf>>& generatedLeaves)
    : root_(0)
{
    // Make sure size is power of two.
    assert(((size - 1) & size) == 0);
    assert(init.dim1() == size);
    assert(init.dim2() == size);
    assert(init.dim3() == size);

    if (size <= 1) 
    {
        root_ = std::make_shared<Leaf>(0);
        return;
    }
    else 
    {
        root_ = std::make_shared<Branch>(0);
    }

    // push the branch nodes into a queue and take them out to add children to them
    // while we reach the leaves layer
    std::queue<std::shared_ptr<Branch>> nodes;
    nodes.push(std::dynamic_pointer_cast<Branch>(root_));

    int layerAt = 0;
    while (size > 2)
    {
        while (nodes.front()->layer() == layerAt)
        {
            std::shared_ptr<Branch> n = nodes.front(); nodes.pop();
            
            for (int x = 0; x < 2; x++)
            {
                for (int y = 0; y < 2; y++)
                {
                    for (int z = 0; z < 2; z++)
                    {
                        n->child(x, y, z, std::static_pointer_cast<Node>(std::make_shared<Branch>(n->layer() + 1)));
                    }
                }
            }
        }

        layerAt++;
        size /= 2;
    }

    // reached the leaves layer so add leaves to the currently lowest branches
    initLeavesRecursive(root(), init, generatedLeaves, 0, 0, 0);
}
template< typename T, int P >
void Octree<T, P>::initLeavesRecursive(std::shared_ptr<Node>& currNode, vector3d<T>& init, vector3d<std::shared_ptr<Leaf>>& generatedLeaves, int x, int y, int z)
{
    if (currNode == nullptr || currNode->type() == NodeType::LeafNode)
    {
        return;
    }

    std::shared_ptr<Branch> currBranch = std::dynamic_pointer_cast<Branch>(currNode);
    for (int i = 0; i < 2; i++)
    {
        for (int k = 0; k < 2; k++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (currBranch->child(i, k, j) == nullptr)
                {
                    // int index = (2 * x + i) + size_ * (2 * y + k) + size_ * size_ * (2 * z + j);
                    generatedLeaves(2 * x + i, 2 * y + k, 2 * z + j) = 
                        std::make_shared<Leaf>(
	                        init(2 * x + i, 2 * y + k, 2 * z + j),
	                        currBranch->layer() + 1
                        );
                    currBranch->child(i, k, j, generatedLeaves(2 * x + i, 2 * y + k, 2 * z + j));
                }
                else
                {
                    auto child = currBranch->child(i, k, j);
                    initLeavesRecursive(child, init, generatedLeaves, 2 * x + i, 2 * y + k, 2 * z + j);
                }
            }
        }
    }
}

/**
 * Performs a deep copy of an octree. All branch pointers will be followed
 * recursively and new nodes will be allocated.
 *
 * \param o Octree to be copied.
 */
template< typename T, int P >
Octree<T, P>::Octree(const Octree<T, P>& o)
{
    if (!o.root_) 
    {
        root_ = nullptr;
    }
    else 
    {
		root_ = o.root_;
    }
}

/**
 * Recursively deletes all nodes by following branch pointers.
 */
template< typename T, int P >
Octree<T, P>::~Octree() = default;

/**
 * Assigns to this octree the contents of octree \a o.
 */
template< typename T, int P >
Octree<T, P>& Octree<T, P>::operator= (const Octree<T, P>& o)
{
    if (this != &o) // not a self-assignment
    {
        root_ = o.root_;
    }
    return *this;
}

/**
 * \return Pointer to octree's root node.
 */
template< typename T, int P >
std::shared_ptr<typename Octree<T, P>::Node>& Octree<T, P>::root()
{
    return root_;
}

/**
 * Const version of above.
 */
template< typename T, int P >
const std::shared_ptr<typename Octree<T, P>::Node> Octree<T, P>::root() const
{
    return root_;
}
#endif
