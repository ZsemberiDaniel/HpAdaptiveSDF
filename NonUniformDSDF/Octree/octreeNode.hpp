#ifndef OCTREENODE_H
#define OCTREENODE_H
template< typename T, int P >
Octree<T, P>::Node::Node(NodeType type, int layer)
    : layer_(layer), type_(type)
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

template< typename T, int P >
Octree<T, P>::Node::Node(const Node& b)
	: indexInParent(b.indexInParent), layer_(b.layer_), parent_(b.parent_), type_(b.type_)
{
	
}

template< typename T, int P >
typename Octree<T, P>::Node& Octree<T, P>::Node::operator= (const Node& b)
{
	if (this == &b) return *this;
	
	indexInParent = b.indexInParent;
	layer_ = b.layer_;
	parent_ = b.parent_;
	type_ = b.type_;

	return *this;
}
#endif