#ifndef PILOT_SERVER_SCM_HPP_
#define PILOT_SERVER_SCM_HPP_

#include <string>
#include <libguile.h>
#include "Object.hpp"
#include "TcpServer.hpp"

class ServerScm {
public:
    static ServerScm& instance() {
        static ServerScm scm;
        return scm;
    }

    void load_world(const std::string& world);

    Object::Id add_object(const std::string& model);

    void set_object_position(Object::Id object_id, const dVector3& position);

    void set_object_velocity(Object::Id object_id, const dVector3& velocity);

    void set_object_attitude(Object::Id object_id, const dQuaternion& attitude);

    void set_object_angular_velocity(
        Object::Id object_id, const dVector3& angular_velocity);

    void process_network_message(
        std::shared_ptr<TcpSession> session, std::string message);

    void send_object_model(Object::Id object_id);

    static std::string scm_to_std_string(SCM str);

private:
    ServerScm();

    ServerScm(const ServerScm&) = delete;
    ServerScm& operator=(const ServerScm&) = delete;

    static void* do_load(void* data);

    static void* do_process_network_message(void* data);

    static SCM scm_add_object(SCM model);

    static SCM scm_set_object_position(SCM object_id, SCM x, SCM y, SCM z);

    static SCM scm_set_object_velocity(SCM object_id, SCM x, SCM y, SCM z);

    static SCM scm_set_object_attitude(
        SCM object_id,
        SCM x, SCM y, SCM z, SCM w);

    static SCM scm_set_object_angular_velocity(
        SCM object_id, SCM x, SCM y, SCM z);

    static SCM scm_send_object_model(SCM object_id);

    static void* register_functions(void* data);

    void load(const std::string& filename);

    std::shared_ptr<TcpSession> current_session_;
};

ServerScm& server_scm();

#endif
