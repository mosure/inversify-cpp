#pragma once

#include <memory>


struct IFizz {
    virtual ~IFizz() = default;

    virtual int buzz() = 0;

    int counter { 0 };
};

using IFizzUniquePtr = std::unique_ptr<IFizz>;
using IFizzSharedPtr = std::shared_ptr<IFizz>;
