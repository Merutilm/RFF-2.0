//
// Created by Merutilm on 2025-07-18.
//

#pragma once

namespace merutilm::vkh {

    struct InterfaceRepoImpl {

        virtual ~InterfaceRepoImpl() = default;

    };

    using InterfaceRepo = std::unique_ptr<InterfaceRepoImpl>;
    using InterfaceRepoPtr = InterfaceRepoImpl *;
    using InterfaceRepoRef = InterfaceRepoImpl &;
}
