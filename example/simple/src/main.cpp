#include <iostream>
#include <memory>
#include <utility>

#include <mosure/inversify.hpp>

#include <api/symbols.hpp>

#include <src/logger.hpp>
#include <src/mock_logger.hpp>
#include <src/service.hpp>
#include <src/settings.hpp>


namespace inversify = mosure::inversify;

struct RequestLog {
    RequestLog(ILoggerPtr primary, ILoggerPtr secondary)
        : primary_(std::move(primary)), secondary_(std::move(secondary))
    { }

    ILoggerPtr primary_;
    ILoggerPtr secondary_;
};

using RequestLogPtr = std::shared_ptr<RequestLog>;

namespace request_symbols {
    using batchedLog = inversify::Symbol<RequestLogPtr>;
}

template <>
struct inversify::Injectable<RequestLog>
    : inversify::Inject<
        symbols::logger,
        symbols::logger
    >
{ };

int main() {
    inversify::Container<
        symbols::logger,
        symbols::service,
        symbols::settings,
        request_symbols::batchedLog
    > container;

    container.bind<symbols::logger>().to<Logger>().inResolutionScope();
    container.bind<symbols::service>().to<Service>();
    container.bind<symbols::settings>().to<Settings>().inSingletonScope();
    container.bind<request_symbols::batchedLog>().to<RequestLog>();

    //container.bind<ILoggerPtr>(symbols::logger).to<MockLogger>().inSingletonScope();

    container.get<symbols::service>()->run();

    auto requestLog = container.get<request_symbols::batchedLog>();
    std::cout << std::boolalpha
              << "Resolution scoped logger reused within request: "
              << (requestLog->primary_ == requestLog->secondary_)
              << std::endl;

    return 0;
}
