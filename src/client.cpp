#include <thread>
#include "ClientApp.hpp"
#include "TcpClient.hpp"

#include <iostream>

int main() {
    std::thread main([]{ client_app().run(); });
    std::thread tcp([]{ tcp_client().connect("localhost", 2501); });
    main.join();
    tcp_client().stop();
    tcp.join();
}
