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
#include <boost/smart_ptr/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <thread>
#include <vector>
#include <bitset>


template<typename T>
static void printlog(const T& message) {
	std::cout << "[Server: " << message << "]" << std::endl;
}


void set_size_to_bit_array(std::uint16_t *write_size, const size_t& size) {
	std::memcpy(write_size, &size, sizeof(size_t));
}

size_t get_size_from_bit_array(std::uint16_t* write_size) {
	size_t return_size;
	std::memcpy(&return_size, write_size, sizeof(size_t));
	return return_size;
}


class Connect : public boost::enable_shared_from_this<Connect> {
public:
	Connect(boost::asio::ip::tcp::socket _socket, uint16_t _ID) : socket(std::move(_socket)), ID(_ID) {
	}
	void read();
	void send(const std::string& message);
private:
	std::string write_message;
	std::uint16_t write_size[sizeof(size_t)];
	std::uint16_t read_size[sizeof(size_t)];
	std::string read_message;
	boost::asio::ip::tcp::socket socket;
	uint16_t ID;

};


class Server : public boost::enable_shared_from_this<Server> {

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
