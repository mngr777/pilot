#ifndef PILOT_CLIENT_APP_HPP_
#define PILOT_CLIENT_APP_HPP_

#include <string>
#include <osg/ref_ptr>
#include "WorldNode.hpp"

class ClientApp {
public:
    static ClientApp& instance() {
        static ClientApp app;
        return app;
    }

    void run();

    void process_network_message(std::string message);

    osg::ref_ptr<WorldNode> world_node() {
        return world_node_;
    }

private:
    ClientApp();

    ClientApp(const ClientApp&) = delete;
    ClientApp& operator=(const ClientApp&) = delete;

    osg::ref_ptr<WorldNode> world_node_;
};

ClientApp& client_app();

#endif
