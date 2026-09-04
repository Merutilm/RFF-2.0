//
// Created by Merutilm on 9/4/26.
//

#include "ClcSyncPresets.hpp"
namespace merutilm::rff2::ClcSyncPresets {

    std::string Fast::getName() const { return "Fast"; }
    FrtReferenceSyncSettings Fast::genRefSync() const {
        return FrtReferenceSyncSettings{
                .referenceSynchronizationInterval = 16,
                .referenceSynchronizationRadiusPower = 3,
        };
    }
    std::string Normal::getName() const { return "Normal"; }
    FrtReferenceSyncSettings Normal::genRefSync() const {
        return FrtReferenceSyncSettings{
                .referenceSynchronizationInterval = 4,
                .referenceSynchronizationRadiusPower = 2,
        };
    }
    std::string Best::getName() const { return "Best"; }
    FrtReferenceSyncSettings Best::genRefSync() const {
        return FrtReferenceSyncSettings{
                .referenceSynchronizationInterval = 0,
                .referenceSynchronizationRadiusPower = 1,
        };
    }
} // namespace merutilm::rff2::ClcSyncPresets
