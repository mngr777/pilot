#include "Object.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <boost/lexical_cast.hpp>
#include "World.hpp"

#include <iostream>

std::string vector_to_string(const dVector3 v) {
    return std::string("(")
        + boost::lexical_cast<std::string>(v[0]) + " "
        + boost::lexical_cast<std::string>(v[1]) + " "
        + boost::lexical_cast<std::string>(v[2]) + ")";
}

std::string quat_to_string(const dQuaternion q) {
    return std::string("(")
        + boost::lexical_cast<std::string>(q[0]) + " "
        + boost::lexical_cast<std::string>(q[1]) + " "
        + boost::lexical_cast<std::string>(q[2]) + " "
        + boost::lexical_cast<std::string>(q[3]) + ")";
}

Object::Object(
    World* world,
    Id id,
    const std::string& model,
    dWorld& dworld,
    const dVector3& position, const dVector3& linear_vel,
    const dQuaternion& attitude, const dVector3& angular_vel)
    : world_(world),
      id_(id),
      model_(model),
      dbody_()
{
    dbody_.create(dworld);
    dbody_.setPosition(position);
    dbody_.setLinearVel(linear_vel);
    dbody_.setQuaternion(attitude);
    dbody_.setAngularVel(angular_vel);
    update(0.0);
}

Object::Id Object::scm_to_id(SCM id) {
    return scm_to_unsigned_integer(
        id, std::numeric_limits<Id>::min(), std::numeric_limits<Id>::max());
}

SCM Object::scm_from_id(Id id) {
    return scm_from_unsigned_integer(id);
}

void Object::update(double dt) {
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    position_data_cache_.clear();
}

void Object::load_position(dVector3 position) const {
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    auto p = dbody_.getPosition();
    std::copy(p, p + 3, position);
}

void Object::set_position(const dVector3& position) {
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    dbody_.setPosition(position);
}

void Object::set_velocity(const dVector3& velocity) {
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    dbody_.setLinearVel(velocity);
}

void Object::load_attitude(dQuaternion attitude) const {
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    auto q = dbody_.getQuaternion();
    std::copy(q, q + 4, attitude);
}

void Object::set_attitude(const dQuaternion& attitude) {
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    dbody_.setQuaternion(attitude);
}

void Object::set_angular_velocity(const dVector3& velocity) {
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    dbody_.setAngularVel(velocity);
}

std::string Object::position_data() const {
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    if (position_data_cache_.empty()) {
        position_data_cache_ = std::string("(op ")
            + boost::lexical_cast<std::string>(world_->age()) + " "
            + boost::lexical_cast<std::string>(id()) + " "
            + vector_to_string(dbody_.getPosition()) + " "
            + quat_to_string(dbody_.getQuaternion())
            + ")";
    }
    return position_data_cache_;
}

bool Object::force_applied() const {
    const double epsilon = 1e-9;
    std::lock_guard<std::recursive_mutex> lg(update_mutex_);
    const dReal* force = dbody_.getForce();
    for (unsigned i = 0; i < 3; ++i) {
        if (force[i] > epsilon)
            return true;
    }
    return false;
}
