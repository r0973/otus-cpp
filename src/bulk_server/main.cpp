#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <boost/asio.hpp>
#include "BulkProcessor.h"
#include "AsyncLoggerAdaptor.h"
#include "Dispatcher.h"

using boost::asio::ip::tcp;

// Глобальные объекты для ОБЩЕГО статического процессора
static std::shared_ptr<BulkProcessor> g_static_processor = nullptr;
static std::shared_ptr<Dispatcher> g_dispatcher = nullptr;
static std::mutex g_static_mutex;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, size_t bulk_size)
        : socket_(std::move(socket))
        , bulk_size_(bulk_size)
    {
        // Инициализируем общий статический процессор при первом соединении
        std::lock_guard<std::mutex> lock(g_static_mutex);
        if (!g_static_processor) {
            g_dispatcher = std::make_shared<Dispatcher>();
            g_static_processor = std::make_shared<BulkProcessor>(bulk_size);
            auto adapter = std::make_shared<AsyncLoggerAdapter>(g_dispatcher);
            g_static_processor->attach(adapter);
        }
    }

    void start()
    {
        do_read();
    }

    ~Session()
    {
        // Завершаем динамический блок если он активен
        if (in_dynamic_block_ && dynamic_processor_) {
            dynamic_processor_->Finish();
        }
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

                    process_line(line);
                    do_read();
                }
                // При закрытии соединения завершаем динамический блок
                else if (ec == boost::asio::error::eof || 
                         ec == boost::asio::error::connection_reset) {
                    if (in_dynamic_block_ && dynamic_processor_) {
                        dynamic_processor_->Finish();
                        in_dynamic_block_ = false;
                        dynamic_processor_.reset();
                    }
                }
            });
    }

    void process_line(const std::string& line)
    {
        if (line.empty()) return;

        if (line == "{") {
            // Начало динамического блока
            if (!in_dynamic_block_) {
                in_dynamic_block_ = true;
                dynamic_processor_ = std::make_shared<BulkProcessor>(bulk_size_);
                auto adapter = std::make_shared<AsyncLoggerAdapter>(std::make_shared<Dispatcher>());
                dynamic_processor_->attach(adapter);
            }
            dynamic_processor_->ProcessCommand(Command{line});
        }
        else if (line == "}") {
            // Конец динамического блока
            if (in_dynamic_block_ && dynamic_processor_) {
                dynamic_processor_->ProcessCommand(Command{line});
                if (--dynamic_nesting_level_ == 0) {
                    dynamic_processor_->Finish();
                    in_dynamic_block_ = false;
                    dynamic_processor_.reset();
                }
            }
        }
        else if (in_dynamic_block_) {
            // Команда внутри динамического блока
            if (dynamic_processor_) {
                dynamic_processor_->ProcessCommand(Command{line});
            }
        }
        else {
            // Статическая команда - используем ОБЩИЙ процессор
            std::lock_guard<std::mutex> lock(g_static_mutex);
            if (g_static_processor) {
                g_static_processor->ProcessCommand(Command{line});
            }
        }

        // Обработка вложенности динамических блоков
        if (line == "{") {
            dynamic_nesting_level_++;
        }
        else if (line == "}") {
            dynamic_nesting_level_ = std::max(0, dynamic_nesting_level_ - 1);
        }
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    size_t bulk_size_;
    
    // Состояние для динамических блоков
    bool in_dynamic_block_ = false;
    int dynamic_nesting_level_ = 0;
    std::shared_ptr<BulkProcessor> dynamic_processor_;
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
            if (!ec) {
                std::make_shared<Session>(std::move(socket), bulk_size_)->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    size_t bulk_size_;
};

// Глобальная функция для завершения статического процессора (при завершении сервера)
void finish_static_processor()
{
    std::lock_guard<std::mutex> lock(g_static_mutex);
    if (g_static_processor) {
        g_static_processor->Finish();
    }
}

int main(int argc, char* argv[])
{
    try {
        if (argc != 3) {
            std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
            return 1;
        }

        unsigned short port = static_cast<unsigned short>(std::atoi(argv[1]));
        size_t bulk_size = std::stoul(argv[2]);

        boost::asio::io_context io_context;
        Server s(io_context, port, bulk_size);
        
        // Обработка сигналов
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](boost::system::error_code, int) {
            finish_static_processor();
            io_context.stop();
        });

        io_context.run();
    }
    catch(std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}