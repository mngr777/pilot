#ifndef PILOT_NETWORK_HPP_
#define PILOT_NETWORK_HPP_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Object.hpp"
#include "World.hpp"

class TcpSession : public std::enable_shared_from_this<TcpSession> {
public:
    typedef std::size_t Id;

    TcpSession(boost::asio::io_service& io_service, Id id)
        : id_(id),
          socket_(io_service),
          world_age_(0.0),
          last_object_id_(0),
          response_() {}

    void start();

    void add_message(const std::string& message);

    std::string get_next_message();

    boost::asio::ip::tcp::socket& socket() {
        return socket_;
    }

    Id id() const {
        return id_;
    }

private:
    void write();

    void read();

    void handle_write(
        const boost::system::error_code& ec, std::size_t bytes_sent);

    void handle_read(
        const boost::system::error_code& ec, std::size_t bytes_received);

    std::string get_next_object_data();

    Id id_;
    boost::asio::ip::tcp::socket socket_;
    double world_age_;
    Object::Id last_object_id_;
    std::unordered_set<Object::Id> updated_object_ids_;
    boost::asio::streambuf response_;
    std::queue<std::string> outbox_;
    std::condition_variable outbox_cv_;
    std::mutex world_data_mutex_;
    std::mutex socket_mutex_;
    std::mutex outbox_mutex_;
    std::mutex scm_mutex_;
};

class TcpServer {
public:
    static TcpServer& instance() {
        static TcpServer tcp_server;
        return tcp_server;
    }

    void run();

    void stop();

private:
    TcpServer();

    void accept();

    void add_session(std::shared_ptr<TcpSession> session);

    std::shared_ptr<TcpSession> get_session(TcpSession::Id session_id);

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<TcpSession::Id, std::shared_ptr<TcpSession>> sessions_;
    TcpSession::Id next_session_id_;
    std::mutex session_mutex_;
};

TcpServer& tcp_server();

#endif
