#include "../inc/Server.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }

    int port = std::atoi(argv[1]);
    std::string password(argv[2]);

    Server server(port, password);
    if (!server.init()) {
        std::cout << "Failed to initialize server" << std::endl;
        return EXIT_FAILURE;
    }
    
    try {
        server.run();
    } catch (const std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception" << std::endl;
    }

    return EXIT_SUCCESS;
}
