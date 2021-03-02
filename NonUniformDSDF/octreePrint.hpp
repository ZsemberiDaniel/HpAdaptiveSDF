
template<typename T, int P>
void Octree<T, P>::print(glm::vec3 bboxMin, glm::vec3 bboxMax)
{
	std::cout << "Not yet implemented for this octree. See octreePrint.hpp!" << std::endl;
}

inline void printOctree(Octree<Cell>::Node* node, glm::vec3 bboxMin, glm::vec3 bboxMax)
{
	if (node->type() == Octree<Cell>::LeafNode)
	{
		std::cout << "Bbox " << bboxMin.x << " " << bboxMin.y << " " << bboxMin.z << " ---> "
			<< bboxMax.x << " " << bboxMax.y << " " << bboxMax.z << std::endl;
		static_cast<Octree<Cell>::Leaf*>(node)->value().poly.print();
		std::cout << std::endl;
	}
	else if (node->type() == Octree<Cell>::BranchNode)
	{
		auto* branch = static_cast<Octree<Cell>::Branch*>(node);

		for (int z = 0; z <= 1; z++)
		{
			for (int y = 0; y <= 1; y++)
			{
				for (int x = 0; x <= 1; x++)
				{
					int index = z * 4 + y * 2 + x;

					assert(branch->child(x, y, z) == branch->child(index));
					glm::vec3 bboxSizeHalf = (bboxMax - bboxMin) / 2.0f;

					glm::vec3 newBboxMin = glm::vec3(
						bboxMin.x + (x == 0 ? 0 : bboxSizeHalf.x),
						bboxMin.y + (y == 0 ? 0 : bboxSizeHalf.y),
						bboxMin.z + (z == 0 ? 0 : bboxSizeHalf.z)
					);
					glm::vec3 newBboxmax = glm::vec3(
						bboxMax.x - (x == 1 ? 0 : bboxSizeHalf.x),
						bboxMax.y - (y == 1 ? 0 : bboxSizeHalf.y),
						bboxMax.z - (z == 1 ? 0 : bboxSizeHalf.z)
					);

					printOctree(branch->child(x, y, z), newBboxMin, newBboxmax);
				}
			}
		}
	}
}

template<>
inline void Octree<Cell, 2>::print(glm::vec3 bboxMin, glm::vec3 bboxMax)
{
	printOctree(root(), bboxMin, bboxMax);
}