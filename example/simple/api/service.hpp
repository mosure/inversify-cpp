#pragma once

#include <memory>


struct IService {
    virtual void run() = 0;
};

using IServicePtr = std::shared_ptr<IService>;
