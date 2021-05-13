#ifndef OCTREEPACK_H
#define OCTREEPACK_H
#include "../Structures.h"
#include <algorithm>

struct GPUBranch
{
    unsigned int id;
    int level;
    int childrenIndex[8];
};

struct GPULeaf
{
    unsigned int id;
    unsigned int indexInGPUArray = -1;
    int level;
    const Cell* value;
};

inline void makeBranchArray(std::shared_ptr<Octree<Cell>::Branch>& currentBranch, std::vector<GPUBranch*>& branches, std::vector<GPULeaf*>& leaves,
    int& branchId, int& leafId, float& maxCoeff, int& maxLeafDepth)
{
    GPUBranch* gpuBranch = new GPUBranch();
    gpuBranch->level = currentBranch->layer();

    // leaf children will have negative ids for now since we do not know how many branches there are
    for (int i = 0; i < 8; i++)
    {
        std::shared_ptr<Octree<Cell>::Node> child = currentBranch->child(i);

        if (child->type() == Octree<Cell>::BranchNode)
        {
            std::shared_ptr<Octree<Cell>::Branch> branch = std::dynamic_pointer_cast<Octree<Cell>::Branch>(child);
            makeBranchArray(branch, branches, leaves, branchId, leafId, maxCoeff, maxLeafDepth);

            // at this point the child branch will have an id, and that will be that last one
            // that has been dealt out so far, so it is the biggest one
            gpuBranch->childrenIndex[i] = branchId - 1;
        }
        else if (child->type() == Octree<Cell>::LeafNode)
        {
            // making leaf and adding it to leaves vector
            std::shared_ptr<Octree<Cell>::Leaf> leaf = std::dynamic_pointer_cast<Octree<Cell>::Leaf>(child);

            maxLeafDepth = std::max(leaf->layer(), maxLeafDepth);

            GPULeaf* gpuLeaf = new GPULeaf();
            gpuLeaf->level = leaf->layer();
            gpuLeaf->id = leafId++;
            gpuLeaf->value = &leaf->value();

            leaves.push_back(gpuLeaf);

            maxCoeff = std::max(maxCoeff, gpuLeaf->value->poly.coeffMaxValue());

            // assigning this child's index to parent
            gpuBranch->childrenIndex[i] = -static_cast<int>(gpuLeaf->id);
        }
    }

    // the branch only gets dealt an id after all of it's children have one
    gpuBranch->id = branchId++;
    branches.push_back(gpuBranch);
}

template< typename T, int P >
void Octree<T, P>::generate3DLookupTable(std::vector<df::integral<uint32_t>>& lookupTable, int& lookupTableDim1Size) { }

template< >
inline void Octree<Cell, 2>::generate3DLookupTable(std::vector<df::integral<uint32_t>>& lookupTable, int& lookupTableDim1Size)
{
    // generate branches and leaves vector, we only care about leaves now
    std::vector<GPUBranch*> branches;
    std::vector<GPULeaf*> leaves;

    int maxLeafDepth = -1;
    int leafId = 1;
    float maxCoeff = -std::numeric_limits<float>::max();
    int branchCount = 0;
    std::shared_ptr<Branch> branch = std::dynamic_pointer_cast<Branch>(root());
    makeBranchArray(branch, branches, leaves, branchCount, leafId, maxCoeff, maxLeafDepth);

    // init lookup table
    lookupTableDim1Size = 1 << maxLeafDepth;
    lookupTable.resize(lookupTableDim1Size * lookupTableDim1Size * lookupTableDim1Size);

    // all dimensions should be the same, calculate smallest cell size
    const float smallestCellSize = bboxSizeInWorld() / lookupTableDim1Size;

    // setter for convenience
    auto setLookupTableVal = [&lookupTable, lookupTableDim1Size](int x, int y, int z, df::integral<uint32_t> val)
    {
        lookupTable[lookupTableDim1Size * lookupTableDim1Size * z + lookupTableDim1Size * y + x] = val;
    };

    // do not generate the leavesGPU array, but rather generate where the leaves are on the gpu array
    uint32_t pointer = 0;
    for (int i = 0; i < leaves.size(); i++)
    {
        // where the cell starts in cell indices and where it ends
        glm::ivec3 fromIndices = glm::ivec3(leaves[i]->value->bbox.min / smallestCellSize);
        glm::ivec3 toIndices = glm::ivec3(leaves[i]->value->bbox.max / smallestCellSize);

        // set all from start to finish to point to pointer
        for (int x = fromIndices.x; x < toIndices.x; x++)
        {
            for (int y = fromIndices.y; y < toIndices.y; y++)
            {
                for (int z = fromIndices.z; z < toIndices.z; z++)
                {
                    setLookupTableVal(x, y, z, df::integral<uint32_t> { pointer });
                }
            }
        }

        int coeffCount = leaves[i]->value->poly.coeffCount();
        pointer += (int) std::ceil(coeffCount / 2.0f) + 5;
    }

    // delete branches
    for (int i = 0; i < branches.size(); i++) delete branches[i];

    // delete leaves
    for (int i = 0; i < leaves.size(); i++) delete leaves[i];
}

