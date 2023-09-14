#pragma once
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/error.hpp>
#include <boost/system/error_code.hpp>
#include <thread>
#include <vector>


template<typename T>
void printlog(const T& message) {
	std::cout << "[" << message << "]" << std::endl;
}


class Connect {
public:
	Connect(boost::asio::ip::tcp::socket _socket, uint16_t _ID) : socket(std::move(_socket)), ID(_ID) {}
	boost::asio::ip::tcp::socket socket;
	uint16_t ID;
};


class Server {

public:
	Server(const uint16_t& port) : acceptor(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}
	~Server() { Stop(); }

	bool Start();

	void Stop();

private:	
	boost::asio::io_context context;
	boost::system::error_code error;
	boost::asio::ip::tcp::acceptor acceptor;
	std::thread context_thread;
	std::vector<std::unique_ptr<Connect>> Clients_Array;
	uint16_t startID = 1;

	void WaitClientConnect();

};