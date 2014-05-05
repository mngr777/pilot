#ifndef PILOT_CLIENT_TCP_HPP_
#define PILOT_CLIENT_TCP_HPP_

#include <condition_variable>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>

class TcpClient {
public:
    static TcpClient& instance() {
        static TcpClient client;
        return client;
    }

    void connect(std::string server, unsigned port);

    void stop();

    void add_message(const std::string& message);

private:
    TcpClient()
        : io_service_(),
          socket_(io_service_),
          write_in_progress_(false) {}

    void read();

    void writer();

    void handle_read(
        const boost::system::error_code& ec, std::size_t bytes_sent);

    void handle_write(
        const boost::system::error_code& ec, std::size_t bytes_sent);

    std::string wait_for_outbox_message();

    void send_message(std::string message);

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf response_;
    std::queue<std::string> outbox_;
    std::condition_variable outbox_cv_;
    bool write_in_progress_;
    std::condition_variable write_in_progress_cv_;
    std::mutex outbox_mutex_;
    std::mutex socket_mutex_;
};

TcpClient& tcp_client();

#endif
