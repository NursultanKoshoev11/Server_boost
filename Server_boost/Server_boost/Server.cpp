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
		printlog("Server: Error waitclient!!!");
		return false;
	}

	printlog("Server: Stared!!!");
	return true;
}

void Server::Stop() {
	context.stop();
	if (context_thread.joinable())
		context_thread.join();
}

void Server::WaitClientConnect() {
	acceptor.async_accept(
		[this](std::error_code error, boost::asio::ip::tcp::socket socket) {
			if (error) {
				printlog("Server: Connect error!!!");
			}
			else {
				printlog("Server: New Client!!!");
				std::unique_ptr<Connect> tmp = std::make_unique<Connect>(std::move(socket), startID++);
				Clients_Array.push_back(std::move(tmp));
			}
			WaitClientConnect();
		});
}