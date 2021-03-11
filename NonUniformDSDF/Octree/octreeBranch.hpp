#ifndef OCTREEBRANCH_H
#define OCTREEBRANCH_H

template< typename T, int P >
Octree<T, P>::Branch::Branch(int layer)
    : Node(BranchNode, layer)
{
    memset(children, 0, sizeof(children));
}


template< typename T, int P >
Octree<T, P>::Branch::Branch(const Branch& b)
    : Node(b)
{
	for (int i = 0; i < 8; i++)
	{
        this->child(i, b.child(i));
	}
}

template< typename T, int P >
typename Octree<T, P>::Branch& Octree<T, P>::Branch::operator= (const Branch& b)
{
    if (this == &b) return *this;
    Node::operator=(b);

    for (int i = 0; i < 8; i++) {
        this->child(i, b.child(i));
    }

    return *this;
}

template< typename T, int P >
const std::shared_ptr<typename Octree<T, P>::Node> Octree<T, P>::Branch::child(int x, int y, int z) const
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    return children[z][y][x];
}


template< typename T, int P >
typename std::shared_ptr<typename Octree<T, P>::Node> Octree<T, P>::Branch::child(int x, int y, int z)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    return children[z][y][x];
}

template< typename T, int P >
void Octree<T, P>::Branch::child(int x, int y, int z, std::shared_ptr<Node> newChild)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    newChild->parent(std::dynamic_pointer_cast<Branch>(this->shared_from_this()));
    newChild->indexInParent = z * 4 + y * 2 + x;
    std::swap(children[z][y][x], newChild);
}

template< typename T, int P >
const std::shared_ptr<typename Octree<T, P>::Node> Octree<T, P>::Branch::child(int index) const
{
    assert(index >= 0 && index < 8);
    return *(&children[0][0][0] + index);
}

template< typename T, int P >
std::shared_ptr<typename Octree<T, P>::Node> Octree<T, P>::Branch::child(int index)
{
    assert(index >= 0 && index < 8);
    return *(&children[0][0][0] + index);
}

template< typename T, int P >
void Octree<T, P>::Branch::child(int index, std::shared_ptr<Node> newChild)
{
    assert(index >= 0 && index < 8);
    newChild->parent(std::dynamic_pointer_cast<Branch>(this->shared_from_this()));
    newChild->indexInParent = index;
    newChild.swap(*(&children[0][0][0] + index));
}
#endif