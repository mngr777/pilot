#include "WorldNode.hpp"
#include <boost/lexical_cast.hpp>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include "TcpClient.hpp"

void WorldNode::update(double dt) {
    for (auto& p : object_nodes_) {
        std::lock_guard<std::mutex> lg(update_mutex_);
        p.second->update(dt);
    }
}

osg::ref_ptr<ObjectNode> WorldNode::get_or_create_object_node(
    ::Object::Id object_id)
{
    auto it = object_nodes_.find(object_id);
    if (it == object_nodes_.end()) {
        osg::ref_ptr<ObjectNode> object_node = new ObjectNode();
        object_node->set_new_model_node(get_or_create_model_node(""));
        request_object_model(object_id);
        {
            std::lock_guard<std::mutex> lg(update_mutex_);
            object_nodes_[object_id] = object_node;
            addChild(object_node);
        }
        return object_node;
    } else {
        return it->second;
    }
}

void WorldNode::set_object_node_model(::Object::Id object_id, const std::string& filename) {
    auto object_node = get_or_create_object_node(object_id);
    auto model_node = get_or_create_model_node(filename);
    object_node->set_new_model_node(model_node);
}

osg::ref_ptr<osg::Node> WorldNode::get_or_create_model_node(
    std::string filename)
{
    auto it = model_nodes_.find(filename);
    if (it == model_nodes_.end()) {
        osg::ref_ptr<osg::Node> model_node(nullptr);
        if (filename.empty()) {
            // default
            osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere({0.0, 0.0, 0.0}, 5.0);
            osg::ref_ptr<osg::ShapeDrawable> drawable =
                new osg::ShapeDrawable(sphere);
            osg::ref_ptr<osg::Geode> geode = new osg::Geode();
            geode->addDrawable(drawable);
            model_node = geode;
        } else {
            // load from file
            model_node = osgDB::readNodeFile(std::string("data/models/") + filename);
        }

        {
            std::lock_guard<std::mutex> lg(model_mutex_);
            model_nodes_[filename] = model_node;
        }
        return model_node;

    } else {
        return it->second;
    }
}

void WorldNode::request_object_model(::Object::Id object_id) {
    std::string message = std::string("(om ")
        + boost::lexical_cast<std::string>(object_id) + ")";
    tcp_client().add_message(message);
}
