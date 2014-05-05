#include "TcpClient.hpp"
#include <thread>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "ClientApp.hpp"

#include <chrono>

using boost::asio::ip::tcp;

void TcpClient::connect(std::string servername, unsigned port) {
    // resolve address
    tcp::resolver resolver(io_service_);
    tcp::resolver::query query(servername, boost::lexical_cast<std::string>(port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // connect
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end) {
        socket_.close();
        socket_.connect(*endpoint_iterator, error);
        ++endpoint_iterator;
    }
    if (error) throw boost::system::system_error(error);

    // start reading/writing
    read();
    std::thread writer_thread(&TcpClient::writer, this);
    auto work = std::make_shared<boost::asio::io_service::work>(io_service_);
    io_service_.run();
    // service stopped

    // wait for writer
    writer_thread.join();
    // close socket
    socket_.close();
}

void TcpClient::stop() {
    io_service_.stop();
    // wake all waiting writers
    outbox_cv_.notify_all();
}

void TcpClient::read() {
    if (io_service_.stopped()) return;

    std::lock_guard<std::mutex> lg(socket_mutex_);
    boost::asio::async_read_until(
        socket_,
        response_,
        "\n",
        boost::bind(&TcpClient::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void TcpClient::writer() {
    while (true) {
        std::string message = wait_for_outbox_message();
        if (io_service_.stopped()) return;
        send_message(message);
    }
}

void TcpClient::handle_read(
    const boost::system::error_code& ec, std::size_t bytes_sent)
{
    // read message
    std::istream is(&response_);
    std::string line;
    std::getline(is, line);
    // std::cout << line << std::endl;

    // recursive call
    read();

    // process message
    client_app().process_network_message(line);
}

void TcpClient::handle_write(
    const boost::system::error_code& ec, std::size_t bytes_sent)
{
    if (ec) {
        std::cerr << ec << std::endl;
    }
}

std::string TcpClient::wait_for_outbox_message() {
    // wait until message added or io_service stopped
    std::unique_lock<std::mutex> ul(outbox_mutex_);
    outbox_cv_.wait(ul, [this]{
            return !outbox_.empty() || io_service_.stopped(); });
    if (!io_service_.stopped()) {
        // fetch message
        std::string message = outbox_.front();
        outbox_.pop();
        return message;
    } else {
        // just return empty string
        return "";
    }
}

void TcpClient::send_message(std::string message) {
    std::lock_guard<std::mutex> lg(socket_mutex_);
    boost::asio::async_write(
        socket_, boost::asio::buffer(message + "\n"),
        boost::bind(&TcpClient::handle_write, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void TcpClient::add_message(const std::string& message) {
    std::lock_guard<std::mutex> lg(outbox_mutex_);
    outbox_.push(message);
    // wake writer
    outbox_cv_.notify_one();
}

TcpClient& tcp_client() {
    return TcpClient::instance();
}
