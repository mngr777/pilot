#ifndef PILOT_APP_HPP_
#define PILOT_APP_HPP_

#include "World.hpp"

class App {
public:
    static App& instance() {
        static App app;
        return app;
    }

    void run();

    void stop();

    World& world() {
        return world_;
    }

private:
    App() : world_() {}

    App(const App&);
    void operator=(const App&);

    World world_;
};

App& app();

#endif
