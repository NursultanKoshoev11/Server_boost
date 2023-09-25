#include "Server.h"
#include <condition_variable>
#include <mutex>



int main() {
	Server server(1000);

	server.Start();

	std::mutex m_mutex;
	std::condition_variable c_variable;

	while (true)
	{
		std::unique_lock<std::mutex> m_lock(m_mutex);
		c_variable.wait(m_lock);	
	}

	return 0;
}


bool Server::Start() {
	try
	{
		WaitClientConnect();

		context_thread = std::thread([this]() { context.run(); });
	}
	catch (const std::exception&)
	{
		printlog("Error waitclient!!!");
		return false;
	}

	printlog("Stared!!!");
	return true;
}

void Server::Stop() {
	context.stop();
	if (context_thread.joinable())
		context_thread.join();
}

void Connect::send(const std::string& message) {
	write_message = message;
	set_size_to_bit_array(write_size, write_message.size());
	boost::asio::async_write(socket, boost::asio::buffer(write_size),
		[this](const boost::system::error_code& error, size_t bytes_transferred) {
			if (!error) {
				boost::asio::async_write(socket, boost::asio::buffer(write_message), 
					[](const boost::system::error_code& error_, size_t bytes_transferred_) {
						if (!error_) {
							printlog("Success send");
						}
						else {
							printlog("Error by send message");
						}
					});
			}
			else {
				printlog("Error by send size");
			}
		});
}

void Server::WaitClientConnect() {
	acceptor.async_accept(
		[this](std::error_code error, boost::asio::ip::tcp::socket socket) {
			if (error) {
				printlog("Connect error!!!");
			}
			else {
				printlog("New Client!!!");
				std::unique_ptr<Connect> tmp = std::make_unique<Connect>(std::move(socket), startID++);
				tmp->read();
				Clients_Array.push_back(std::move(tmp));
				
			}
			
			WaitClientConnect();
		});
}

void Connect::read() {
	if (socket.is_open()) {
		boost::asio::async_read(socket, boost::asio::buffer(read_size),
			[this](const boost::system::error_code& error, std::size_t bytes_transferred) {
				if (!error) {
					size_t size = get_size_from_bit_array(read_size);
					write_message.resize(size);
					boost::asio::async_read(socket, boost::asio::buffer(write_message),
						[this](const boost::system::error_code& error_, std::size_t bytes_transferred_) {
							if (!error_) {
								printlog(write_message);
							}
							else {
								printlog("Error by read message");
							}
						});
				}
				else {
					printlog("Error by read size");
				}
			});
	}
	else {
		printlog("Socket close");
	}
}

