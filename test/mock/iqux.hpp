#pragma once

#include <memory>


struct IQux {
    virtual ~IQux() = default;

    virtual int buzz() = 0;
};

using IQuxUniquePtr = std::unique_ptr<IQux>;
using IQuxSharedPtr = std::shared_ptr<IQux>;
