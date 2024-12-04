#include "mem/cache/replacement_policies/tree_plru_rp.hh"

#include <cmath>
#include "base/intmath.hh"
#include "base/logging.hh"
#include "params/TreePLRURP.hh"

namespace gem5
{

namespace replacement_policy
{

// Utility functions for parent, left, right subtree, etc.
static uint64_t parentIndex(const uint64_t index) { return std::floor((index - 1) / 2); }
static uint64_t leftSubtreeIndex(const uint64_t index) { return 2 * index + 1; }
static uint64_t rightSubtreeIndex(const uint64_t index) { return 2 * index + 2; }
static bool isRightSubtree(const uint64_t index) { return index % 2 == 0; }

TreePLRU::TreePLRUReplData::TreePLRUReplData(
    const uint64_t index, std::shared_ptr<PLRUTree> tree)
    : index(index), tree(tree), weight(1) // Initialize with default weight of 1
{
}

TreePLRU::TreePLRU(const Params &p)
    : Base(p), numLeaves(p.num_leaves), count(0), treeInstance(nullptr)
{
    fatal_if(!isPowerOf2(numLeaves),
             "Number of leaves must be non-zero and a power of 2");
}

void TreePLRU::invalidate(const std::shared_ptr<ReplacementData> &replacement_data)
{
    auto treePLRU_replacement_data =
        std::static_pointer_cast<TreePLRUReplData>(replacement_data);
    PLRUTree *tree = treePLRU_replacement_data->tree.get();
    uint64_t tree_index = treePLRU_replacement_data->index;

    // Parse and update tree to make it point to the new LRU
    do
    {
        const bool right = isRightSubtree(tree_index);
        tree_index = parentIndex(tree_index);
        tree->at(tree_index) = right;
    } while (tree_index != 0);
}

void TreePLRU::touch(const std::shared_ptr<ReplacementData> &replacement_data) const
{
    auto treePLRU_replacement_data =
        std::static_pointer_cast<TreePLRUReplData>(replacement_data);
    PLRUTree *tree = treePLRU_replacement_data->tree.get();
    uint64_t tree_index = treePLRU_replacement_data->index;

    // Increase weight on access
    treePLRU_replacement_data->weight += 1;

    // Update tree to make every bit point away from the new MRU
    do
    {
        const bool right = isRightSubtree(tree_index);
        tree_index = parentIndex(tree_index);
        tree->at(tree_index) = !right;
    } while (tree_index != 0);
}

void TreePLRU::reset(const std::shared_ptr<ReplacementData> &replacement_data) const
{
    touch(replacement_data); // Reset has the same functionality as touch
}

ReplaceableEntry *TreePLRU::getVictim(const ReplacementCandidates &candidates) const
{
    // Ensure there are candidates available
    assert(candidates.size() > 0);

    // Use the tree from the first candidate (all should share the same tree)
    const PLRUTree *tree = std::static_pointer_cast<TreePLRUReplData>(
                               candidates[0]->replacementData)
                               ->tree.get();
    uint64_t tree_index = 0;

    // Traverse the tree to locate the victim
    while (tree_index < tree->size())
    {
        tree_index = tree->at(tree_index) ? rightSubtreeIndex(tree_index) : leftSubtreeIndex(tree_index);
    }

    // Find the corresponding victim among the leaves
    auto victim = candidates[tree_index - (numLeaves - 1)];
    auto victim_weight = std::static_pointer_cast<TreePLRUReplData>(
                             victim->replacementData)
                             ->weight;

    // Compare weights of potential victims
    for (uint64_t i = tree_index - (numLeaves - 1); i < candidates.size(); ++i)
    {
        auto candidate = candidates[i];
        auto candidate_weight = std::static_pointer_cast<TreePLRUReplData>(
                                    candidate->replacementData)
                                    ->weight;

        if (candidate_weight < victim_weight)
        {
            victim = candidate;
            victim_weight = candidate_weight;
        }
    }

    return victim;
}

std::shared_ptr<ReplacementData> TreePLRU::instantiateEntry()
{
    if (count % numLeaves == 0)
    {
        treeInstance = new PLRUTree(numLeaves - 1, false);
    }

// Create a new entry and increment count
    auto treePLRUReplData = std::make_shared<TreePLRUReplData>(
        (count % numLeaves) + numLeaves - 1,
        std::shared_ptr<PLRUTree>(treeInstance));

    count++;
    return treePLRUReplData;
}

void TreePLRU::decayWeights(const ReplacementCandidates &candidates)
{
    // Apply decay factor to all entries
    for (auto &entry : candidates)
    {
        auto treePLRU_replacement_data = std::static_pointer_cast<TreePLRUReplData>(
            entry->replacementData);

        // Decay weight to prevent indefinite growth
        treePLRU_replacement_data->weight = std::max(1, treePLRU_replacement_data->weight / 2);
    }
}

} // namespace replacement_policy
} // namespace gem5