#pragma once

#include <memory>


struct IServiceA {
    virtual int foo() = 0;
};
using ServiceAPtr = std::shared_ptr<IServiceA>;

struct IServiceB {
    virtual int foo() = 0;
};
using ServiceBPtr = std::shared_ptr<IServiceB>;

struct IServiceC {
    virtual int foo() = 0;
};
using ServiceCPtr = std::shared_ptr<IServiceC>;
