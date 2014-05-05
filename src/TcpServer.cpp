#include "TcpServer.hpp"
#include <thread>
#include "App.hpp"
#include "ServerScm.hpp"

#include <chrono>

using boost::asio::ip::tcp;

void TcpSession::start() {
    // start reading/writing
    read();
    write();

    // add world update callback
    auto shared_this = shared_from_this();
    app().world().add_update_callback(
        [this, shared_this] {
            // wake up on world update
            outbox_cv_.notify_all(); });
}

void TcpSession::add_message(const std::string& message) {
    {
        std::lock_guard<std::mutex> lg(outbox_mutex_);
        outbox_.push(message);
    }
    outbox_cv_.notify_one();
}

void TcpSession::write() {
    std::string message = get_next_message();
    std::lock_guard<std::mutex> lg(socket_mutex_);
    boost::asio::async_write(
        socket_, boost::asio::buffer(message + "\n"),
        boost::bind(&TcpSession::handle_write, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

std::string TcpSession::get_next_message() {
    {
        // wait for data to send
        std::unique_lock<std::mutex> ul(outbox_mutex_);
        auto shared_this = shared_from_this();
        outbox_cv_.wait(ul, [this, shared_this] {
                return app().world().age() > world_age_
                    || !outbox_.empty();
            });
    }
    std::string message;
    std::lock_guard<std::mutex> lg(outbox_mutex_);
    if (!outbox_.empty()) {
        // fetch message from outbox
        message = outbox_.front();
        outbox_.pop();
    }
    // send next object data
    if (message.empty()) {
        message = get_next_object_data();
    }
    return message;
}

void TcpSession::read() {
    std::lock_guard<std::mutex> lg(socket_mutex_);
    boost::asio::async_read_until(
        socket_,
        response_,
        "\n",
        boost::bind(&TcpSession::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void TcpSession::handle_read(
    const boost::system::error_code& ec, std::size_t bytes_received)
{
    if (!ec) {
        // read message
        std::istream is(&response_);
        std::string line;
        std::getline(is, line);

        {
            std::lock_guard<std::mutex> lg(scm_mutex_);
            server_scm().process_network_message(shared_from_this(), line);
        }

        read();
    } else {
        // recursive call
        std::cerr << ec << std::endl;
    }
}

void TcpSession::handle_write(
    const boost::system::error_code& ec, std::size_t bytes_sent)
{
    if (!ec) {
        write();
    } else {
        std::cerr << ec << std::endl;
    }
}

std::string TcpSession::get_next_object_data() {
    std::string message;
    std::lock_guard<std::mutex> lg(world_data_mutex_);
    auto next_object = app().world().next_object(last_object_id_);
    double world_age = app().world().age();
    if (world_age != world_age_) {
        world_age_ = world_age;
        updated_object_ids_.clear();
    }
    if (updated_object_ids_.find(next_object->id())
        == updated_object_ids_.end()
    ) {
        message = next_object->position_data();
        updated_object_ids_.insert(next_object->id());
    }
    last_object_id_ = next_object->id();
    return message;
}

TcpServer::TcpServer()
    : io_service_(),
      acceptor_(io_service_, tcp::endpoint(tcp::v4(), 2501)),
      next_session_id_(1) {}

void TcpServer::run() {
    accept();
    auto work = std::make_shared<boost::asio::io_service::work>(io_service_);
    io_service_.run();
    std::cout << "io_service stopped" << std::endl;
}

void TcpServer::stop() {
}

void TcpServer::accept() {
    auto new_session =
        std::make_shared<TcpSession>(io_service_, next_session_id_++);
    acceptor_.async_accept(
        new_session->socket(),
        [this, new_session](const boost::system::error_code& ec) {
            if (!ec) {
                add_session(new_session);
                new_session->start();
                // std::cout << "session started" << std::endl << std::endl;

                accept();
            } else {
                std::cerr << ec << std::endl;
            }
        });
}

void TcpServer::add_session(std::shared_ptr<TcpSession> session) {
    std::lock_guard<std::mutex> lg(session_mutex_);
    sessions_[session->id()] = session;
}

std::shared_ptr<TcpSession> TcpServer::get_session(TcpSession::Id session_id) {
    std::lock_guard<std::mutex> lg(session_mutex_);
    auto it = sessions_.find(session_id);
    return (it == sessions_.end())
        ? nullptr
        : it->second;
}

TcpServer& tcp_server() {
    return TcpServer::instance();
}
