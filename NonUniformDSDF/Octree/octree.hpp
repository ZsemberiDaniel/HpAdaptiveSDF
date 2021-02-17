#include <cstring>

/**
 * \param size Size of octree, in nodes. Should be a power of two. For example,
 * an octree with \a size = 256 will represent a cube divided into 256x256x256
 * nodes. <b>Must be a power of two.</b>
 * 
 * \param init a size x size x size 3D array with which the tree can be initialized.
 */
template< typename T, int AS >
Octree<T, AS>::Octree(int size, vector3d<T>& init, vector3d<Leaf*>& generatedLeaves)
    : root_(0)
    , size_(size)
{
    // Make sure size is power of two.
    assert(((size - 1) & size) == 0);
    assert(init.dim1() == size);
    assert(init.dim2() == size);
    assert(init.dim3() == size);

    if (size <= 1) 
    {
        root_ = new Leaf(0);
        return;
    }
    else 
    {
        root_ = new Branch(0);
    }

    // push the branch nodes into a queue and take them out to add children to them
    // while we reach the leaves layer
    std::queue<Branch*> nodes;
    nodes.push(reinterpret_cast<Branch*>(root_));

    int layerAt = 0;
    while (size > 2)
    {
        while (nodes.front()->layer() == layerAt)
        {
            Branch* n = nodes.front(); nodes.pop();
            
            for (int i = 0; i < 2; i++)
            {
                for (int k = 0; k < 2; k++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        n->child(i, k, j, new Branch(n->layer() + 1));
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
template< typename T, int AS >
void Octree<T, AS>::initLeavesRecursive(Node* currNode, vector3d<T>& init, vector3d<Leaf*>& generatedLeaves, int x, int y, int z)
{
    if (currNode == nullptr || currNode->type() == NodeType::LeafNode)
    {
        return;
    }

    Branch* currBranch = reinterpret_cast<Branch*>(currNode);
    for (int i = 0; i < 2; i++)
    {
        for (int k = 0; k < 2; k++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (currBranch->child(i, j, k) == nullptr)
                {
                    // int index = (2 * x + i) + size_ * (2 * y + k) + size_ * size_ * (2 * z + j);
                    generatedLeaves(2 * x + i, 2 * y + k, 2 * z + j) = new Leaf(
                        init(2 * x + i, 2 * y + k, 2 * z + j),
                        currBranch->layer() + 1);
                    currBranch->child(i, j, k, generatedLeaves(2 * x + i, 2 * y + k, 2 * z + j));
                }
                else
                {
                    initLeavesRecursive(currBranch->child(i, j, k), init, generatedLeaves, 2 * x + i, 2 * y + k, 2 * z + j);
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
template< typename T, int AS >
Octree<T, AS>::Octree(const Octree<T, AS>& o)
    : size_(o.size_)
{
    if (!o.root_) {
        root_ = 0;
    }
    else {
        switch (o.root_->type()) {
        case BranchNode:
            root_ = new Branch(*reinterpret_cast<Branch*>(o.root_));
            break;
        case LeafNode:
            root_ = new Leaf(*reinterpret_cast<Leaf*>(o.root_));
            break;
        }
    }
}

/**
 * Recursively deletes all nodes by following branch pointers.
 */
template< typename T, int AS >
Octree<T, AS>::~Octree()
{
    deleteNode(root_);
}

/**
 * Swaps the octree's contents with another's. This is a cheap operation as only
 * the root pointers are swapped, not the whole structure.
 */
template< typename T, int AS >
void Octree<T, AS>::swap(Octree<T, AS>& o)
{
    std::swap(root_, o.root_);
    std::swap(size_, o.size_);
}

/**
 * Assigns to this octree the contents of octree \a o.
 */
template< typename T, int AS >
Octree<T, AS>& Octree<T, AS>::operator= (Octree<T, AS> o)
{
    swap(o);
    return *this;
}

/**
 * \return Size of octree, in nodes, as specified in the constructor.
 */
template< typename T, int AS >
int Octree<T, AS>::size() const
{
    return size_;
}

/**
 * Deletes a node polymorphically. If the node is a branch node, it will delete
 * all its subtree recursively.
 */
template< typename T, int AS >
void Octree<T, AS>::deleteNode(Node*& node)
{
    if (node) {
        if ((node)->type() == BranchNode) {
            delete reinterpret_cast<Branch*>(node);
        }
        else {
            assert((node)->type() == LeafNode);
            delete reinterpret_cast<Leaf*>(node);
        }
        // *node = nullptr;
    }
}

/**
 * \return Pointer to octree's root node.
 */
template< typename T, int AS >
typename Octree<T, AS>::Node*& Octree<T, AS>::root()
{
    return root_;
}

/**
 * Const version of above.
 */
template< typename T, int AS >
const typename Octree<T, AS>::Node* Octree<T, AS>::root() const
{
    return root_;
}

/**
 * \return Total number of nodes in the octree.
 */
template< typename T, int AS >
int Octree<T, AS>::nodes() const
{
    return nodesRecursive(root_);
}

/**
 * Helper function for nodes() method.
 */
template< typename T, int AS >
int Octree<T, AS>::nodesRecursive(const Node* node)
{
    if (!node) {
        return 0;
    }

    int n = 1;
    if (node->type() == BranchNode) {
        for (int i = 0; i < 8; ++i) {
            n += nodesRecursive(
                reinterpret_cast<const Branch*>(node)->child(i));
        }
    }

    return n;
}

template< typename T, int AS >
Octree<T, AS>::Node::Node(NodeType type, int layer)
    : type_(type), layer_(layer)
{
}

template< typename T, int AS >
Octree<T, AS>::Node::Node(NodeType type)
    : type_(type)
{
}

template< typename T, int AS >
typename Octree<T, AS>::NodeType Octree<T, AS>::Node::type() const
{
    return type_;
}

template< typename T, int AS >
Octree<T, AS>::Branch::Branch(int layer)
    : Node(BranchNode, layer)
{
    memset(children, 0, sizeof(children));
}

template< typename T, int AS >
Octree<T, AS>::Branch::Branch(const Branch& b)
    : Node(BranchNode, b.layer(), b.parent())
{
    for (int i = 0; i < 8; ++i) {
        if (b.child(i)) {
            switch (b.child(i)->type()) {
            case BranchNode:
                child(i) = new Branch(
                    *reinterpret_cast<const Branch*>(b.child(i)));
                break;
            case LeafNode:
                child(i) = new Leaf(
                    *reinterpret_cast<const Leaf*>(b.child(i)));
                break;
            }
        }
        else {
            child(i) = 0;
        }
    }
}

template< typename T, int AS >
Octree<T, AS>::Branch::~Branch()
{
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                assert(children[i][j][k] != this);
                deleteNode(children[i][j][k]);
            }
        }
    }
}

template< typename T, int AS >
const typename Octree<T, AS>::Node* Octree<T, AS>::Branch::child(int x, int y, int z) const
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    return children[z][y][x];
}


template< typename T, int AS >
typename Octree<T, AS>::Node* Octree<T, AS>::Branch::child(int x, int y, int z)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    return children[z][y][x];
}

template< typename T, int AS >
typename void Octree<T, AS>::Branch::child(int x, int y, int z, Node* newChild)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    newChild->parent(this);
    newChild->indexInParent = z * 4 + y * 2 + x;
    children[z][y][x] = newChild;
}

template< typename T, int AS >
const typename Octree<T, AS>::Node* Octree<T, AS>::Branch::child(int index) const
{
    assert(index >= 0 && index < 8);
    return *(&children[0][0][0] + index);
}

template< typename T, int AS >
typename Octree<T, AS>::Node* Octree<T, AS>::Branch::child(int index)
{
    assert(index >= 0 && index < 8);
    return *(&children[0][0][0] + index);
}

template< typename T, int AS >
typename void Octree<T, AS>::Branch::child(int index, Node* newChild)
{
    assert(index >= 0 && index < 8);
    newChild->parent(this);
    newChild->indexInParent = index;
    *(&children[0][0][0] + index) = newChild;
}

template< typename T, int AS >
Octree<T, AS>::Leaf::Leaf(int layer)
    : Node(LeafNode, layer) { }

template< typename T, int AS >
Octree<T, AS>::Leaf::Leaf(const T& v, int layer)
    : Node(LeafNode, layer)
    , value_(v) { }

template< typename T, int AS >
const T& Octree<T, AS>::Leaf::value() const
{
    return value_;
}

template< typename T, int AS >
T& Octree<T, AS>::Leaf::value()
{
    return value_;
}

template< typename T, int AS >
void Octree<T, AS>::Leaf::setValue(const T& v)
{
    value_ = v;
}

template< typename T, int AS >
void Octree<T, AS>::Leaf::subdivide(vector3d<T>& init, vector3d<Leaf*>& newLeaves)
{
    Branch* newBranch = new Branch(this->layer());

    for (int i = 0; i < 2; i++)
    {
        for (int k = 0; k < 2; k++)
        {
            for (int j = 0; j < 2; j++)
            {
                // int index = (2 * x + i) + size_ * (2 * y + k) + size_ * size_ * (2 * z + j);
                newLeaves(i, k, j) = new Leaf(init(i, k, j), this->layer() + 1);
                newBranch->child(i, k, j, newLeaves(i, k, j));
            }
        }
    }

    if (this->parent_)
    {
        reinterpret_cast<Branch*>(this->parent_)->child(this->indexInParent, newBranch);
    }

    this->parent_ = nullptr;
    this->layer_ = -1;
    delete this;
}