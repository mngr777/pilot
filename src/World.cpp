#include "World.hpp"

#include <iostream>
#include <random>

World::World()
    : age_(0.0),
      dworld_(),
      next_object_id_(1),
      next_callback_id_(1)
{
    dworld_.setCFM(1e-5);
}

void World::update(double dt) {
    {
        std::lock_guard<std::mutex> lg(update_mutex_);
        // update objects
        for (auto& p : objects_)
            p.second->update(dt);
        // update bodies
        dworld_.step(dt);
    }

    // update age
    {
        std::lock_guard<std::mutex> lg(age_mutex_);
        age_ += dt;
    }

    // run callbacks
    {
        std::lock_guard<std::mutex> lg(update_callback_mutex_);
        for (const auto& item : update_callbacks_)
            (item.second)();
    }
}

std::shared_ptr<Object> World::add_object(const std::string& model) {
    return add_object(
        model,
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}, {0.0, 0.0, 0.0});
}

std::shared_ptr<Object> World::add_object(
    const std::string& model,
    const dVector3& position, const dVector3& linear_vel,
    const dQuaternion& attitude, const dVector3& angular_vel)
{
    std::lock_guard<std::mutex> lg(update_mutex_);
    auto object = std::make_shared<Object>(
        this,
        next_object_id_++,
        model, dworld_,
        position, linear_vel,
        attitude, angular_vel);
    objects_[object->id()] = object;
    object_ids_.insert(object->id());
    return object;
}

std::shared_ptr<Object> World::get_object(Object::Id object_id) {
    std::lock_guard<std::mutex> lg(update_mutex_);
    auto it = objects_.find(object_id);
    return (it == objects_.end())
        ? nullptr
        : it->second;
}

std::shared_ptr<Object> World::next_object(Object::Id object_id) {
    std::lock_guard<std::mutex> lg(update_mutex_);
    if (object_ids_.empty())
        return nullptr;
    auto it = object_ids_.lower_bound(object_id + 1);
    if (it == object_ids_.end()) {
        return objects_[*(object_ids_.begin())];
    } else {
        return objects_[*it];
    }
}

World::CallbackId World::add_update_callback(Callback callback) {
    std::lock_guard<std::mutex> lg(update_callback_mutex_);
    update_callbacks_[next_callback_id_++] = callback;
    return next_callback_id_;
}

double World::age() const {
    std::lock_guard<std::mutex> lg(age_mutex_);
    return age_;
}
