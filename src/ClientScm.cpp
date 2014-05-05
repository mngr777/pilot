#include "ClientScm.hpp"
#include "ClientApp.hpp"

#include <iostream>

ClientScm::ClientScm() {
    scm_with_guile(&register_functions, nullptr);
    load("scm/client_init.scm");
}

void ClientScm::set_object_position(
    Object::Id object_id, double t, const osg::Vec3d& position)
{
    client_app().world_node()
        ->get_or_create_object_node(object_id)
        ->set_new_position(position, t);
}

void ClientScm::set_object_attitude(
    Object::Id object_id, double t, const osg::Quat& attitude)
{
    client_app().world_node()
        ->get_or_create_object_node(object_id)
        ->set_new_attitude(attitude, t);
}

void ClientScm::set_object_model(
    Object::Id object_id, std::string model)
{
    client_app().world_node()->set_object_node_model(object_id, model);
}

void ClientScm::process_network_message(std::string message) {
    scm_with_guile(&do_process_network_message, &message);
}

std::string ClientScm::scm_to_std_string(SCM str) {
    char* c_str = scm_to_locale_string(str);
    std::string result(c_str);
    free(c_str);
    return result;
}

void ClientScm::load(const std::string& filename) {
    std::string filename_tmp(filename);
    scm_with_guile(&ClientScm::do_load, &filename_tmp);
}

void* ClientScm::do_load(void* data) {
    auto filename = static_cast<std::string*>(data);
    scm_c_primitive_load(filename->c_str());
    return nullptr;
}

void* ClientScm::do_process_network_message(void* data) {
    auto message = static_cast<std::string*>(data);
    scm_call_1(
        scm_variable_ref(scm_c_lookup("process-network-message")),
        scm_c_read_string(message->c_str()));
    return nullptr;
}

SCM ClientScm::scm_set_object_position(SCM object_id, SCM t, SCM x, SCM y, SCM z) {
    instance().set_object_position(
        Object::scm_to_id(object_id),
        scm_to_double(t),
        osg::Vec3d(scm_to_double(x), scm_to_double(y), scm_to_double(z)));
    return SCM_BOOL_T;
}

SCM ClientScm::scm_set_object_attitude(SCM object_id, SCM t, SCM x, SCM y, SCM z, SCM w) {
    instance().set_object_attitude(
        Object::scm_to_id(object_id),
        scm_to_double(t),
        osg::Quat(
            scm_to_double(x), scm_to_double(y), scm_to_double(z),
            scm_to_double(w)));
    return SCM_BOOL_T;
}

SCM ClientScm::scm_set_object_model(SCM object_id, SCM model) {
    instance().set_object_model(
        Object::scm_to_id(object_id),
        scm_to_std_string(model));
    return SCM_BOOL_T;
}

void* ClientScm::register_functions(void* data) {
    scm_c_define_gsubr(
        "set-object-position", 5, 0, 0,
        (scm_t_subr)&ClientScm::scm_set_object_position);

    scm_c_define_gsubr(
        "set-object-attitude", 6, 0, 0,
        (scm_t_subr)&ClientScm::scm_set_object_attitude);

    scm_c_define_gsubr(
        "set-object-model", 2, 0, 0,
        (scm_t_subr)&ClientScm::scm_set_object_model);

    return nullptr;
}

ClientScm& client_scm() {
    return ClientScm::instance();
}
