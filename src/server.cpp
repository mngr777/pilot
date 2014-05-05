#include <iostream>
#include <thread>
#include "App.hpp"
#include "TcpServer.hpp"

int main() {
    std::thread main([](){ app().run(); });
    std::thread server([](){ tcp_server().run(); });
    server.join();
    main.join();
}

