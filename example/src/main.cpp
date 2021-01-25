#include <mosure/inversify.hpp>

#include <api/symbols.hpp>

#include <src/logger.hpp>
#include <src/mock_logger.hpp>
#include <src/service.hpp>
#include <src/settings.hpp>


namespace inversify = mosure::inversify;

int main() {
    inversify::Container container;

    container.bind(symbols::logger).to<Logger>().inSingletonScope();
    container.bind(symbols::service).to<Service>();
    container.bind(symbols::settings).to<Settings>().inSingletonScope();

    //container.bind<ILoggerPtr>(symbols::logger).to<MockLogger>().inSingletonScope();

    container.get(symbols::service)->run();

    return 0;
}
