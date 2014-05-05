#include "ClientApp.hpp"
#include <boost/timer/timer.hpp>
#include <osg/Group>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include "ClientScm.hpp"

#include <chrono>
#include <thread>

ClientApp::ClientApp()
    : world_node_(new WorldNode()) {}

void ClientApp::run() {
    // set up viewer
    osgViewer::Viewer viewer;
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(world_node_);
    viewer.setSceneData(root);
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());

    // start timer
    boost::timer::cpu_timer timer;
    double time_prev = timer.elapsed().wall / 1e9;
    while (!viewer.done()) {
        viewer.frame();
        double time_curr = timer.elapsed().wall / 1e9;
        world_node_->update(time_curr - time_prev);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        time_prev = time_curr;
    }
}

void ClientApp::process_network_message(std::string message) {
    client_scm().process_network_message(std::move(message));
}

ClientApp& client_app() {
    return  ClientApp::instance();
}
