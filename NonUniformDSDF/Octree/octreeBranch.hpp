template< typename T, int P >
Octree<T, P>::Branch::Branch(int layer)
    : Node(BranchNode, layer)
{
    memset(children, 0, sizeof(children));
}

template< typename T, int P >
Octree<T, P>::Branch::Branch(const Branch& b)
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

template< typename T, int P >
Octree<T, P>::Branch::~Branch()
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

template< typename T, int P >
const typename Octree<T, P>::Node* Octree<T, P>::Branch::child(int x, int y, int z) const
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    return children[z][y][x];
}


template< typename T, int P >
typename Octree<T, P>::Node* Octree<T, P>::Branch::child(int x, int y, int z)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    return children[z][y][x];
}

template< typename T, int P >
void Octree<T, P>::Branch::child(int x, int y, int z, Node* newChild)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    newChild->parent(this);
    newChild->indexInParent = z * 4 + y * 2 + x;
    children[z][y][x] = newChild;
}

template< typename T, int P >
const typename Octree<T, P>::Node* Octree<T, P>::Branch::child(int index) const
{
    assert(index >= 0 && index < 8);
    return *(&children[0][0][0] + index);
}

template< typename T, int P >
typename Octree<T, P>::Node* Octree<T, P>::Branch::child(int index)
{
    assert(index >= 0 && index < 8);
    return *(&children[0][0][0] + index);
}

template< typename T, int P >
void Octree<T, P>::Branch::child(int index, Node* newChild)
{
    assert(index >= 0 && index < 8);
    newChild->parent(this);
    newChild->indexInParent = index;
    *(&children[0][0][0] + index) = newChild;
}