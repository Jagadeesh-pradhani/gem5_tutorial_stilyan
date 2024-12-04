#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_TREE_PLRU_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_TREE_PLRU_RP_HH__

#include <cstdint>
#include <memory>
#include <vector>
#include "mem/cache/replacement_policies/base.hh"

namespace gem5
{

struct TreePLRURPParams;

namespace replacement_policy
{

class TreePLRU : public Base // Inherit from Base class
{
private:
    typedef std::vector<bool> PLRUTree;

    const uint64_t numLeaves; // Initialize this in the constructor initializer list
    uint64_t count;
    std::shared_ptr<PLRUTree> treeInstance; // Use smart pointer to manage memory automatically
    uint64_t agingThreshold;

protected:
    struct TreePLRUReplData : public ReplacementData // Ensure it inherits ReplacementData
    {
        const uint64_t index;
        std::shared_ptr<PLRUTree> tree;
        uint64_t agingCounter;

        TreePLRUReplData(const uint64_t idx, std::shared_ptr<PLRUTree> tr)
            : index(idx), tree(std::move(tr)), agingCounter(0) {}
    };

public:
    typedef TreePLRURPParams Params;
    TreePLRU(const Params &p);
    ~TreePLRU() = default;

    // Override methods from the Base class
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data) override;
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const override;
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

} // namespace replacement_policy
} // namespace gem5

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_TREE_PLRU_RP_HH__