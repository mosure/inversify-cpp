#include <mosure/inversify.hpp>

#include <api/types.hpp>

#include <src/logger.hpp>
#include <src/mock_logger.hpp>
#include <src/service.hpp>
#include <src/settings.hpp>


namespace inversify = mosure::inversify;

int main() {
    inversify::Container container {};

    container.bind<ILoggerPtr>(types::logger).to<Logger>().inSingletonScope();
    container.bind<IServicePtr>(types::service).to<Service>();
    container.bind<ISettings>(types::settings).to<Settings>().inSingletonScope();

    //container.bind<ILoggerPtr>(types::logger).to<MockLogger>().inSingletonScope();

    container.get<IServicePtr>(types::service)->run();

    return 0;
}
