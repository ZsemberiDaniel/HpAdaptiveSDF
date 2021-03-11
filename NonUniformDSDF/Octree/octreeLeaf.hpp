#ifndef OCTREELEAF_H
#define OCTRELEAF_H

template< typename T, int P >
Octree<T, P>::Leaf::Leaf(int layer)
    : Node(LeafNode, layer) { }

template< typename T, int P >
Octree<T, P>::Leaf::Leaf(const T& v, int layer)
    : Node(LeafNode, layer)
    , value_(v) { }


template< typename T, int P >
Octree<T, P>::Leaf::Leaf(const Leaf& b)
	: Node(b), value_(b.value())
{}

template< typename T, int P >
typename Octree<T, P>::Leaf& Octree<T, P>::Leaf::operator= (const Leaf& b)
{
    if (this == &b) return *this;
	
    Node::operator=(b);
    setValue(b.value());

    return *this;
}

template< typename T, int P >
const T& Octree<T, P>::Leaf::value() const
{
    return value_;
}

template< typename T, int P >
T& Octree<T, P>::Leaf::value()
{
    return value_;
}

template< typename T, int P >
void Octree<T, P>::Leaf::setValue(const T& v)
{
    value_ = v;
}

template< typename T, int P >
void Octree<T, P>::Leaf::subdivide(vector3d<T>& init, vector3d<std::shared_ptr<Leaf>>& newLeaves)
{
    std::shared_ptr<Branch> newBranch = std::make_shared<Branch>(this->layer());

    for (int x = 0; x < 2; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                // int index = (2 * x + i) + size_ * (2 * y + k) + size_ * size_ * (2 * z + j);
                newLeaves(x, y, z) = std::make_shared<Leaf>(init(x, y, z), this->layer() + 1);
                newBranch->child(x, y, z, std::static_pointer_cast<Node>(newLeaves(x, y, z)));
            }
        }
    }

    if (this->parent_)
    {
        std::dynamic_pointer_cast<Branch>(this->parent_)->child(this->indexInParent, std::static_pointer_cast<Node>(newBranch));
    }

    this->parent_ = nullptr;
    this->layer_ = -1;
}
#endif