#include "App.hpp"
#include <boost/timer/timer.hpp>

#include <chrono>
#include <thread>
#include "ServerScm.hpp"

void App::run() {
    server_scm().load_world("test.scm");

    boost::timer::cpu_timer timer;
    double time_prev = timer.elapsed().wall / 1e9;

    while (true) {
        // update world
        double time_curr = timer.elapsed().wall / 1e9;
        world_.update(time_curr - time_prev);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        time_prev = time_curr;
    }
}

App& app() {
    return App::instance();
}
