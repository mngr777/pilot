#include "ObjectNode.hpp"

#include <iostream>

const ObjectNode::PositionBuffer::size_type ObjectNode::PositionBufferSize = 3;

void ObjectNode::update(double dt) {
    std::lock_guard<std::mutex> lg(update_mutex_);
    // position
    if (new_position_set_) {
        setPosition(new_position_);
        new_position_set_ = false;
    } else if (position_buffer_.size() > 1) {
        // auto position = guess_next_position(dt);
        // store_position(position, last_position_time_ + dt);
    }

    // attitude
    if (new_attitude_set_) {
        setAttitude(new_attitude_);
        new_attitude_set_ = false;
    }

    // object model
    if (new_model_node_) {
        insertChild(0, new_model_node_);
        new_model_node_ = nullptr;
    }
}

void ObjectNode::set_new_position(const osg::Vec3d& position, double t) {
    std::lock_guard<std::mutex> lg(update_mutex_);
    new_position_ = position;
    new_position_set_ = true;

    position_buffer_.push_back(std::make_pair(position, t));
    if (position_buffer_.size() > PositionBufferSize) {}
        position_buffer_.pop_front();
}

void ObjectNode::set_new_attitude(const osg::Quat& attitude, double t) {
    std::lock_guard<std::mutex> lg(update_mutex_);
    new_attitude_ = attitude;
    new_attitude_set_ = true;
}

void ObjectNode::set_new_model_node(osg::ref_ptr<osg::Node> model_node) {
    std::lock_guard<std::mutex> lg(update_mutex_);
    new_model_node_ = model_node;
}

