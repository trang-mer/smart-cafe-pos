#include "Server.h"
#include "Logger.h"

int main() {
    int port = 8080;

    Server server(port);

    if (!server.start()) {
        Logger::error("Cannot start server");
        return 1;
    }

    server.run();

    return 0;
}