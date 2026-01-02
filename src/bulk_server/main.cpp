#include <iostream>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "async.h"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, size_t bulk_size)
    : socket_(std::move(socket))
	, bulk_size_(bulk_size)
	{}

    void start()
	{
        context_ = connect(bulk_size_);
        do_read();
    }

    ~Session()
	{
        if (context_)
			disconnect(context_);
    }

private:
    void do_read()
	{
        auto self(shared_from_this());
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, [[maybe_unused]] std::size_t length) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string line;
                    std::getline(is, line);
                    
                    // Убираем \r если клиент из Windows
                    if (!line.empty() && line.back() == '\r')
						line.pop_back();

                    receive(context_, line.c_str(), line.size());
                    do_read();
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    size_t bulk_size_;
    void* context_ = nullptr;
};

class Server
{
public:
    Server(boost::asio::io_context& io_context, short port, size_t bulk_size)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
		, bulk_size_(bulk_size)
	{
        do_accept();
    }

private:
    void do_accept()
	{
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
          	if (!ec)
			{
                std::make_shared<Session>(std::move(socket), bulk_size_)->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    size_t bulk_size_;
};

int main(int argc, char* argv[])
{
    try
	{
        if (argc != 3)
		{
            std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
            return 1;
        }

        unsigned short port = static_cast<unsigned short>(std::atoi(argv[1]));
        size_t bulk_size = std::stoul(argv[2]);

        boost::asio::io_context io_context;
        Server s(io_context, port, bulk_size);
        io_context.run();
    }
	catch(std::exception& e)
	{
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
