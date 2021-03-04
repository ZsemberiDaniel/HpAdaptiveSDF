

template< typename T, int P >
Octree<T, P>::Leaf::Leaf(int layer)
    : Node(LeafNode, layer) { }

template< typename T, int P >
Octree<T, P>::Leaf::Leaf(const T& v, int layer)
    : Node(LeafNode, layer)
    , value_(v) { }

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
void Octree<T, P>::Leaf::subdivide(vector3d<T>& init, vector3d<Leaf*>& newLeaves)
{
    Branch* newBranch = new Branch(this->layer());

    for (int x = 0; x < 2; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                // int index = (2 * x + i) + size_ * (2 * y + k) + size_ * size_ * (2 * z + j);
                newLeaves(x, y, z) = new Leaf(init(x, y, z), this->layer() + 1);
                newBranch->child(x, y, z, newLeaves(x, y, z));
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