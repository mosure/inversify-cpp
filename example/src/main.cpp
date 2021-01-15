#include <mosure/inversify.hpp>

#include <api/symbols.hpp>

#include <src/logger.hpp>
#include <src/mock_logger.hpp>
#include <src/service.hpp>
#include <src/settings.hpp>


namespace inversify = mosure::inversify;

int main() {
    inversify::Container container {};

    container.bind<ILoggerPtr>(symbols::logger).to<Logger>().inSingletonScope();
    container.bind<IServicePtr>(symbols::service).to<Service>();
    container.bind<ISettings>(symbols::settings).to<Settings>().inSingletonScope();

    //container.bind<ILoggerPtr>(symbols::logger).to<MockLogger>().inSingletonScope();

    container.get<IServicePtr>(symbols::service)->run();

    return 0;
}
