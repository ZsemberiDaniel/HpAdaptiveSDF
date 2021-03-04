
template< typename T, int P >
Octree<T, P>::Node::Node(NodeType type, int layer)
    : type_(type), layer_(layer)
{
}

template< typename T, int P >
Octree<T, P>::Node::Node(NodeType type)
    : type_(type)
{
}

template< typename T, int P >
typename Octree<T, P>::NodeType Octree<T, P>::Node::type() const
{
    return type_;
}