template< typename T, int P >
void Octree<T, P>::packForGPU(std::vector<unsigned int>& branchGPU, std::vector<unsigned int>& leavesGPU, int& branchCount, float& compressAmount)
{}

template< >
inline void Octree<Cell, 2>::packForGPU(std::vector<unsigned int>& branchGPU, 
                                        std::vector<unsigned int>& leavesGPU,
                                        int& branchCount, float& compressAmount)
{
    std::vector<GPUBranch*> branches;
    std::vector<GPULeaf*> leaves;

    int maxLeafDepth = -1;
    int leafId = 1;
    float maxCoeff = -std::numeric_limits<float>::max();
    branchCount = 0;
    std::shared_ptr<Branch> branch = std::dynamic_pointer_cast<Branch>(root());
    makeBranchArray(branch, branches, leaves, branchCount, leafId, maxCoeff, maxLeafDepth);
    compressAmount = std::max(1.0f, maxCoeff);

    // branch gpu storage has fixed size, leaves have variable size because of polynomials
    const int BR_SIZE = 1 + 8;
    branchGPU.resize(branches.size() * (BR_SIZE));
    leavesGPU.clear();

    // make leaves gpu storage and free up malloced memory
    for (int i = 0; i < leaves.size(); i++)
    {
        // mark where this starts in gpu storage
        leaves[i]->indexInGPUArray = static_cast<unsigned int>(leavesGPU.size());

        glm::vec3 leafMinPos = leaves[i]->value->bbox.min;
        leavesGPU.push_back(leaves[i]->level);
        leavesGPU.push_back(glm::floatBitsToUint(leafMinPos.x));
        leavesGPU.push_back(glm::floatBitsToUint(leafMinPos.y));
        leavesGPU.push_back(glm::floatBitsToUint(leafMinPos.z));
        leavesGPU.push_back(leaves[i]->value->degree());

        int coeffCount = leaves[i]->value->poly.coeffCount();
        for (int k = 0; k < coeffCount; k += 2)
        {
            glm::vec2 coeffVec2 = glm::vec2(0.0f);
            if (k + 0 < coeffCount) coeffVec2.x = leaves[i]->value->poly(k + 0) / compressAmount;
            if (k + 1 < coeffCount) coeffVec2.y = leaves[i]->value->poly(k + 1) / compressAmount;

            if (abs(coeffVec2.x) < 0.0001f) coeffVec2.x = 0.0f;
            if (abs(coeffVec2.y) < 0.0001f) coeffVec2.y = 0.0f;

            leavesGPU.push_back(glm::packSnorm2x16(coeffVec2));
        }
    }

    // make branches gpu storage and free up malloced memory
    for (int i = 0; i < branches.size(); i++)
    {
        branchGPU[BR_SIZE * i + 0] = branches[i]->level;
        for (int k = 1; k <= 8; k++)
        {
            branchGPU[BR_SIZE * i + k] = static_cast<unsigned int>(
                branches[i]->childrenIndex[k - 1] < 0 ? // is it a leaf?
                //                                              + 1 since leaf id starts at 1 so negative trick can be used
                leaves[abs(branches[i]->childrenIndex[k - 1] + 1)]->indexInGPUArray + branches.size() : // if so the index needs to be shifted
                branches[i]->childrenIndex[k - 1]);
        }

        delete branches[i];
    }

    // delete leaves
    for (int i = 0; i < leaves.size(); i++) delete leaves[i];
}
#endif
