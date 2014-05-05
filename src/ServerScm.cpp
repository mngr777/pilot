#include "ServerScm.hpp"
#include <utility>
#include <boost/lexical_cast.hpp>
#include "App.hpp"

ServerScm::ServerScm() : current_session_(nullptr) {
    scm_with_guile(&register_functions, nullptr);
    load("scm/server_init.scm");
}

void ServerScm::load_world(const std::string& world) {
    // TODO: clear current world
    load(std::string("data/worlds/" + world));
}

Object::Id ServerScm::add_object(const std::string& model) {
    auto object = app().world().add_object(model);
    return object->id();
}

void ServerScm::set_object_position(
    Object::Id object_id, const dVector3& position)
{
    auto object = app().world().get_object(object_id);
    if (object) object->set_position(position);
}

void ServerScm::set_object_velocity(
    Object::Id object_id, const dVector3& velocity)
{
    auto object = app().world().get_object(object_id);
    if (object) object->set_velocity(velocity);
}

void ServerScm::set_object_attitude(
    Object::Id object_id, const dQuaternion& attitude)
{
    auto object = app().world().get_object(object_id);
    if (object) object->set_attitude(attitude);
}

void ServerScm::set_object_angular_velocity(
    Object::Id object_id, const dVector3& angular_velocity)
{
    auto object = app().world().get_object(object_id);
    if (object) object->set_angular_velocity(angular_velocity);
}

void ServerScm::process_network_message(
    std::shared_ptr<TcpSession> session, std::string message)
{
    current_session_ = session;
    scm_with_guile(&do_process_network_message, &message);
}

void ServerScm::send_object_model(Object::Id object_id) {
    if (!current_session_) return;
    auto object = app().world().get_object(object_id);
    if (object) {
        std::string message = std::string("(om ")
            + boost::lexical_cast<std::string>(object->id()) + " "
            + "\"" + object->model() + "\")";
        current_session_->add_message(message);
    }
}

std::string ServerScm::scm_to_std_string(SCM str) {
    char* c_str = scm_to_locale_string(str);
    std::string result(c_str);
    free(c_str);
    return result;
}

void* ServerScm::do_load(void* data) {
    auto filename = static_cast<std::string*>(data);
    scm_c_primitive_load(filename->c_str());
    return nullptr;
}

void* ServerScm::do_process_network_message(void* data) {
    auto message = static_cast<std::string*>(data);
    scm_call_1(
        scm_variable_ref(scm_c_lookup("process-network-message")),
        scm_c_read_string(message->c_str()));
    return nullptr;
}

SCM ServerScm::scm_send_object_model(SCM object_id) {
    instance().send_object_model(Object::scm_to_id(object_id));
    return SCM_BOOL_T;
}

SCM ServerScm::scm_add_object(SCM model) {
    Object::Id object_id = instance().add_object(scm_to_std_string(model));
    return Object::scm_from_id(object_id);
}

SCM ServerScm::scm_set_object_position(SCM object_id, SCM x, SCM y, SCM z) {
    instance().set_object_position(
        Object::scm_to_id(object_id),
        {scm_to_double(x), scm_to_double(y), scm_to_double(z)});
    return SCM_BOOL_T;
}

SCM ServerScm::scm_set_object_velocity(SCM object_id, SCM x, SCM y, SCM z) {
    instance().set_object_velocity(
        Object::scm_to_id(object_id),
        {scm_to_double(x), scm_to_double(y), scm_to_double(z)});
    return SCM_BOOL_T;
}

SCM ServerScm::scm_set_object_attitude(
    SCM object_id,
    SCM x, SCM y, SCM z, SCM w)
{
    instance().set_object_attitude(
        Object::scm_to_id(object_id),
        {scm_to_double(x), scm_to_double(y), scm_to_double(z), scm_to_double(w)});
    return SCM_BOOL_T;
}

SCM ServerScm::scm_set_object_angular_velocity(
        SCM object_id, SCM x, SCM y, SCM z)
{
    instance().set_object_angular_velocity(
        Object::scm_to_id(object_id),
        {scm_to_double(x), scm_to_double(y), scm_to_double(z)});
    return SCM_BOOL_T;
}

void* ServerScm::register_functions(void* data) {
    scm_c_define_gsubr(
        "send-object-model", 1, 0, 0,
        (scm_t_subr)&ServerScm::scm_send_object_model);

    scm_c_define_gsubr(
        "add-object", 1, 0, 0,
        (scm_t_subr)&ServerScm::scm_add_object);

    scm_c_define_gsubr(
        "set-object-position", 4, 0, 0,
        (scm_t_subr)&ServerScm::scm_set_object_position);

    scm_c_define_gsubr(
        "set-object-velocity", 4, 0, 0,
        (scm_t_subr)&ServerScm::scm_set_object_velocity);

    scm_c_define_gsubr(
        "set-object-attitude", 5, 0, 0,
        (scm_t_subr)&ServerScm::scm_set_object_attitude);

    scm_c_define_gsubr(
        "set-object-angular-velocity", 4, 0, 0,
        (scm_t_subr)&ServerScm::scm_set_object_angular_velocity);

    return nullptr;
}

void ServerScm::load(const std::string& filename) {
    std::string filename_tmp(filename);
    scm_with_guile(&ServerScm::do_load, &filename_tmp);
}

ServerScm& server_scm() {
    return ServerScm::instance();
}
