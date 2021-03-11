#ifndef OCTREEPACK_H
#define OCTREEPACK_H
#include "../Structures.h"

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
    int& branchId, int& leafId, float& maxCoeff)
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
            makeBranchArray(branch, branches, leaves, branchId, leafId, maxCoeff);

            // at this point the child branch will have an id, and that will be that last one
            // that has been dealt out so far, so it is the biggest one
            gpuBranch->childrenIndex[i] = branchId - 1;
        }
        else if (child->type() == Octree<Cell>::LeafNode)
        {
            // making leaf and adding it to leaves vector
            std::shared_ptr<Octree<Cell>::Leaf> leaf = std::dynamic_pointer_cast<Octree<Cell>::Leaf>(child);

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
void Octree<T, P>::packForGPU(std::vector<unsigned int>& branchGPU, std::vector<unsigned int>& leavesGPU, int& branchCount, float& compressAmount)
{}

template< >
inline void Octree<Cell, 2>::packForGPU(std::vector<unsigned int>& branchGPU, std::vector<unsigned int>& leavesGPU, int& branchCount,
    float& compressAmount)
{
    std::vector<GPUBranch*> branches;
    std::vector<GPULeaf*> leaves;
    int leafId = 1;

    float maxCoeff = -std::numeric_limits<float>::max();
    branchCount = 0;
    std::shared_ptr<Branch> branch = std::dynamic_pointer_cast<Branch>(root());
    makeBranchArray(branch, branches, leaves, branchCount, leafId, maxCoeff);
    compressAmount = std::max(1.0f, maxCoeff);

    // branch gpu storage has fixed size, leaves have variable size because of polynomials
    const int BR_SIZE = 1 + 8;
    branchGPU.resize(branches.size() * (BR_SIZE));
    leavesGPU.clear();

    // make leaves gpu storage and free up malloced memory
    for (int i = 0; i < leaves.size(); i++)
    {
        // mark where this starts in gpu storage
        leaves[i]->indexInGPUArray = leavesGPU.size();

        leavesGPU.push_back(leaves[i]->level);
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
            branchGPU[BR_SIZE * i + k] =
                branches[i]->childrenIndex[k - 1] < 0 ? // is it a leaf?
                //                                              + 1 since leaf id starts at 1 so negative trick can be used
                leaves[abs(branches[i]->childrenIndex[k - 1] + 1)]->indexInGPUArray + branches.size() : // if so the index needs to be shifted
                branches[i]->childrenIndex[k - 1];
        }

        delete branches[i];
    }

    // delete leaves
    for (int i = 0; i < leaves.size(); i++) delete leaves[i];
}
#endif
