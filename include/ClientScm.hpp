#ifndef PILOT_CLIENT_SCM_HPP_
#define PILOT_CLIENT_SCM_HPP_

#include <string>
#include <libguile.h>
#include <osg/Quat>
#include <osg/Vec3d>
#include "Object.hpp"

class ClientScm {
public:
    static ClientScm& instance() {
        static ClientScm scm;
        return scm;
    }

    void set_object_position(
        Object::Id object_id, double t,  const osg::Vec3d& position);

    void set_object_attitude(
        Object::Id object_id, double t, const osg::Quat& attitude);

    void set_object_model(
        Object::Id object_id, std::string model);

    void process_network_message(std::string message);

    static std::string scm_to_std_string(SCM str);

private:
    ClientScm();

    void load(const std::string& filename);

    ClientScm(const ClientScm&) = delete;
    ClientScm& operator=(const ClientScm&) = delete;

    static void* do_load(void* data);

    static void* do_process_network_message(void* data);

    static SCM scm_set_object_position(
        SCM object_id, SCM t, SCM x, SCM y, SCM z);

    static SCM scm_set_object_attitude(
        SCM object_id, SCM t, SCM x, SCM y, SCM z, SCM w);

    static SCM scm_set_object_model(SCM object_id, SCM model);

    static void* register_functions(void* data);
};

ClientScm& client_scm();

#endif
