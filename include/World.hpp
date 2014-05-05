#ifndef PILOT_WORLD_HPP_
#define PILOT_WORLD_HPP_

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <ode/ode.h>
#include "Object.hpp"

class World {
public:
    typedef std::size_t CallbackId;
    typedef std::function<void(void)> Callback;

    World();

    void update(double dt);

    std::shared_ptr<Object> add_object(const std::string& model);

    std::shared_ptr<Object> add_object(
        const std::string& model,
        const dVector3& position, const dVector3& linear_vel,
        const dQuaternion& attitude, const dVector3& angular_vel);

    std::shared_ptr<Object> get_object(Object::Id object_id);

    std::shared_ptr<Object> next_object(Object::Id object_id);

    CallbackId add_update_callback(Callback callback);

    double age() const;

private:
    typedef std::unordered_map<Object::Id, std::shared_ptr<Object>>
        ObjectMap;

    double age_;
    dWorld dworld_;
    ObjectMap objects_;
    Object::Id next_object_id_;
    std::set<Object::Id> object_ids_;
    std::map<CallbackId, Callback> update_callbacks_;
    CallbackId next_callback_id_;
    mutable std::mutex update_mutex_;
    mutable std::mutex age_mutex_;
    std::mutex update_callback_mutex_;
};

#endif
