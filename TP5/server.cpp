#include "Server.h"
#include <iostream>
#include <string>
#include <boost\bind.hpp>

#define NOT_FOUND (x) ("HTTP/1.1 404 Not Found\nDate:"+(x)+"\nCache-Control: public, max-age=30\nExpires: Date + 30s (Ej: Tue, 04 Sep 2018 18:21:49 GMT)\nContent-Length: 0\nContent-Type: text/html; charset=iso-8859-1")


using boost::asio::ip::tcp;
std::string make_string(char * path);
Server::Server(boost::asio::io_context& io_context)

	:	context_(io_context),
	acceptor_(io_context,tcp::endpoint(tcp::v4(),13)),
	socket_(io_context)
{
}

Server::~Server()
{}


void Server::start() {
	if (socket_.is_open()) {
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket_.close();
	}
	start_waiting_connection();
}

void Server::start_waiting_connection()
{
	std::cout << "start_waiting_connection()" << std::endl;
	if (socket_.is_open()) 
	{
		std::cout << "Error: can't accept new connection from an open socket" << std::endl;
	}
	acceptor_.async_accept(
		socket_,
		boost::bind(
			&Server::connection_received_cb,
			this,
			boost::asio::placeholders::error
		)
	);
} 


void Server::start_answering()
{
	std::cout << "start_answering()" << std::endl;
	msg = make_string();
	boost::asio::async_write(
		socket_,
		boost::asio::buffer(msg),
		boost::bind(
			&Server::response_sent_cb,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket_.close();
}


void Server::connection_received_cb(const boost::system::error_code& error) {
	std::cout << "connection_received_cb" << std::endl;
	if (!error) {
		start_answering();
		start_waiting_connection();
	}
	else {
		std::cout << error.message() << std::endl;
	}
}

void Server::response_sent_cb(const boost::system::error_code& error, size_t bytes_sent)
{
	std::cout << "response_sent_cb()" << std::endl;
	if (!error)
	{
		std::cout << "Response sent" << bytes_sent << "bytes" << std::endl;
	}
}

std::string make_string(char * path)//cambiar a lo que querramos hacer
{
#pragma warning(disable : 4996)
	using namespace std;
	FILE* p;
	p = fopen(path, "r");
	time_t now = time(0);
	if (p = NULL) {
		return NOT_FOUND(ctime(&now));
	}
	else {
		return FOUND;
	}

}