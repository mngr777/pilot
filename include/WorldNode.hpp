#ifndef PILOT_WORLD_NODE_HPP_
#define PILOT_WORLD_NODE_HPP_

#include <mutex>
#include <string>
#include <unordered_map>
#include <osg/Group>
#include "Object.hpp"
#include "ObjectNode.hpp"

class WorldNode : public osg::Group {
public:
    void update(double dt);

    osg::ref_ptr<ObjectNode> get_or_create_object_node(::Object::Id object_id);

    void set_object_node_model(::Object::Id object_id, const std::string& filename);

private:
    typedef std::unordered_map<::Object::Id, osg::ref_ptr<ObjectNode>>
        ObjectNodeMap;
    typedef std::unordered_map<std::string, osg::ref_ptr<osg::Node>>
        ModelNodeMap;

    osg::ref_ptr<osg::Node> get_or_create_model_node(std::string filename);

    void request_object_model(::Object::Id object_id);

    ObjectNodeMap object_nodes_;
    ModelNodeMap model_nodes_;
    std::mutex update_mutex_;
    std::mutex model_mutex_;
};

#endif
