#include "mem/cache/replacement_policies/tree_plru_rp.hh"
#include "params/TreePLRURP.hh"
#include <limits> // For UINT64_MAX

namespace gem5
{

namespace replacement_policy
{

TreePLRU::TreePLRU(const TreePLRURPParams &p)
    : Base(p), numLeaves(p.num_leaves), count(0), agingThreshold(p.aging_threshold), treeInstance(nullptr)
{
    // Initialize the PLRU tree if needed.
}

void
TreePLRU::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
{
    auto data = std::static_pointer_cast<TreePLRUReplData>(replacement_data);
    data->agingCounter = 0;
    // Set the data entry as the next probable victim
    if (data->tree && data->index < data->tree->size()) {
        data->tree->at(data->index) = false;
    }
}

void
TreePLRU::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    auto data = std::static_pointer_cast<TreePLRUReplData>(replacement_data);
    data->agingCounter++;
    // Mark this entry as the most recently used in the tree
    if (data->tree && data->index < data->tree->size()) {
        data->tree->at(data->index) = true;
    }
}

void
TreePLRU::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    touch(replacement_data);
}

ReplaceableEntry*
TreePLRU::getVictim(const ReplacementCandidates& candidates) const
{
    ReplaceableEntry* victim = nullptr;
    uint64_t min_age = std::numeric_limits<uint64_t>::max();

    for (const auto& candidate : candidates) {
        auto data = std::static_pointer_cast<TreePLRUReplData>(candidate->replacementData);

        if (data->agingCounter < min_age && data->agingCounter <= agingThreshold) {
            min_age = data->agingCounter;
            victim = candidate;
        }
    }

    return victim ? victim : candidates.front();
}

std::shared_ptr<ReplacementData>
TreePLRU::instantiateEntry()
{
    // Allocate the tree if it's the first entry
    if (count == 0) {
        treeInstance = std::make_shared<PLRUTree>(numLeaves - 1, false);
    }

    auto repl_data = std::make_shared<TreePLRUReplData>(count++, treeInstance);

    if (count == numLeaves) {
        count = 0; // Reset count to reuse the tree
    }

    return repl_data;
}

} // namespace replacement_policy
} // namespace gem5