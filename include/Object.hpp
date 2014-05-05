#ifndef PILOT_OBJECT_HPP_
#define PILOT_OBJECT_HPP_

#include <cstddef>
#include <mutex>
#include <string>
#include <ode/ode.h>
#include <libguile.h>

std::string vector_to_string(const dVector3 v);
std::string quat_to_string(const dQuaternion q);

class World;

class Object {
public:
    typedef std::size_t Id;

    Object(
        World* world,
        Id id,
        const std::string& model,
        dWorld& dworld,
        const dVector3& position, const dVector3& linear_vel,
        const dQuaternion& quat, const dVector3& angular_vel);

    static Id scm_to_id(SCM id);

    static SCM scm_from_id(Id id);

    Id id() const {
        return id_;
    }

    const std::string& model() {
        return model_;
    }

    void update(double dt);

    void load_position(dVector3 position) const;

    void set_position(const dVector3& position);

    void set_velocity(const dVector3& velocity);

    void load_attitude(dQuaternion attitude) const;

    void set_attitude(const dQuaternion& attitude);

    void set_angular_velocity(const dVector3& velocity);

    std::string position_data() const;

    bool force_applied() const;

private:
    World* world_;
    Id id_;
    std::string model_;
    dBody dbody_;
    mutable std::string position_data_cache_;
    mutable std::recursive_mutex update_mutex_;
};

#endif
