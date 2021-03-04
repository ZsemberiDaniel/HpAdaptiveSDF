#pragma once
#include "Structures.h"

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

inline void makeBranchArray(const Octree<Cell>::Branch* currentBranch, std::vector<GPUBranch*>& branches, std::vector<GPULeaf*>& leaves,
    int& branchId, int& leafId)
{
    GPUBranch* gpuBranch = new GPUBranch();
    gpuBranch->level = currentBranch->layer();

    // leaf children will have negative ids for now since we do not know how many branches there are
    for (int i = 0; i < 8; i++)
    {
        const Octree<Cell>::Node* child = currentBranch->child(i);

        if (child->type() == Octree<Cell>::BranchNode)
        {
            makeBranchArray(static_cast<const Octree<Cell>::Branch*>(child), branches, leaves, branchId, leafId);

            // at this point the child branch will have an id, and that will be that last one
            // that has been dealt out so far, so it is the biggest one
            gpuBranch->childrenIndex[i] = branchId - 1;
        }
        else if (child->type() == Octree<Cell>::LeafNode)
        {
            // making leaf and adding it to leaves vector
            const auto* leaf = static_cast<const Octree<Cell>::Leaf*>(child);

            GPULeaf* gpuLeaf = new GPULeaf();
            gpuLeaf->level = leaf->layer();
            gpuLeaf->id = leafId++;
            gpuLeaf->value = &leaf->value();

            leaves.push_back(gpuLeaf);

            // assigning this child's index to parent
            gpuBranch->childrenIndex[i] = -static_cast<int>(gpuLeaf->id);
        }
    }

    // the branch only gets dealt an id after all of it's children have one
    gpuBranch->id = branchId++;
    branches.push_back(gpuBranch);
}

template< typename T, int P >
void Octree<T, P>::packForGPU(std::vector<unsigned int>& branchGPU, std::vector<unsigned int>& leavesGPU, int& branchCount)
{}

template< >
inline void Octree<Cell, 2>::packForGPU(std::vector<unsigned int>& branchGPU, std::vector<unsigned int>& leavesGPU, int& branchCount)
{
    std::vector<GPUBranch*> branches;
    std::vector<GPULeaf*> leaves;
    int leafId = 1;

    makeBranchArray(static_cast<const Branch*>(root()), branches, leaves, branchCount, leafId);

    // branch gpu storage has fixed size, leaves have variable size because of polynomials
    const int BR_SIZE = 1 + 8;
    branchGPU.resize(branches.size() * (BR_SIZE));
    leavesGPU.clear();

    glm::vec3 dir = glm::normalize(glm::vec3(2, 3, 4)) / 2.0f;
    Polynomial poly(1);
    poly(0) = 0;
    poly(1) = dir.z;
    poly(2) = dir.y;
    poly(3) = dir.x;

    // make leaves gpu storage and free up malloced memory
    for (int i = 0; i < leaves.size(); i++)
    {
        // mark where this starts in gpu storage
        leaves[i]->indexInGPUArray = leavesGPU.size();

        leavesGPU.push_back(leaves[i]->level);
        leavesGPU.push_back(leaves[i]->value->degree());
        // leavesGPU.push_back(1);

       /* int coeffCount = poly.coeffCount();
        for (int k = 0; k < coeffCount; k += 2)
        {

            glm::vec2 coeffVec2 = glm::vec2(0.0f);
            if (k + 0 < coeffCount) coeffVec2.x = poly(k + 0);
            if (k + 1 < coeffCount) coeffVec2.y = poly(k + 1);

            leavesGPU.push_back(glm::packSnorm2x16(coeffVec2));
        }*/

        int coeffCount = leaves[i]->value->poly.coeffCount();
        for (int k = 0; k < coeffCount; k += 2)
        {
            /*glm::vec4 coeffVec4 = glm::vec4(0.0f);
            if (k + 0 < coeffCount) coeffVec4.x = leaves[i]->value->poly(k + 0);
            if (k + 1 < coeffCount) coeffVec4.y = leaves[i]->value->poly(k + 1);
            if (k + 2 < coeffCount) coeffVec4.z = leaves[i]->value->poly(k + 2);
            if (k + 3 < coeffCount) coeffVec4.w = leaves[i]->value->poly(k + 3);
        	
            // DEBUG START
            coeffVec4.x = std::round(coeffVec4.x * 1000.0f) / 1000.0f;
            coeffVec4.y = std::round(coeffVec4.y * 1000.0f) / 1000.0f;
            coeffVec4.z = std::round(coeffVec4.z * 1000.0f) / 1000.0f;
            coeffVec4.w = std::round(coeffVec4.w * 1000.0f) / 1000.0f;
            // DEBUG END
        	
            leavesGPU.push_back(glm::packSnorm4x8(coeffVec4));*/

            glm::vec2 coeffVec2 = glm::vec2(0.0f);
            if (k + 0 < coeffCount) coeffVec2.x = leaves[i]->value->poly(k + 0);
            if (k + 1 < coeffCount) coeffVec2.y = leaves[i]->value->poly(k + 1);

            // DEBUG START
            //coeffVec2.x = std::round(coeffVec2.x * 1000.0f) / 1000.0f;
            // coeffVec2.y = std::round(coeffVec2.y * 1000.0f) / 1000.0f;
        	// DEBUG END

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